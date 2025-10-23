use std::fs;
use std::process::Command;
use std::sync::Arc;
use std::path::PathBuf;
use std::time::Instant;

use git2::Repository;
use scopeguard::defer;
use indicatif::MultiProgress;
use colored::*;


use crate::spinner::Spinner;
use crate::config::{Config, ConfigFile, ProjectConfig};
use crate::args::{AddTarget, BinaryType, CMakeOptions, NewProject, NewSyrisProject};
use crate::{Result,utils};
use crate::cmake;
use crate::modules;


pub async fn update_modules(config : ProjectConfig, config_file : Option<ConfigFile>, multi : Arc<MultiProgress>) -> Result<()>{
    let spinner = Spinner::new("Updating modules...", Some(multi.clone()));
    defer!(spinner.finish(););
    let config_file = match config_file {
        Some(config_file) => config_file,
        None => {
            ConfigFile::new_from_file(&config)?
        }
    };
    if !fs::exists(&config.project_paths.modules)?{
        fs::create_dir(&config.project_paths.modules)?;
    }
    let modules_dir_list = fs::read_dir(&config.project_paths.modules).map_err(|e| format!("Failed to read directory modules: {e}"))?;
    let mut current_modules_libraries = Vec::new();
    let git_repo = Repository::open(&config.project_paths.root).map_err(|e| format!("Failed to open git repository: {e}"))?;
    let submodules = git_repo.submodules().map_err(|e| format!("Failed to get git submodules: {e}"))?;
    for entry in modules_dir_list{
        let entry = entry.map_err(|e| format!("Failed to read entry: {e}"))?;
        if entry.path().is_dir(){
            if let Some(name) = entry.file_name().to_str(){
                current_modules_libraries.push(name.to_string());
            }
        }
    }

    if let Some(modules) = &config_file.modules{
        for current_modules_lib in &current_modules_libraries{
            if !modules.contains_key(current_modules_lib){
                if let Ok(submodule) = git_repo.find_submodule(format!("modules/{}", &current_modules_lib).as_str()){
                    spinner.change_message(format!("Updating modules: Removing module {}...", current_modules_lib));
                    let output = Command::new("git")
                        .args(["submodule", "deinit", "-f", "--", format!("submodule.modules/{current_modules_lib}").as_str()])
                        .current_dir(&config.project_paths.root)
                        .output()
                        .map_err(|e| format!("Failed to run command: {e}"))?;

                    if !output.status.success(){
                        Err(format!("Failed to deinit submodule {current_modules_lib}: {}", String::from_utf8_lossy(&output.stderr)))?;
                    }
                    
                    let output = Command::new("git")
                        .args(["config", "f", ".git/config", "--remove-section", format!("submodule.modules/{current_modules_lib}").as_str()])
                        .current_dir(&config.project_paths.root)
                        .output()
                        .map_err(|e| format!("failed to run command: {e}"))?;
                    
                    if !output.status.success(){
                        Err(format!("Failed to remove submodule {current_modules_lib} from git config: {}", String::from_utf8_lossy(&output.stderr)))?;
                    }
                    spinner.change_message("Updating modules...");
                }
                fs::remove_dir_all(config.project_paths.modules.join(current_modules_lib)).map_err(|e| format!("Failed to remove {current_modules_lib} modules: {e}"))?;
            }
        }
        for (name, module) in modules{
            if !current_modules_libraries.contains(name){
                modules::add_module_to_modules_dir(name.as_str(), module, &git_repo, &config, multi.clone())?;
            }
        }
    }
    Ok(())
}


pub async fn init_vs_conf(config : ProjectConfig, project : NewSyrisProject, multi : Arc<MultiProgress>) -> Result<()>{
    let spinner = Spinner::new("setting vs conf...", Some(multi.clone()));
    let vs_conf_src = config.asharis_root.join(".vscode");
    utils::copy_dir_rec(&vs_conf_src, &config.project_paths.root).map_err(|e| format!("Failed to copy /.vscode config: {e}"))?;
    spinner.finish();
    Ok(())
}

pub async fn init_source(config : ProjectConfig, project : NewProject, multi : Arc<MultiProgress>) -> Result<()>{
    let spinner  = Spinner::new("setting src contents...", Some(multi.clone()));
    fs::create_dir(config.project_paths.src.join(&project.name)).map_err(|e| format!("Failed to create src dir: {e}"))?;
    let template_main_src = config.asharis_root.join("resources").join("new_project").join("TemplateMain.cpp");
    let template_main_dst = config.project_paths.src.join(&project.name).join("main.cpp");
    fs::copy(template_main_src,template_main_dst ).map_err(|e| format!("Failed to copy template main.cpp to src/{}/main.cpp: {e}", &project.name))?;
    let project_config_file_src = config.asharis_root.join("resources").join("new_project").join("TemplateConfig.yaml");
    let project_config_file_dst = &config.project_paths.config_file;
    let contents = fs::read_to_string(project_config_file_src).map_err(|e| format!("Failed to read config.yaml: {e}"))?;

    let modified_content = contents.replace(config.flags.project_name, &project.name);
    fs::write(project_config_file_dst, modified_content).map_err(|e| format!("Failed to write modified src cmakelists: {e}"))?;
    spinner.finish();
    Ok(())
}

pub async fn pip_glad_install(config : &ProjectConfig, multi : Arc<MultiProgress>) -> Result<()>{
    // Step 1: Create and activate the virtual environment
    let spinner = Spinner::new("generating glad files", Some(multi));
    //println!("Creating virtual environment...");
    let folders_to_glad = ["modules", "syris", "modules", "glad"];
    let path_to_glad = folders_to_glad.iter().fold(config.project_paths.root.clone(), |path, folder| path.join(folder));
    spinner.change_message("creating python venv");
    let outupt = Command::new("python")
        .args(["-m", "venv", "venv"])
        .current_dir(&path_to_glad)
        .output()
        .map_err(|e| format!("Failed to run command: {e}"))?;
    if !outupt.status.success(){
        Err("failed to create python env")?;
    }

    // Step 2: Install glad2 in the virtual environment
    spinner.change_message("installing glad2..");
    let output = Command::new("pip")
        .args(["install", "glad2"])
        .current_dir(path_to_glad.join("venv").join("Scripts"))
        .output()
        .map_err(|e| format!("Failed to run command: {e}"))?;
    if !output.status.success(){
        Err("failed to create python env")?;
    }

    // Step 3: Run glad2 to generate files
    spinner.change_message("generating glad2 files with glad2 generator...");
    let output = Command::new("python")
        .args(["-m", "glad", "--api", "gl:core=4.6", "--out-path", path_to_glad.join("src").to_str().unwrap(), "c"])
        .current_dir(path_to_glad.join("venv").join("Scripts"))
        .output()
        .map_err(|e| format!("Failed to run command: {e}"))?;
    if !output.status.success(){
        Err(format!("failed to generate glad2, [{}]", String::from_utf8_lossy(&output.stderr)))?;
    }

    // Step 4: Clean up or notify user
    spinner.change_message("removing python venv...");
    fs::remove_dir_all(path_to_glad.join("venv")).map_err(|e| format!("Failed to remove python venv directory: {e}"))?;
    /*let glad_cmakelists_file_src = config.asharis_root.join("resources").join("GladCMakeLists.txt");
    let glad_cmakelists_file_dst = path_to_glad.join("CMakeLists.txt");

    fs::copy(glad_cmakelists_file_src, glad_cmakelists_file_dst).unwrap();*/
    spinner.finish();
    Ok(())
}

/*
pub fn generate_cmake_from_conf(config : &Config) -> Result<()>{
    let config_file = ConfigFile::new_from_file(&config)?;
    let mut cmake_include_paths : Vec<String>  = Vec::new();
    if let Some(modules) = &config_file.modules {
        modules.iter().all(|(name, module)| {
            match module{
                Module::GitUrl(_) => {
                    cmake_include_paths.push(name.clone());
                }
                Module::Spec(spec) => 
                    if let Some(include_path) = &spec.include_path{
                        cmake_include_paths.push(format!("{name}/{include_path}"));
                    }else{
                        cmake_include_paths.push(name.clone());
                    }
            }
            return true;
        });
    }
    let cmake_modules = config_file.get_all_cmake_modules(&config.project_paths.root)?
        .iter().map(|m| m.module_name.clone()).collect::<Vec<_>>();
    let cmake_link_modules = config_file.get_all_cmake_modules(&config.project_paths.root)?
        .iter().map(|m| m.project_name.clone()).collect::<Vec<_>>();
/*    let target_link_libraries : String = cmake_modules
        .iter()
        .map(|module| module.project_name.as_str() )
        .collect::<Vec<_>>()
        .join("\n");*/
    let cmake_template = CMakeTemplate::new(&config);
    cmake_template.generate_to_file(config, config.project_paths.root.clone(),
        &GeneratePattern{   
            project_name : &config_file.project,
            add_command : config_file.builds.get(&config_file.project).ok_or("library not in builds")?.as_str(),
            include_paths: Some(&cmake_include_paths),
            link_modules : Some(&cmake_link_modules),
            subdirectories : Some(&cmake_modules),
            sources_path : None,
            recursive_glob : None
        })?;
    Ok(())
}
*/
pub async fn init_project(config : Config, multi : Arc<MultiProgress>) -> Result<()>{
    todo!();/* 
    let name : String = if let Some(name) = config.project_paths.root.iter().last(){
        name.to_string_lossy().into_owned()
    } else {
        Err("Failed to get project name")?
    };
    let project_spinner = Spinner::new(format!("Initializing project {}...", &name), Some(multi.clone()));
    let tasks_spinner = Spinner::new("Creating project dir...", Some(multi.clone()));
    if !config.project_paths.root.exists(){
        Err("Error directory doens't exists")?;
    }
    tasks_spinner.change_message("creating src dir...");
    fs::create_dir(&config.project_paths.src)?;
    
    tasks_spinner.change_message("initializing git...");
    let output = Command::new("git")
        .arg("init")
        .current_dir(&config.project_paths.root)
        .output()?;
    if !output.status.success(){
        Err("Failed to initialize git")?;
    }
    tasks_spinner.change_message("adding gitignore...");
    let gitignore_src = config.asharis_root.join("resources").join(".gitignore");
    let gitignore_dst = config.project_paths.root.join(".gitignore");
    fs::copy(gitignore_src, gitignore_dst)?;

    init_source(config.clone(), NewProject{name : name.clone()}, multi.clone()).await?;

    tasks_spinner.finish();
    //MAIN CMAKELISTS
    cmake::generate_to_file_from_path(&config, &config.project_paths.root).map_err(|e| format!("Failed to generate cmake: {e}"))?;

    build_cmake_project(&config, multi.clone())?;
    project_spinner.finish();
    Ok(())
    */
}

pub async fn new_project(config : Config, project : NewProject, multi : Arc<MultiProgress>) -> Result<()>{
    let project_spinner = Spinner::new(format!("Creating project {}...", &project.name), Some(multi.clone()));
    let tasks_spinner = Spinner::new("Creating project dir...", Some(multi.clone()));
    //config.change_root(project_paths.root.join(PathBuf::from(&project.name)));
    let project_root = config.cmd_path.join(&project.name);
    if project_root.exists(){
        Err("Error already exists")?;
    }
    fs::create_dir(&project_root)?;

    tasks_spinner.change_message("creating config.yaml...");

    let config_src = config.asharis_root.join("resources").join("new_project").join("TemplateConfig.yaml");
    let config_dst = project_root.join("config.yaml");
    let contents = fs::read_to_string(&config_src).map_err(|e| format!("failed to read TemplateConfig.yaml: {e}, path: {}", config_src.to_str().unwrap()))?;
    let modified_contents = contents.replace(config.flags.project_name, &project.name);
    fs::write(config_dst, modified_contents)?;

    let config = ProjectConfig::from(config, project_root)?;
    tasks_spinner.change_message("creating src dir...");
    fs::create_dir(&config.project_paths.src)?;
    
    tasks_spinner.change_message("initializing git...");
    let output = Command::new("git")
        .arg("init")
        .current_dir(&config.project_paths.root)
        .output()?;
    if !output.status.success(){
        Err("Failed to initialize git")?;
    }
    tasks_spinner.change_message("adding gitignore...");
    let gitignore_src = config.asharis_root.join("resources").join(".gitignore");
    let gitignore_dst = config.project_paths.root.join(".gitignore");
    fs::copy(gitignore_src, gitignore_dst)?;

    init_source(config.clone(), project.clone(), multi.clone()).await?;

    tasks_spinner.finish();
    //MAIN CMAKELISTS
    cmake::generate_to_file_from_path(&config, &config.project_paths.root).map_err(|e| format!("Failed to generate cmake: {e}"))?;

    build_cmake_project(&config, multi.clone())?;
    project_spinner.finish();
    println!("{}. Project {} created.", "Success".green(), project.name.blue());
    Ok(())
}

pub fn build_cmake_project(config : &ProjectConfig, multi : Arc<MultiProgress>) -> Result<()>{
    let spinner = Spinner::new("Generating cmake build files", Some(multi));
    defer!(spinner.finish(););
    if !fs::exists(&config.project_paths.build)?{
        fs::create_dir(&config.project_paths.build)?;
    }
    
    let output = Command::new("cmake")
        .args(["..", "-G", "Visual Studio 17 2022", "-A", "x64"])
        .current_dir(&config.project_paths.build)
        .output()?;
    if !output.status.success(){
        Err(format!("failed to to build cmake project, {}", String::from_utf8_lossy(&output.stderr)))?;
    }
    Ok(())
}


#[derive(PartialEq)]
pub enum BuildProjectOpts{
    Silent,
    SilentIfOkey,
    WriteOutputAlways
}

pub fn build_project(config : &ProjectConfig, options : &CMakeOptions, multi : Arc<MultiProgress>, output_options : BuildProjectOpts) -> Result<()>{
    let start = Instant::now();
    if !fs::exists(&config.project_paths.build)?{
        fs::create_dir(&config.project_paths.build)?;
    }
    let build_config = options.get_config()?;
    let target = options.get_target(&config.config_file)?;
    println!("{} {} [Config: {}]", "Building".green(), target.blue(), build_config);
    let mut command = Command::new("cmake");
    command
        .args(["--build", ".", "--config", build_config, "--target", target.as_str()])
        .current_dir(&config.project_paths.build);
    match output_options{
        BuildProjectOpts::SilentIfOkey => {
            let output = command.output()?;
            if !output.status.success(){
                Err(format!("{} to build {}:\n{}", "Failed".red(), target.blue(), String::from_utf8_lossy(&output.stdout)))?;
            }
        }
        BuildProjectOpts::WriteOutputAlways => {
            let status = command.status()?;
            if !status.success(){
                Err(format!("Failed to {}: \n{:?}", target.blue(), status.code()))?;
            }
        }
        BuildProjectOpts::Silent => {
            let _ = command.status()?;
        }
    }
    let elapsed = start.elapsed();
    if output_options != BuildProjectOpts::Silent{
        println!("{} {} built [target: {}] in {:#?}", "Success".green(), target.blue(), build_config, elapsed);
    }
    Ok(())

}
pub fn run_project(config : &ProjectConfig, options : &CMakeOptions, multi : Arc<MultiProgress>) -> Result<()>{
    let target = options.get_target(&config.config_file)?;
    if let Some(binary_type) = config.config_file.targets.get(target){
        if *binary_type != BinaryType::Executable{
            Err(format!("{} is a {}: only Executable targets can be run", target.blue(), binary_type.to_string()))?
        }
    }
    build_project(config, options, multi.clone(),  BuildProjectOpts::SilentIfOkey)?;
    let build_config = options.get_config()?;
    let executable_path = PathBuf::from("output").join("bin").join(build_config).join(&target);
    let mut command = String::from("./");
    command.push_str(&executable_path.to_string_lossy());
    println!("{} {}", "Running".green(), target.blue());
    let _ = Command::new(command.as_str())
        .current_dir(&config.project_paths.root)
        .status().map_err(|e| format!("failed to run 'run' command: {e}"))?;
    Ok(())
}
pub fn clean_project(config : &ProjectConfig, multi : Arc<MultiProgress>) -> Result<()>{
    let spinner = Spinner::new(format!("{} {}", "Cleaning".green(), config.config_file.project.blue()), Some(multi.clone()));
    defer!(spinner.finish(););
    if fs::exists(&config.project_paths.build)?{
        fs::remove_dir_all(&config.project_paths.build)?;
    }
    if fs::exists(&config.project_paths.output)?{
        fs::remove_dir_all(&config.project_paths.output)?;
    }
    spinner.finish();
    Ok(())
}

pub fn add_target(config : &ProjectConfig, target : &AddTarget) -> Result<()>{
    let mut config_file = ConfigFile::new_from_file(config)?;
    if config_file.targets.contains_key(&target.name){
        Err(format!("Target {} already defined config.yaml", target.name.blue()))?
    }
    let path_to_target = config.project_paths.src.join(&target.name);
    if fs::exists(&path_to_target)?{
        Err(format!("There is already a directory {} in src", target.name))?;
    }
    fs::create_dir(&path_to_target)?;
    fs::create_dir_all(path_to_target.join("src").join(&target.name))?;
    let pattern = cmake::GeneratePattern{
        project_name : &target.name,
        add_command : target.binary_type.clone(),
        include_paths : None,
        link_modules : None,
        subdirectories : None,
        sources_path : None,
        recursive_glob : None,
        cmp_def : None
    };
    cmake::generate_to_file(config, path_to_target, &pattern)?;
    config_file.targets.insert(target.name.clone(), target.binary_type.clone());
    config_file.write()?;
    Ok(())
}
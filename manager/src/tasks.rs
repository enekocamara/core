use indicatif::{MultiProgress,ProgressBar, ProgressStyle};
use tokio::time::{ sleep, Duration};
use std::fmt::format;
use std::{fs, path};
use std::path::{PathBuf, Path};
use std::process::Command;
use std::sync::Arc;
use std::result::Result;
use std::collections::HashMap;
use serde::{Deserialize,Serialize};
use git2::{Repository, Submodule};

use crate::spinner::Spinner;
use crate::config::{Config, ConfigFile, Vendor, VendorSpec, VendorSourceGit, VendorSourceCurl, VendorSource, CmakeModule};
use crate::args::{NewProject, NewSyrisProject, AddLibrary, RemoveLibrary};
use crate::{new_project, utils};

fn remove_vendor_git_submodule<'re>(name : &str, project_root : &Path) -> Result<(), String>{
    let output = Command::new("git")
        .args(["submodule", "deinit", "-f", format!("vendor/{name}").as_str()])
        .current_dir(project_root)
        .output()
        .map_err(|e| format!("Failed to run command {e}"))?;
    if !output.status.success(){
        Err(format!("Failed to deinit submodule vendor/{name}: {}", String::from_utf8_lossy(&output.stderr)))?;
    }

    fs::remove_dir_all(project_root.join("vendor").join(name)).map_err(|e| format!("Failed to remove dir vendor/{name}: {e}"))?;

    let output = Command::new("git")
        .args(["rm","--cached", format!("vendor/{name}").as_str()])
        .current_dir(project_root)
        .output()
        .map_err(|e| format!("Failed to run command {e}"))?;
    if !output.status.success(){
        Err(format!("Failed to remove vendor{name} cache: {}", String::from_utf8_lossy(&output.stderr)))?;
    }

    let output = Command::new("git")
        .args(["config", "-f", ".gitmodules", "--remove-section", format!("submodule.vendor/{name}").as_str()])
        .current_dir(project_root)
        .output()
        .map_err(|e| format!("Failed to run command {e}"))?;
    if !output.status.success(){
        Err(format!("Failed to deinit submodule vendor/{name}: {}", String::from_utf8_lossy(&output.stderr)))?;
    }
    let path_to_remove = project_root.join(".git").join("modules").join("vendor").join(name); 
    fs::remove_dir_all(&path_to_remove).map_err(|e| format!("Failed to remove dir {:?}: {e}", path_to_remove))?;
    Ok(())
}
fn add_vendor_git_submodule<'re>(git_repo : &'re Repository, name : &str, url : &str, branch : Option<&str>, project_root : &Path) -> Result<Submodule<'re>, String>{
    let output = {
        if let Some(branch) = branch{
            Command::new("git")
                .args(["submodule", "add", "-b", branch,  url, format!("vendor/{name}").as_str()])
                .current_dir(project_root)
                .output()
                .map_err(|e| format!("Failed to run command: {e}"))?
        }else{
            println!("Repot path {:?}",git_repo.path());
            Command::new("git")
                .args(["submodule", "add", url, format!("vendor/{name}").as_str()])
                .current_dir(project_root)
                .output()
                .map_err(|e| format!("Failed to run command: {e}"))?
        }
    };
    if !output.status.success(){
        Err(format!("Failed to add git submodule {name}: {}", String::from_utf8_lossy(&output.stderr)))?
    }
    let output = {
        Command::new("git")
            .args(["submodule", "update", "--init", "--recursive"])
            .current_dir(project_root)
            .output()
            .map_err(|e| format!("Failed to run command: {e}"))?
    };
    if !output.status.success(){
        Err(format!("Failed to init git submodule {name}: {}", String::from_utf8_lossy(&output.stderr)))?
    }
    git_repo.find_submodule(format!("vendor/{name}").as_str()).map_err(|e| format!("Failed to find submodule {name}: {e}"))
}

pub async fn update_vendor(config : Config, config_file : Option<ConfigFile>, multi : Arc<MultiProgress>) -> Result<(), String>{
    let config_file = match config_file {
        Some(config_file) => config_file,
        None => {
            let config_file = fs::read_to_string(config.project_root.join("config.yaml"))
                .map_err(|e| format!("Failed to open config.yaml: {e}"))?;
            serde_yaml::from_str::<ConfigFile>(&config_file)
                .map_err(|e| format!("failed to parse config.yaml: {e}"))?
        }
    };
    if !fs::exists(config.project_root.join("vendor")).unwrap(){
        fs::create_dir(config.project_root.join("vendor")).map_err(|e| format!("Failed to create vendor directory"))?;
    }
    let vendor_dir_list = fs::read_dir(config.project_root.join("vendor")).map_err(|e| format!("Failed to read directory vendor: {e}"))?;
    let mut current_vendor_libraries = Vec::new();
    let git_repo = Repository::open(&config.project_root).map_err(|e| format!("Failed to open git repository: {e}"))?;
    let submodules = git_repo.submodules().map_err(|e| format!("Failed to get git submodules: {e}"))?;
    for entry in vendor_dir_list{
        let entry = entry.map_err(|e| format!("Failed to read entry: {e}"))?;
        if entry.path().is_dir(){
            if let Some(name) = entry.file_name().to_str(){
                current_vendor_libraries.push(name.to_string());
            }
        }
    }

    if let Some(vendor) = &config_file.vendor{
        for current_vendor_lib in &current_vendor_libraries{
            if !vendor.contains_key(current_vendor_lib){
                if let Ok(submodule) = git_repo.find_submodule(format!("vendor/{}", &current_vendor_lib).as_str()){
                    let output = Command::new("git")
                        .args(["submodule", "deinit", "-f", "--", format!("submodule.vendor/{current_vendor_lib}").as_str()])
                        .current_dir(&config.project_root)
                        .output()
                        .map_err(|e| format!("failed to run command"))?;

                    if !output.status.success(){
                        return Err(format!("Failed to deinit submodule {current_vendor_lib}: {}", String::from_utf8_lossy(&output.stderr)));
                    }
                    
                    let output = Command::new("git")
                        .args(["config", "f", ".git/config", "--remove-section", format!("submodule.vendor/{current_vendor_lib}").as_str()])
                        .current_dir(&config.project_root)
                        .output()
                        .map_err(|e| format!("failed to run command"))?;
                    
                    if !output.status.success(){
                        return Err(format!("Failed to remove submodule {current_vendor_lib} from git config: {}", String::from_utf8_lossy(&output.stderr)));
                    }
                }
                fs::remove_dir_all(config.project_root.join("vendor").join(current_vendor_lib)).map_err(|e| format!("Failed to remove {current_vendor_lib} vendor: {e}"))?;
            }
        }
        for (name, specs) in vendor{
            if !current_vendor_libraries.contains(name){
                match &specs{
                    Vendor::GitUrl(git_url) => {
                        add_vendor_git_submodule(&git_repo, name, &git_url, None, &config.project_root)?;
                    }
                    Vendor::Spec(spec) => match &spec.source{
                        VendorSource::Curl(curl) => match curl {
                            VendorSourceCurl::CurlUrl{curl_url} =>{todo!("implement curl")}
                            VendorSourceCurl::CurlUrls{curl_urls} =>{todo!("implement curl")}
                        }
                        VendorSource::Git(git) =>{
                            add_vendor_git_submodule(&git_repo, name, &git.git_url, git.git_branch.as_deref(), &config.project_root)?;
                        }
                    }
                }
            }
        }
    }
    Ok(())
}

pub async fn remove_library(config : Config, library : RemoveLibrary, multi : Arc<MultiProgress>) -> Result<(),String>{
    let config_source = fs::read_to_string(config.project_root.join("config.yaml")).map_err(|e| format!("Failed to read config.yaml"))?;
    let config_file : ConfigFile = serde_yaml::from_str(&config_source).map_err(|e| format!("Failed to parse config.yaml: {e}"))?;
    if let Some(vendor) = &config_file.vendor{
        if let Some(specs) = vendor.get(&library.name){
            match &specs{
                Vendor::GitUrl(_) => {
                    remove_vendor_git_submodule(&library.name, &config.project_root)?;
                },
                Vendor::Spec(specs) => {
                    match &specs.source{
                        VendorSource::Curl(_) => {
                            fs::remove_dir_all(config.project_root.join("vendor").join(&library.name)).map_err(|e| format!("Failed to remove {} directory: {e}", library.name))?;
                        }
                        VendorSource::Git(_) => {
                            remove_vendor_git_submodule(&library.name, &config.project_root)?;
                        }
                    }
                }
            }
        }
    }else {
        return Err(format!("Failed to remove {}, not found", library.name));
    }
    Ok(())
}
pub async fn add_library_to_config(config : Config, library : AddLibrary, multi : Arc<MultiProgress>) -> Result<(),String>{
    let vendor_list = fs::read_to_string(config.asharis_root.join("resources").join("vendorlist.yaml"))
        .map_err(|e| format!("failed to open vendorlist.yaml file"))?;

    #[derive(Debug, Deserialize, Serialize)]
    struct VendorList{
        vendor : HashMap<String, Vendor>
    }
    
    let libraries : VendorList = serde_yaml::from_str(&vendor_list)
        .map_err(|e| format!("Failed to parse vendor list: {e}"))?;
    if let Some((lib_name, specs)) = libraries.vendor.get_key_value(&library.name){
        let config_file = fs::read_to_string(config.project_root.join("config.yaml"))
            .map_err(|e| format!("Failed to open config.yaml: {e}"))?;
        let mut parsed_config_file : ConfigFile = serde_yaml::from_str(&config_file)
            .map_err(|e| format!("failed to parse config.yaml: {e}"))?;
        let vendor_map = parsed_config_file.vendor.get_or_insert(HashMap::new());
        vendor_map.insert(lib_name.to_string(), specs.clone());
        let modified_config_file = serde_yaml::to_string(&parsed_config_file).map_err(|e| format!("failed to parse new config file: {e}"))?;
        fs::write(config.project_root.join("config.yaml"), modified_config_file)
            .map_err(|e| format!("failed to write new config.yaml: {e}"))?;
        update_vendor(config.clone(), Some(parsed_config_file), multi).await?;
        generate_cmake_from_conf(config).await.map_err(|e|format!("Failed to generate updated cmakefile: {e}"))?;
    }else {
        return Err(format!("Failed to find library in vendor list. try adding it yoursel in the config.yaml file under [vendor]"))
    }
    Ok(())
}

pub async fn init_syris(config : Config, project : NewSyrisProject, multi : Arc<MultiProgress>) -> Result<(),String>{
    let spinner = Spinner::new("initializing syris",Some(multi));
    let syris_url = "https://github.com/enekocamara/Syris";
    let syris_folder = config.project_root.join("vendor").join("syris");
    //println!("syris folder: '{:?}'", syris_folder);
    let output = Command::new("git")
        .args(["submodule", "add", syris_url, "vendor/syris"])
        .current_dir(&config.project_root)
        .output()
        .map_err(|e| format!("failed to run command {}", e))?;
    if !output.status.success(){
        return Err(format!("failed to add submodule {}", String::from_utf8_lossy(&output.stderr)));
    }

//    println!("Submodule updates...");
    spinner.change_message("initializing submodules recursive...");
    let output = Command::new("git")
        .args(["submodule", "update", "--init", "--recursive"])
        .current_dir(&syris_folder)
        .output()
        .map_err(|e| format!("failed to run command {}", e))?;

    if !output.status.success(){
        return Err(format!("failed to add submodule {}", String::from_utf8_lossy(&output.stderr)));
    }

    //GIT
    spinner.change_message("adding gitignore...");
    let gitignore_src = config.asharis_root.join("resources").join(".gitignore");
    let gitignore_dst = config.project_root.join(".gitignore");
    fs::copy(gitignore_src, gitignore_dst).map_err(|e| format!("failed to copy .gitignore: {}", e))?;

    //IMGUI CMAKE
    spinner.change_message("adding imgui cmakelists...");
    let imgui_src = config.asharis_root.join("resources").join("ImGuiCMakeLists.txt");
    let imgui_dst = config.project_root.join("vendor").join("syris").join("vendor").join("imgui_docking").join("CMakeLists.txt");
    
    fs::copy(imgui_src, imgui_dst).map_err(|e| format!("failed to copy imgui cmakelists: {}", e))?;

    //GLAD CMAKE
    spinner.change_message("adding glad cmakelists...");
    let glad_src = config.asharis_root.join("resources").join("GladCMakeLists.txt");
    let glad_dst = config.project_root.join("vendor").join("syris").join("vendor").join("glad").join("CMakeLists.txt");
    
    fs::copy(glad_src, glad_dst).map_err(|e| format!("failed to copy glad cmakelists: {}", e))?;

    spinner.finish();
    Ok(())
}

pub async fn init_entry_point(config : Config, project : NewSyrisProject, multi : Arc<MultiProgress>)->Result<(), String>{
    let spinner = Spinner::new("initializing entry point", Some(multi));
    spinner.change_message("setting entrypoint...");
    let entry_point_src = config.asharis_root.join("resources").join("EntryPoint").join("src");
    utils::copy_dir_rec(&entry_point_src, &config.project_root.join("EntryPoint")).map_err(|e| format!("failed to copy dir entrypoint: {e}"))?;
    let entry_point_cmakelists_file_src = config.asharis_root.join("resources").join("EntryPoint").join("CMakeLists.txt");
    let entry_point_cmakelists_file_dst = config.project_root.join("EntryPoint").join("CMakeLists.txt");

    let contents = fs::read_to_string(entry_point_cmakelists_file_src).map_err(|e| format!("Failed to read entrypoint cmakelists: {e}"))?;

    let modified_content = contents.replace(config.project_name_flag, &project.name);

    fs::write(entry_point_cmakelists_file_dst, modified_content).map_err(|e| format!("failed to write modified entrypoint cmakelists file: {e}"))?;
    Ok(())
}

pub async fn init_vs_conf(config : Config, project : NewSyrisProject, multi : Arc<MultiProgress>) -> Result<(), String>{
    let spinner = Spinner::new("setting vs conf...", Some(multi.clone()));
    let vs_conf_src = config.asharis_root.join(".vscode");
    utils::copy_dir_rec(&vs_conf_src, &config.project_root).map_err(|e| format!("Failed to copy /.vscode config: {e}"))?;
    spinner.finish();
    Ok(())
}

pub async fn init_source(config : Config, project : NewProject, multi : Arc<MultiProgress>) -> Result<(), String>{
    let spinner  = Spinner::new("setting src contents...", Some(multi.clone()));
    fs::create_dir(config.project_root.join("src").join(&project.name)).map_err(|e| format!("Failed to create src dir: {e}"))?;
    let template_main_src = config.asharis_root.join("resources").join("new_project").join("TemplateMain.cpp");
    let template_main_dst = config.project_root.join("src").join(&project.name).join("main.cpp");
    fs::copy(template_main_src,template_main_dst ).map_err(|e| format!("Failed to copy template main.cpp to src/{}/main.cpp: {e}", &project.name))?;
    let project_config_file_src = config.asharis_root.join("resources").join("new_project").join("TemplateConfig.yaml");
    let project_config_file_dst = config.project_root.join("config.yaml");
    let contents = fs::read_to_string(project_config_file_src).map_err(|e| format!("Failed to read config.yaml: {e}"))?;

    let modified_content = contents.replace(config.project_name_flag, &project.name);
    fs::write(project_config_file_dst, modified_content).map_err(|e| format!("Failed to write modified src cmakelists: {e}"))?;
    spinner.finish();
    Ok(())
}
pub async fn init_syris_source(config : Config, project : NewSyrisProject, multi : Arc<MultiProgress>) -> Result<(), String>{
    let spinner  = Spinner::new("setting src contents...", Some(multi.clone()));
    fs::create_dir(config.project_root.join("src").join(&project.name)).map_err(|e| format!("Failed to create src dir: {e}"))?;
    let project_cmakelists_file_src = config.asharis_root.join("resources").join("Template").join("TemplateCMakeLists.txt");
    let project_cmakelists_file_dst = config.project_root.join("src").join("CMakeLists.txt");
    let contents = fs::read_to_string(project_cmakelists_file_src).map_err(|e| format!("Failed to read src cmakelists.txt: {e}"))?;

    let modified_content = contents.replace(config.project_name_flag, &project.name);
    fs::write(project_cmakelists_file_dst, modified_content).map_err(|e| format!("Failed to write modified src cmakelists: {e}"))?;

    let project_name_first_upper_flag = "%PROJECT_NAME_FIRST_UPPER%";
    let project_name_first_upper = utils::first_uppercase(&project.name);

    let template_hpp_file_src = config.asharis_root.join("resources").join("Template").join("Template.hpp");
    let template_hpp_file_dst = config.project_root.join("src").join(&project.name).join(format!("{}App.hpp", &project_name_first_upper));
    let contents = fs::read_to_string(template_hpp_file_src).map_err(|e| format!("Failed to read template hpp file: {e}"))?;

    let modified_content = contents.replace(project_name_first_upper_flag, &project_name_first_upper);
    let modified_content = modified_content.replace(config.project_name_flag, &project.name);
//    println!("Path : {}App.hpp", &project_name_first_upper);
    fs::write(template_hpp_file_dst, modified_content).map_err(|e| format!("Failed to write modified template hpp file: {e}"))?;

    let template_cpp_file_src = config.asharis_root.join("resources").join("Template").join("Template.cpp");
    let template_cpp_file_dst = config.project_root.join("src").join(&project.name).join(format!("{}App.cpp", &project_name_first_upper));
    let contents = fs::read_to_string(template_cpp_file_src).map_err(|e| format!("Failed to read template cpp file: {e}"))?;

    let modified_content = contents.replace(project_name_first_upper_flag, &project_name_first_upper);
    let modified_content = modified_content.replace(config.project_name_flag, &project.name);
    fs::write(template_cpp_file_dst, modified_content).map_err(|e| format!("failed to write modified template cpp file: {e}"))?;
    Ok(())
}

pub async fn pip_glad_install(config : &Config, multi : Arc<MultiProgress>) -> Result<(), String>{
    // Step 1: Create and activate the virtual environment
    let spinner = Spinner::new("generating glad files", Some(multi));
    //println!("Creating virtual environment...");
    let folders_to_glad = ["vendor", "syris", "vendor", "glad"];
    let path_to_glad = folders_to_glad.iter().fold(config.project_root.clone(), |path, folder| path.join(folder));
    spinner.change_message("creating python venv");
    let outupt = Command::new("python")
        .args(["-m", "venv", "venv"])
        .current_dir(&path_to_glad)
        .output()
        .map_err(|e| format!("Failed to run command: {e}"))?;
    if !outupt.status.success(){
        return Err("failed to create python env".to_string());
    }

    // Step 2: Install glad2 in the virtual environment
    spinner.change_message("installing glad2..");
    let output = Command::new("pip")
        .args(["install", "glad2"])
        .current_dir(path_to_glad.join("venv").join("Scripts"))
        .output()
        .map_err(|e| format!("Failed to run command: {e}"))?;
    if !output.status.success(){
        return Err("failed to create python env".to_string());
    }

    // Step 3: Run glad2 to generate files
    spinner.change_message("generating glad2 files with glad2 generator...");
    let output = Command::new("python")
        .args(["-m", "glad", "--api", "gl:core=4.6", "--out-path", path_to_glad.join("src").to_str().unwrap(), "c"])
        .current_dir(path_to_glad.join("venv").join("Scripts"))
        .output()
        .map_err(|e| format!("Failed to run command: {e}"))?;
    if !output.status.success(){
        return Err(format!("failed to generate glad2, [{}]", String::from_utf8_lossy(&output.stderr)));
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

pub async fn generate_cmake_from_conf(config : Config) -> Result<(), String>{
    let cmake_template_contents =  fs::read_to_string(config.asharis_root.join("resources").join("TemplateCmakeLists.txt")).map_err(|e| format!("failed to open TemplateCmakeLists.txt"))?;
    let config_contents = fs::read_to_string(config.project_root.join("config.yaml")).map_err(|e| format!("failed to open conf.yaml"))?;
    let config_file : ConfigFile = serde_yaml::from_str(&config_contents).map_err(|e| {format!("failed to parse conf.yaml file: {e}")})?;
    let cmake_add_command: String = {
        match config_file.targets[&config_file.project].as_str(){
            "static_library" => {
                format!("add_library({} STATIC ${{SOURCES}})", config_file.project, )
            }
            "dynamic_library" => {
                format!("add_library({} DYNAMIC ${{SOURCES}})", config_file.project, )
            }
            "executable" => {
                format!("add_executable({} ${{SOURCES}})", config_file.project, )
            }
            other => {
                return Err(format!("target type unavailable {}, use static_library, dynamic_library or executable.", other));
            },
        }
    };
    let mut cmake_include_paths : String =  "".to_string();
    let mut cmake_link_libraries : String = "".to_string();
    if let Some(vendor) = &config_file.vendor {
        vendor.iter().all(|(module, vendor)| {
            match vendor{
                Vendor::GitUrl(url) => {
                    cmake_include_paths.push_str(&format!("${{CMAKE_SOURCE_DIR}}/vendor/{module}\n"));
                }
                Vendor::Spec(spec) => 
                    if let Some(include_path) = &spec.include_path{
                        cmake_include_paths.push_str(&format!("${{CMAKE_SOURCE_DIR}}/vendor/{module}/{include_path}\n"));
                    }else{
                        cmake_include_paths.push_str(&format!("${{CMAKE_SOURCE_DIR}}/vendor/{module}\n"));
                    }
            }
            return true;
        });
    }
    let cmake_modules = config_file.get_all_cmake_modules(&config.project_root)?;
    let cmake_subdirectories = format!(
        "\n{}",
        cmake_modules
            .iter()
            .map(|m| format!("add_subdirectory(vendor/{})",m.module_name))
            .collect::<Vec<_>>()
            .join("\n")
    );
    let target_link_libraries : String = cmake_modules
        .iter()
        .map(|module| module.project_name.as_str() )
        .collect::<Vec<_>>()
        .join("\n");
    let cmake_modified_contents = cmake_template_contents.replace(&config.project_name_flag, &config_file.project)
        .replace(config.cmake_add_command_flag, &cmake_add_command)
        .replace(config.cmake_vendor_include_paths_flag, &cmake_include_paths)
        .replace(config.cmake_vendor_link_libraries_flag, &target_link_libraries)
        .replace(config.cmake_add_subdirectories_flag, &cmake_subdirectories);
    fs::write(config.project_root.join("CmakeLists.txt"), cmake_modified_contents).map_err(|e| format!("failed to write generatedcmakelist file"))?;
    return Ok(());
}
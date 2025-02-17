use std::process::Command;
use std::path::{Path,PathBuf};
use std::fs;
use std::sync::Arc;
use std::collections::HashMap;

use git2::{Repository, Submodule};
use indicatif::MultiProgress;
use serde::{Serialize,Deserialize};
use scopeguard::defer;

use crate::Spinner;
use crate::Result;
use crate::config::{Config, ConfigFile, Module, ModuleSource, ModuleSourceCurl, ModuleSourceError};
use crate::utils;
use crate::args::{AddModule,RemoveModule};
use crate::generate_cmake_from_conf;
use crate::cmake::{CMakeTemplate,GeneratePattern};

pub fn remove_git_submodule<'re>(name : &str, git_path : Option<&str>, project_root : &Path) -> Result<()>{
    let path = match git_path{
        Some(git_path) => format!("modules/{name}/{git_path}"),
        None => format!("modules/{name}")
    };
    let output = Command::new("git")
        .args(["submodule", "deinit", "-f", path.as_str()])
        .current_dir(project_root)
        .output()
        .map_err(|e| format!("Failed to run command {e}"))?;
    if !output.status.success(){
        Err(format!("Failed to deinit submodule {path}: {}", String::from_utf8_lossy(&output.stderr)))?;
    }

    fs::remove_dir_all(project_root.join("modules").join(name)).map_err(|e| format!("Failed to remove dir {path}: {e}"))?;

    let output = Command::new("git")
        .args(["add",".gitmodules"])
        .current_dir(project_root)
        .output()
        .map_err(|e| format!("Failed to run command {e}"))?;
    if !output.status.success(){
        Err(format!("Failed to add .gitmodules: {}", String::from_utf8_lossy(&output.stderr)))?;
    }

    let output = Command::new("git")
        .args(["rm","--cached", path.as_str()])
        .current_dir(project_root)
        .output()
        .map_err(|e| format!("Failed to run command {e}"))?;
    if !output.status.success(){
        Err(format!("Failed to remove {path} cache: {}", String::from_utf8_lossy(&output.stderr)))?;
    }

    let output = Command::new("git")
        .args(["config", "-f", ".gitmodules", "--remove-section", format!("submodule.{path}").as_str()])
        .current_dir(project_root)
        .output()
        .map_err(|e| format!("Failed to run command {e}"))?;
    if !output.status.success(){
        Err(format!("Failed to deinit submodule {path}: {}", String::from_utf8_lossy(&output.stderr)))?;
    }
    let path_to_remove = match git_path{
        Some(git_path) => project_root.join(".git").join("modules").join("modules").join(name).join(git_path),
        None => project_root.join(".git").join("modules").join("modules").join(name),
    };
    fs::remove_dir_all(&path_to_remove).map_err(|e| format!("Failed to remove dir {:?}: {e}", path_to_remove))?;
    Ok(())
}
pub fn add_git_submodule<'re>(git_repo : &'re Repository, name : &str, git_path : Option<&str>, url : &str, branch : Option<&str>, project_root : &Path, multi : Arc<MultiProgress>) -> Result<Submodule<'re>>{
    let spinner = Spinner::new(format!("Adding module {name}: git add submodule..."), Some(multi.clone()));
    defer!(spinner.finish(););
    let path = match git_path{
        Some(path) => format!("modules/{name}/{path}"),
        None => format!("modules/{name}")
    };
    let output = {
        if let Some(branch) = branch{
            Command::new("git")
                .args(["submodule", "add", "-b", branch,  url, path.as_str()])
                .current_dir(project_root)
                .output()
                .map_err(|e| format!("Failed to run command: {e}"))?
        }else{
            Command::new("git")
                .args(["submodule", "add", url, path.as_str()])
                .current_dir(project_root)
                .output()
                .map_err(|e| format!("Failed to run command: {e}"))?
        }
    };
    if !output.status.success(){
        Err(format!("Failed to add git submodule {name}: {}", String::from_utf8_lossy(&output.stderr)))?
    }
    spinner.change_message(format!("Adding module {name}: git init update submodule..."));
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
    git_repo.find_submodule(path.as_str()).map_err(|e| format!("Failed to find submodule {name}: {e}").into())
}

pub fn add_module_to_modules_dir(name : &str, module : &Module, git_repo : &Repository, config : &Config, config_file : &ConfigFile, multi : Arc<MultiProgress>)->Result<()>{
    match module{
        Module::GitUrl(git_url) => {
            add_git_submodule(&git_repo, name,None, &git_url, None, &config.project_root, multi.clone())?;
        }
        Module::Spec(spec) => {
            match &spec.source{
                ModuleSource::Curl(curl) => match curl {
                    ModuleSourceCurl::CurlUrl{curl_url} =>{
                        utils::curl_url(&curl_url, &config.project_root.join("modules").join(name))
                            .map_err(|e| format!("Failed to add module {name} : {e}"))?;
                    }
                    ModuleSourceCurl::CurlUrls{curl_urls} =>{
                        let path = config.project_root.join("modules").join(name);
                        for url in curl_urls{
                            utils::curl_url(url, &path).map_err(|e| format!("Failed to add module {name} : {e}"))?;
                        }
                    }
                }
                ModuleSource::Git(git) =>{
                    add_git_submodule(&git_repo, name, git.git_path.as_deref(), &git.git_url, git.git_branch.as_deref(), &config.project_root, multi.clone())?;
                }
            }
            if let Some(cmd) = &spec.cmd{
                let output = Command::new(cmd.exec.as_str())
                    .args(&cmd.args)
                    .current_dir(config.project_root.join("modules").join(name))
                    .output()?;
                if !output.status.success(){
                    Err(format!("Failed to run module setupt command: {} {}", &cmd.exec, cmd.args.join(" ")))?;
                }
            }
            if let Some(cmake) = &spec.cmake{
                if let Some(true) = cmake.gen{
                    let cmake_template = CMakeTemplate::new(&config);
                    let path = config.project_root.join("modules").join(name);
                    //include  path is for main cmakelists not for generated one
                    /*let path_vec = match &spec.include_path{
                        Some(path) => {
                            Some(vec![path.clone()])
                        }
                        None => None
                    }; */
                    cmake_template.generate_to_file(config, path,&GeneratePattern{
                        project_name : name,
                        add_command : "static_library",
                        include_paths: None,//path_vec.as_ref(),
                        link_modules : None,
                        subdirectories : None,
                        sources_path : cmake.sources_path.as_deref(),
                        recursive_glob : cmake.recursive_glob
                    }).map_err(|e| format!("Failed to generate cmake_file: {e}"))?;
                }
            }
        }
    }
    Ok(())
}

pub fn add_module_to_config(config : &Config, module : &AddModule, multi : Arc<MultiProgress>) -> Result<Module>{
    let add_module_spinner = Spinner::new(format!("Adding module {}", module.name), Some(multi.clone()));
    defer!{add_module_spinner.finish();}
    let tasks_spinner = Spinner::new(format!("Reading moduleslists file {}", module.name), Some(multi.clone()));
    defer!{tasks_spinner.finish();};

    let modules_list = fs::read_to_string(config.asharis_root.join("resources").join("moduleslist.yaml"))
        .map_err(|e| format!("Failed to open moduleslist.yaml file: {e}"))?;

    #[derive(Debug, Deserialize, Serialize)]
    struct ModulesList{
        modules : HashMap<String, Module>
    }
    
    tasks_spinner.change_message("parsing modules lists");
    let modules : ModulesList = serde_yaml::from_str(&modules_list)
        .map_err(|e| format!("Failed to parse modules list: {e}"))?;
    tasks_spinner.change_message(format!("looking for {} in modules lists", &module.name));
    if let Some((name, module)) = modules.modules.get_key_value(&module.name){
        tasks_spinner.change_message(format!("module found: adding module config to core.yaml"));
        let mut config_file = ConfigFile::new_from_file(&config)?;
        let modules_map = config_file.modules.get_or_insert(HashMap::new());
        modules_map.insert(name.to_string(), module.clone());
        config_file.write()?;
        tasks_spinner.finish();
        /*
        add_module_spinner.change_message(format!("Adding module {}: module found", module.name));
        let git_repo = Repository::open(&config.project_root)?;
        add_module_to_modules_dir(lib_name, &git_repo, &config, &config_file, multi)?;
        //update_modules(config.clone(), Some(parsed_config_file), multi).await?;
        */
        Ok(module.clone())
    }else {
        Err(format!("Failed to find module in modules list. Try adding it yoursel in the config.yaml file under [modules]"))?
    }
}

pub fn add_module(config : Config, add_module : &AddModule, multi : Arc<MultiProgress>) -> Result<()>{
    let module = add_module_to_config(&config, &add_module, multi.clone())
        .map_err(|e| format!("Failed to add module to config: {e}"))?;
    let git_repo = Repository::open(&config.project_root)?;
    let config_file = ConfigFile::new_from_file(&config)?;
    add_module_to_modules_dir(add_module.name.as_str(), &module, &git_repo, &config, &config_file, multi)?;
    generate_cmake_from_conf(&config).map_err(|e|format!("Failed to generate updated cmakefile: {e}"))?;
    Ok(())
}
pub fn remove_module(config : Config, module : &RemoveModule, multi : Arc<MultiProgress>) -> Result<()>{
    let mut config_file = ConfigFile::new_from_file(&config)?;
    if let Some(modules) = &mut config_file.modules{
        if let Some(specs) = modules.get(&module.name){
            match &specs{
                Module::GitUrl(_) => {
                    remove_git_submodule(&module.name, None,&config.project_root)?;
                },
                Module::Spec(specs) => {
                    match &specs.source{
                        ModuleSource::Curl(_) => {
                            fs::remove_dir_all(config.project_root.join("modules").join(&module.name)).map_err(|e| format!("Failed to remove {} directory: {e}", module.name))?;
                        }
                        ModuleSource::Git(git) => {
                            remove_git_submodule(&module.name,git.git_path.as_deref(),  &config.project_root)?;
                        }
                    }
                }
            }
            modules.remove(&module.name);
            config_file.write()?;
        }
    }else {
        Err(format!("Failed to remove {}, not found", module.name))?;
    }
    
    Ok(())
}
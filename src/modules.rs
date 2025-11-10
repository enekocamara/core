use std::process::Command;
use std::path::{Path, PathBuf};
use std::fs;
use std::sync::Arc;
use std::collections::HashMap;

use git2::{Repository, Submodule};
use indicatif::MultiProgress;
use colored::*;
use serde::{Serialize,Deserialize};
use scopeguard::defer;

use crate::Spinner;
use crate::Result;
use crate::config::{Config, ConfigFile, Module, ModuleSource, ModuleSourceCurl, ProjectConfig};
use crate::utils;
use crate::args::{AddModule,BinaryType};
use crate::cmake::{self,GeneratePattern};

pub fn remove_git_submodule<'re>(name : &str, version : &str, git_path : Option<&str>, project_root : &Path) -> Result<()>{
    let path = match git_path{
        Some(git_path) => format!("modules/{name}_{version}/{git_path}"),
        None => format!("modules/{name}_{version}")
    };
    let output = Command::new("git")
        .args(["submodule", "deinit", "-f", path.as_str()])
        .current_dir(project_root)
        .output()
        .map_err(|e| format!("Failed to run command {e}"))?;
    if !output.status.success(){
        Err(format!("Failed to deinit submodule {path}: {}", String::from_utf8_lossy(&output.stderr)))?;
    }

    fs::remove_dir_all(project_root.join("modules").join(format!("{name}_{version}"))).map_err(|e| format!("Failed to remove dir {path}: {e}"))?;

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
        Some(git_path) => project_root.join(".git").join("modules").join("modules").join(format!("{name}_{version}")).join(git_path),
        None => project_root.join(".git").join("modules").join("modules").join(format!("{name}_{version}")),
    };
    fs::remove_dir_all(&path_to_remove).map_err(|e| format!("Failed to remove dir {:?}: {e}", path_to_remove))?;
    Ok(())
}
pub fn add_git_submodule<'re>(git_repo : &'re Repository, name : &str, git_path : Option<&str>, url : &str, version : &str, project_root : &Path, multi : Arc<MultiProgress>) -> Result<Submodule<'re>>{
    let spinner = Spinner::new(format!("{} module {}: git add submodule...", "Adding".green(), name.blue()), Some(multi.clone()));
    defer!(spinner.finish(););
    let dir_path = match git_path{
        Some(path) => format!("modules/{name}_{version}/{path}"),
        None => format!("modules/{name}_{version}")
    };
    let output = {
        //if let Some(branch) = branch{
            Command::new("git")
                .args(["submodule", "add",  url, dir_path.as_str()])
                .current_dir(project_root)
                .output()
                .map_err(|e| format!("Failed to run command: {e}"))?
        /* }else{
            Command::new("git")
                .args(["submodule", "add", url, path.as_str()])
                .current_dir(project_root)
                .output()
                .map_err(|e| format!("Failed to run command: {e}"))?
        }*/
    };
    if !output.status.success(){
        Err(format!("Failed to add git submodule {}: {}", name.blue(), String::from_utf8_lossy(&output.stderr)))?
    }

    let output = {
        //if let Some(branch) = branch{
            let dir_path = match git_path{
                Some(path) => PathBuf::from("modules").join(format!("{name}_{version}")).join(path),
                None => PathBuf::from("modules").join(format!("{name}_{version}"))
            };
            Command::new("git")
                .args(["checkout", version])
                .current_dir(project_root.join(dir_path))
                .output()
                .map_err(|e| format!("Failed to run command: {e}"))?
        /* }else{
            Command::new("git")
                .args(["submodule", "add", url, path.as_str()])
                .current_dir(project_root)
                .output()
                .map_err(|e| format!("Failed to run command: {e}"))?
        }*/
    };
    if !output.status.success(){
        Err(format!("Failed to checkout module to version {version},{}", String::from_utf8_lossy(&output.stderr)))?
    }

    spinner.change_message(format!("{} module {}: git init update submodule...", "Adding".green(), name.blue()));
    let output = {
        Command::new("git")
            .args(["submodule", "update", "--init"])
            .current_dir(project_root)
            .output()
            .map_err(|e| format!("Failed to run command: {e}"))?
    };
    if !output.status.success(){
        Err(format!("Failed to init git submodule {}: {}", name.blue(), String::from_utf8_lossy(&output.stderr)))?
    }
    git_repo.find_submodule(&dir_path.as_str()).map_err(|e| format!("Failed to find submodule {}: {e}", name.blue()).into())
}

pub fn add_module_to_modules_dir(name : &str, module : &Module, git_repo : &Repository, config : &ProjectConfig,  multi : Arc<MultiProgress>)->Result<()>{

    let mut ID : String = name.to_string();
            match &module.source{
                ModuleSource::Curl(curl) =>{
                    match curl {
                        ModuleSourceCurl::CurlUrl{curl_url} =>{
                            utils::curl_url(&curl_url, &config.project_paths.root.join("modules").join(&ID))
                                .map_err(|e| format!("Failed to add module {name} : {e}"))?;
                        }
                        ModuleSourceCurl::CurlUrls{curl_urls} =>{
                            let path = config.project_paths.root.join("modules").join(&ID);
                            for url in curl_urls{
                                utils::curl_url(url, &path).map_err(|e| format!("Failed to add module {name} : {e}"))?;
                            }
                        }
                    }
                }
                ModuleSource::Git(git) => {
                    add_git_submodule(&git_repo, name, git.git_path.as_deref(), &git.git_url, &git.git_version, &config.project_paths.root, multi.clone())?;
                    ID = format!("{name}_{}", &git.git_version);
                }
            };
            if let Some(cmd) = &module.cmd{
                let output = Command::new(cmd.exec.as_str())
                    .args(&cmd.args)
                    .current_dir(config.project_paths.modules.join(&ID))
                    .output()?;
                if !output.status.success(){
                    Err(format!("Failed to run module setupt command: {} {}", &cmd.exec, cmd.args.join(" ")))?;
                }
            }
            if let Some(cmake) = &module.cmake{
                if let Some(true) = cmake.gen{
                    if fs::exists(config.project_paths.modules.join(name).join("config.yaml"))?{
                        Err(format!("Module {name} has a config.yaml. Cmake custom generation is not compatible with modules that use config.yaml"))?
                    }
                    let path = config.project_paths.modules.join(&ID);
                    //include  path is for main cmakelists not for generated one
                    let path_vec = match &cmake.include_path{
                        Some(path) => {
                            Some(vec![path.clone()])
                        }
                        None => None
                    }; 
                    cmake::generate_to_file(config, path,&GeneratePattern{
                        project_name : ID.as_str(),
                        add_command : BinaryType::StaticLibrary,
                        include_paths: path_vec.as_ref(),
                        link_modules : None,
                        subdirectories : None,
                        sources_path : cmake.sources_path.as_deref(),
                        recursive_glob : cmake.recursive_glob,
                        cmp_def : None
                    }).map_err(|e| format!("Failed to generate cmake_file: {e}"))?;
                }
    };
    //early return only if its not git. if so we need to add them recursively and handle core module dependencies

    if !module.is_git(){
        return Ok(())
    }
    //handle childs and module dependencies
    let path_to_config_file = config.project_paths.modules.join(&ID).join("config.yaml");
    if !path_to_config_file.exists(){
        //todo!("recursively initialize all submodules");
        /*let output = {
            Command::new("git")
                .args(["submodule", "update", "--init", "--recurse", format!("modules/{name}").as_str()])
                .current_dir(&config.project_paths.root)
                .output()
                .map_err(|e| format!("Failed to run command: {e}"))?
        };
        if !output.status.success(){
            Err(format!("Failed to init git submodule {name}: {}", String::from_utf8_lossy(&output.stderr)))?
        }*/
        return Ok(())
    }
    let module_config_file = ConfigFile::new_from_path(&config.project_paths, &path_to_config_file)?;
    cmake::generate_to_file_from_path(config, &config.project_paths.modules.join(&ID))?;
    //need to figure out which ones are git submodules and which are core modules
    //let git_repo = config.get_git2_repo()?;
    let git_submodule = git_repo.find_submodule(module.get_git_name(&ID).unwrap().as_str()).map_err(|e|format!("Failed to find submodule {ID}: {e}"))?;
    let git_submodule_repo = git_submodule.open()?;
    let module_config_file_modules = match &module_config_file.modules{
        Some(modules) => modules,
        None => return Ok(())
    };
    let config_file_git_submodules = module_config_file.get_all_git_submodules_pair();
    
    //initialize modules that are both git and found in the config file
    for git_submodule in git_submodule_repo.submodules()?{
        let git_submodule_name = git_submodule.name().ok_or(format!("submodule name is not valid UTF-8"))?;
        for (config_submodule_name, config_module) in &config_file_git_submodules{
            let pathed_config_module_name = config_module.get_git_name(config_submodule_name).unwrap();
            //check if submodule is from config.yaml
            if pathed_config_module_name.as_str() == git_submodule_name{
                if let Ok(_) = git_repo.find_submodule(git_submodule_name){
                    println!("Module {git_submodule_name} is already part of the main project git submodules")
                }else{
                    add_module_to_modules_dir(&config_submodule_name, config_module, git_repo, config, multi.clone())?;
                }
                break;
            }
        }
        //todo if not found then its a normal git submodule that must be recursively initiailzed
    }
    //initialize modules that are both curl and found in the config file
    for (config_submodule_name, config_module) in module_config_file_modules{
        if !config_module.is_git(){
            add_module_to_modules_dir(&config_submodule_name, &config_module, git_repo, config, multi.clone())?;
        }
    }
    Ok(())
}

pub fn get_available_modules(config : &ProjectConfig) -> Result<HashMap<String, Module>>{
    let modules_list = fs::read_to_string(config.asharis_root.join("resources").join("moduleslist.yaml"))
        .map_err(|e| format!("Failed to open moduleslist.yaml file: {e}"))?;

    #[derive(Debug, Deserialize, Serialize)]
    struct ModulesList{
        modules : HashMap<String, Module>
    }

    let modules : ModulesList = serde_yaml::from_str(&modules_list)
        .map_err(|e| format!("Failed to parse modules list: {e}"))?;
    
    Ok(modules.modules)
}

pub fn add_module_to_config(config : &ProjectConfig, module : &AddModule, multi : Arc<MultiProgress>) -> Result<Module>{
    let add_module_spinner = Spinner::new(format!("{} module {}", "Adding".green(),  module.name.blue()), Some(multi.clone()));
    defer!{add_module_spinner.finish();}
    let tasks_spinner = Spinner::new(format!("Getting available modules"), Some(multi.clone()));
    defer!{tasks_spinner.finish();};

    let available_modules = get_available_modules(config)?;
    tasks_spinner.change_message(format!("looking for {} in modules lists", &module.name.blue()));
    if let Some((name, module)) =   available_modules.get_key_value(&module.name){
        tasks_spinner.change_message(format!("{} found: adding module config to core.yaml", name.blue()));
        let mut config_file = ConfigFile::new_from_file(&config).map_err(|e| format!("Failed to read config_file: {e}"))?;
        let modules_map = config_file.modules.get_or_insert(HashMap::new());
        modules_map.insert(name.clone(), module.clone());
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
        Err(format!("Failed to find {} in modules list. Try adding it yoursel in the config.yaml file under [modules]", &module.name.blue()))?
    }
}

pub fn add_module(config : &ProjectConfig, add_module : &AddModule, multi : Arc<MultiProgress>) -> Result<()>{
    let module = add_module_to_config(&config, &add_module, multi.clone())
        .map_err(|e| format!("Failed to add module to config: {e}"))?;
    let git_repo = Repository::open(&config.project_paths.root)?;
    add_module_to_modules_dir(add_module.name.as_str(), &module, &git_repo, &config, multi)
        .map_err(|e| format!("Failed to add module to modules dir: {e}"))?;
    println!("{} {} module added.", "Success".green(), add_module.name.blue());
    Ok(())
}

pub fn remove_module(config : &mut ProjectConfig, name : &String, multi : Arc<MultiProgress>) -> Result<()>{
    remove_module_from_dir(config, name, multi.clone())?;
    remove_module_from_config(name, &mut config.config_file)?;
    cmake::generate_to_file_from_path(config, &config.project_paths.root)?;
    Ok(())
}

pub fn remove_module_from_dir(config : &ProjectConfig, name : &String, multi : Arc<MultiProgress>) -> Result<()>{
    let id = config.config_file.get_module_id(name)
        .ok_or_else(|| format!("Failed to find module with name: {name}"))?;
    match config.config_file.get_module_reference_count(name) {
        0 => Err(format!("Failed to find {name} in any config_file modules"))?,
        1 => {},//continue with module deletion, its not any module's dependency
        _ => {
            println!("multiple references to {name}. will not be removed");
            return Ok(())
        },
    }
    let modules = config.get_all_installed_modules_pair()?;

    let module = modules.get(&id);
    if !fs::exists(config.project_paths.modules.join(&id))?{
        println!("module {name} was already removed from modules");
        return Ok(())
    }
    if let Some(module_config_file) = config.config_file.get_config_file(name){
        if let Some(modules) = module_config_file.modules.clone(){
            println!("Removing {name} dependency modules: {:?}", modules.keys());
            for module in modules.keys(){
                remove_module_from_dir(config, module, multi.clone())?;
            }
        }
    }
    if let Some(modules) = &config.config_file.modules{
        if let Some(module) = modules.get(name){
            match &module.source{
                ModuleSource::Curl(_) => {
                    fs::remove_dir_all(config.project_paths.modules.join(name)).map_err(|e| format!("Failed to remove {} directory: {e}", name))?;
                }
                ModuleSource::Git(git) => {
                    remove_git_submodule(name,git.git_version.as_str(), git.git_path.as_deref(),  &config.project_paths.root)?;
                }
            }
        } else {
            Err(format!("Failed to remove {}, not found", name))?;
        }
    }else {
        Err(format!("Failed to remove {}, not found", name))?;
    }
    Ok(())
}

pub fn remove_module_from_config(name : &String,config_file : & mut ConfigFile) -> Result<()>{
    if let Some(modules) = &mut config_file.modules{
        if modules.contains_key(name){
            modules.remove(name);
            config_file.write()?;
        }else{
            Err(format!("Failed to remove {}, not found", name))?;
        }
    }else {
        Err(format!("Failed to remove {}, not found", name))?;
    }
    Ok(())
}
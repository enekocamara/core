use std::fmt::format;
use std::hash::Hash;
use std::process::{Command, Output};
use std::path::{Path,PathBuf};
use std::fs;
use std::sync::Arc;
use std::collections::HashMap;

use git2::{Repository, Submodule};
use indicatif::MultiProgress;
use serde::de::IntoDeserializer;
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
            .args(["submodule", "update", "--init"])
            .current_dir(project_root)
            .output()
            .map_err(|e| format!("Failed to run command: {e}"))?
    };
    if !output.status.success(){
        Err(format!("Failed to init git submodule {name}: {}", String::from_utf8_lossy(&output.stderr)))?
    }
    git_repo.find_submodule(path.as_str()).map_err(|e| format!("Failed to find submodule {name}: {e}").into())
}

pub fn add_module_to_modules_dir(name : &str, module : &Module, git_repo : &Repository, config : &Config,  multi : Arc<MultiProgress>)->Result<()>{

    match module{
        Module::GitUrl(git_url) => {
            add_git_submodule(&git_repo, name,None, &git_url, None, &config.project_paths.root, multi.clone())?;
        }
        Module::Spec(spec) => {
            match &spec.source{
                ModuleSource::Curl(curl) =>{
                    match curl {
                        ModuleSourceCurl::CurlUrl{curl_url} =>{
                            utils::curl_url(&curl_url, &config.project_paths.root.join("modules").join(name))
                                .map_err(|e| format!("Failed to add module {name} : {e}"))?;
                        }
                        ModuleSourceCurl::CurlUrls{curl_urls} =>{
                            let path = config.project_paths.root.join("modules").join(name);
                            for url in curl_urls{
                                utils::curl_url(url, &path).map_err(|e| format!("Failed to add module {name} : {e}"))?;
                            }
                        }
                    }
                }
                ModuleSource::Git(git) => {
                    add_git_submodule(&git_repo, name, git.git_path.as_deref(), &git.git_url, git.git_branch.as_deref(), &config.project_paths.root, multi.clone())?;
                }
            };
            if let Some(cmd) = &spec.cmd{
                let output = Command::new(cmd.exec.as_str())
                    .args(&cmd.args)
                    .current_dir(config.project_paths.modules.join(name))
                    .output()?;
                if !output.status.success(){
                    Err(format!("Failed to run module setupt command: {} {}", &cmd.exec, cmd.args.join(" ")))?;
                }
            }
            if let Some(cmake) = &spec.cmake{
                if let Some(true) = cmake.gen{
                    let cmake_template = CMakeTemplate::new(&config);
                    let path = config.project_paths.modules.join(name);
                    //include  path is for main cmakelists not for generated one
                    let path_vec = match &cmake.include_path{
                        Some(path) => {
                            Some(vec![path.clone()])
                        }
                        None => None
                    }; 
                    cmake_template.generate_to_file(config, path,&GeneratePattern{
                        project_name : name,
                        add_command : "static_library",
                        include_paths: path_vec.as_ref(),
                        link_modules : None,
                        subdirectories : None,
                        sources_path : cmake.sources_path.as_deref(),
                        recursive_glob : cmake.recursive_glob
                    }).map_err(|e| format!("Failed to generate cmake_file: {e}"))?;
                }
            }
        }
    };
    //early return only if its not git. if so we need to add them recursively and handle core module dependencies
    if !module.is_git(){
        return Ok(())
    }
    //handle childs and module dependencies
    let path_to_config_file = config.project_paths.modules.join(name).join("config.yaml");
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
    let module_config_file = ConfigFile::new_from_path(config, &path_to_config_file)?;
    //need to figure out which ones are git submodules and which are core modules
    //let git_repo = config.get_git2_repo()?;
    let git_submodule = git_repo.find_submodule(module.get_git_name(name).unwrap().as_str()).map_err(|e|format!("Failed to find submodule {name}: {e}"))?;
    let git_submodule_repo = git_submodule.open()?;
    let module_config_file_modules = match &module_config_file.modules{
        Some(modules) => modules,
        None => return Ok(())
    };
    let config_file_git_submodules = module_config_file.get_all_git_submodules_pair();
    
    //initialize modules that are both git and found in the config file
    for git_submodule in git_submodule_repo.submodules()?{
        let git_submodule_name = git_submodule.name().ok_or(format!("submodule name is not valid UTF-8"))?;
        let mut is_a_config_module = false;
        for (config_submodule_name, config_module) in &config_file_git_submodules{
            let pathed_config_module_name = config_module.get_git_name(config_submodule_name).unwrap();
            //check if submodule is from config.yaml
            is_a_config_module = true;
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

pub fn get_available_modules(config : &Config) -> Result<HashMap<String, Module>>{
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

pub fn add_module_to_config(config : &Config, module : &AddModule, multi : Arc<MultiProgress>) -> Result<Module>{
    let add_module_spinner = Spinner::new(format!("Adding module {}", module.name), Some(multi.clone()));
    defer!{add_module_spinner.finish();}
    let tasks_spinner = Spinner::new(format!("Getting available modules"), Some(multi.clone()));
    defer!{tasks_spinner.finish();};

    let available_modules = get_available_modules(config)?;
    tasks_spinner.change_message(format!("looking for {} in modules lists", &module.name));
    if let Some((name, module)) =   available_modules.get_key_value(&module.name){
        tasks_spinner.change_message(format!("module found: adding module config to core.yaml"));
        let mut config_file = ConfigFile::new_from_file(&config).map_err(|e| format!("failed to read config_file: {e}"))?;
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
    let git_repo = Repository::open(&config.project_paths.root)?;
    let config_file = ConfigFile::new_from_file(&config)?;
    add_module_to_modules_dir(add_module.name.as_str(), &module, &git_repo, &config, multi)
        .map_err(|e| format!("Failed to add module to modules dir: {e}"))?;
    generate_cmake_from_conf(&config).map_err(|e|format!("Failed to generate updated cmakefile: {e}"))?;
    Ok(())
}
pub fn remove_module(config : &Config, name : &String, multi : Arc<MultiProgress>, config_file : & mut ConfigFile) -> Result<()>{
    if let Ok(mut module_config_file) = ConfigFile::new_from_path(config, &config.project_paths.modules.join(name).join("config.yaml")){
        if let Some(modules) = module_config_file.modules.clone(){
            println!("Removing {name} dependency modules: {:?}", modules.keys());
            for module in modules.keys(){
                remove_module(config, module, multi.clone(), &mut module_config_file)?;
            }
        }else{
            println!("Failed");
        }
    }else{
        println!("Failed");
    }
    if let Some(modules) = &mut config_file.modules{
        if let Some(specs) = modules.get(name){
            match &specs{
                Module::GitUrl(_) => {
                    remove_git_submodule(name, None,&config.project_paths.root)?;
                },
                Module::Spec(specs) => {
                    match &specs.source{
                        ModuleSource::Curl(_) => {
                            fs::remove_dir_all(config.project_paths.modules.join(name)).map_err(|e| format!("Failed to remove {} directory: {e}", name))?;
                        }
                        ModuleSource::Git(git) => {
                            remove_git_submodule(name,git.git_path.as_deref(),  &config.project_paths.root)?;
                        }
                    }
                }
            }
            modules.remove(name);
            config_file.write()?;
        }
    }else {
        Err(format!("Failed to remove {}, not found", name))?;
    }
    
    Ok(())
}
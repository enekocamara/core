use std::path::PathBuf;
use std::env;
use std::collections::{HashMap, HashSet};
use std::fmt;
use std::fs;
use std::hash::{Hash, Hasher};

use colored::Colorize;
use serde::{Deserialize, Serialize};
use serde::de::{Visitor, MapAccess};
use serde::de::Error as SerdeError;
use git2::Repository;

use crate::args::BinaryType;
use crate::utils;
use crate::Result;

#[derive(Debug, Deserialize, Serialize)]
pub struct ConfigFile{
    pub project : String,
    pub current_target : String,
    pub targets : HashMap<String, BinaryType>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub build_ops : Option<HashMap<String, Build>>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub modules : Option<HashMap<String, Module>>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub cmp_defs : Option<Vec<String>>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub command : Option<HashMap<String, ClapSerdeCommand>>,
    #[serde(skip)]
    path : PathBuf,
    #[serde(skip)]
    childs : Vec<ConfigFile>
}

#[derive(Debug, Deserialize, Serialize)]
pub struct ClapSerdeCommand{
    program : String,
    #[serde(skip_serializing_if = "Option::is_none")]
    args : Option<Vec<String>>,
    #[serde(skip_serializing_if = "Option::is_none")]
    current_dir_self : Option<bool>
}

impl ClapSerdeCommand{
    pub fn run(&self, config : &Config, path_to_config_file : &PathBuf, runtime_args : Option<Vec<String>>) -> Result<()>{
        let mut command_str = format!("cmd C/ {}", &self.program);
        let mut command = std::process::Command::new("cmd");
        command.arg("/C")
            .arg(&self.program);
    
        if let Some(command_args) = &self.args{
            command.args(command_args);
            command_str.push_str(command_args.join(" ").as_str());
        }
        if let Some(runtime_args) = runtime_args{
            command.args(&runtime_args);
            command_str.push_str(runtime_args.join(" ").as_str());
        }
        let directory = {
            if matches!(self.current_dir_self, None | Some(false)){
                command.current_dir(&config.project_paths.root);
                &config.project_paths.root
            }else{
                command.current_dir(&path_to_config_file);
                &path_to_config_file
            }
        };

        let status = command.status()?;
        if !status.success(){
            Err(format!("Exit code: {:?}\nRun `{}` from `{:?}`", status.code(), command_str, directory))?
        }
        Ok(())
    }
}

#[derive(PartialEq)]
pub struct CmakeModule{
    pub module_name : String,
    pub project_name : String
}

impl Eq for CmakeModule{}

impl Hash for CmakeModule{
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.module_name.hash(state);
    }
}

impl ConfigFile{
    pub fn new_from_path(config : &Config, path : &PathBuf) -> Result<ConfigFile>{
        let file_source = fs::read_to_string(path)
            .map_err(|e| format!("Failed to read config.yaml: {e}"))?;
        let mut file = serde_yaml::from_str::<ConfigFile>(&file_source).map_err(|e| format!("Failed to parse config.yaml: {e}"))?;

        //check if modules have config files if so integrate
        if let Some(modules) = &file.modules{
            for name in modules.keys(){
                let module_config_path = config.project_paths.modules.join(&name).join("config.yaml");
                if fs::exists(&module_config_path).unwrap(){
                    file.childs.push(ConfigFile::new_from_path(config, &module_config_path)?);
                }
            }
        }
        file.path = path.clone();
        Ok(file)
    }
    pub fn new_from_file(config : &Config) -> Result<ConfigFile>{
        ConfigFile::new_from_path(config, &config.project_paths.config_file)
    }
    pub fn get_path(&self) -> &PathBuf{
        &self.path
    }
    pub fn get_dir_path(&self) -> PathBuf{
        let mut hold = self.path.clone();
        hold.pop();
        hold
    }
    pub fn write(self : &Self) -> Result<()>{
        let source = serde_yaml::to_string(self).map_err(|e| format!("Failed to parse to string: {e}"))?;
        fs::write(&self.path, source).map_err(|e| format!("Failed to overwrite config.yaml: {e}"))?;
        Ok(())
    }
    pub fn get_all_git_submodules<'a>(&'a self) -> Vec<&'a str>{
        if let Some(modules) = &self.modules{
            let mut submodules = Vec::new();
            for (name, specs) in modules{
                match specs{
                    Module::GitUrl(_) => {
                        submodules.push(name.as_str());
                    }
                    Module::Spec(specs) => {
                        match specs.source {
                            ModuleSource::Curl(_)=>{}
                            ModuleSource::Git(_)=>{
                                submodules.push(name.as_str());
                            }
                        }
                    }
                }
            }
            submodules
        }else {
            Vec::new()
        }
    }
    pub fn get_all_git_submodules_pair<'a>(&'a self) -> Vec<(&'a String, &'a Module)>{
        if let Some(modules) = &self.modules{
            let mut submodules = Vec::new();
            for (name, module) in modules{
                match module{
                    Module::GitUrl(_) => {
                        submodules.push((name, module));
                    }
                    Module::Spec(specs) => {
                        match specs.source {
                            ModuleSource::Curl(_)=>{}
                            ModuleSource::Git(_)=>{
                                submodules.push((name, module))
                            }
                        }
                    }
                }
            }
            submodules
        }else {
            Vec::new()
        }
    }
    pub fn get_all_cmake_modules(&self, project_root : &PathBuf) -> Result<HashSet<CmakeModule>>{
        let mut cmake_modules = HashSet::new();
        if let Some(modules) = &self.modules{
            for (name, module) in modules{
                let cmake_path = project_root.join("modules").join(name).join("CMakeLists.txt");
                if fs::exists(&cmake_path).map_err(|e| format!("Failed to check if cmake exists: {e}"))?{
                    
                    cmake_modules.insert(CmakeModule{
                        module_name : name.clone(),
                        project_name : {
                            if let Some(link_name) = module.get_link_name(){
                                link_name
                            }else{
                                utils::get_cmake_project_name(project_root.join("modules").join(name))
                                    .map_err(|e| format!("Failed to get cmake project name for {name}: {e}"))?
                            }
                        }
                    });
                }
            }
        }
        for child in &self.childs{
            let child_modules = child.get_all_cmake_modules(project_root)?;
            for child_mod in child_modules{
                cmake_modules.insert(child_mod);
            }
        }
        Ok(cmake_modules)
    }
    pub fn get_all_submodules_pair(self : &Self, recurse : bool) -> HashMap<String, Module>{
        let mut modules = if let Some(modules) = &self.modules{
            modules.clone()
        }else{
            HashMap::new()
        };
        if recurse{
            for child in &self.childs{
                let child_modules = child.get_all_submodules_pair(true);
                for (name, module) in child_modules{
                    modules.insert(name, module);
                }
            }
        }
        modules
    }

    pub fn get_module_reference_count(self : &Self, to_find : &str) -> u32{
        let mut count = 0;
        if let Some(modules) = &self.modules{
            if modules.contains_key(to_find){
                count += 1;
            }
            for child in &self.childs{
                count += child.get_module_reference_count(to_find);
            }
            count
        }else {
            return 0;
        }
    }

    pub fn get_config_file(self: &Self, name : &str) -> Option<&ConfigFile>{
        if self.project == *name{
            return Some(self)
        }
        self.childs.iter().find(|module_config_file| module_config_file.project == *name)
    }
    pub fn get_module_path<'a>(&'a self, name : &str) -> Result<&'a PathBuf>{
        if self.project == *name{
            return Ok(&self.path)
        }
        return self.childs.iter().find_map(|module_config_file|{
            if module_config_file.project == *name{
               return  Some(&module_config_file.path)
            }
            None
        } ).ok_or(crate::Error::custom(format!("Module {} not found", name.blue() )))
    }
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Build{
    #[serde(skip_serializing_if = "Option::is_none")]
    pub rebuild : Option<Vec<String>>
}

#[derive(Debug, Deserialize, Serialize, Clone)]
#[serde(untagged)]
pub enum Module{
    GitUrl(String),
    Spec(ModuleSpec)
}

impl Module{
    pub fn get_git_path(self : &Self) -> Option<&str>{
        match self{
            Module::GitUrl(_) => None,
            Module::Spec(spec) => match &spec.source {
                ModuleSource::Curl(_) => None,
                ModuleSource::Git(git) => git.git_path.as_deref()
            }
        }
    }
    pub fn get_git_name(self : &Self, name : &str) -> Option<String>{
        match self{
            Module::GitUrl(_) => Some(format!("modules/{name}")),
            Module::Spec(spec) => match &spec.source{
                ModuleSource::Git(git) => {
                    if let Some(git_path) = &git.git_path{
                        return Some(format!("modules/{name}/{git_path}"))
                    }
                    Some(format!("modules/{name}"))
                }
                ModuleSource::Curl(_) => None
            }
        }
    }
    pub fn is_git(self : &Self) -> bool{
        match self{
            Module::GitUrl(_) => true,
            Module::Spec(spec) => match &spec.source{
                ModuleSource::Git(_) => true,
                ModuleSource::Curl(_) => false
            }

        }
    }
    pub fn get_link_name(self : &Self) -> Option<String>{
        match self{
            Module::GitUrl(_) => None,
            Module::Spec(spec) => {
                spec.link_name.clone()
            }
        }
    }
}

#[derive(Debug, Deserialize, Serialize, Clone)]
pub struct ModuleSpec{
    #[serde(flatten)]
    pub source : ModuleSource,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub src_path : Option<String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub cmake : Option<Cmake>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub include_path : Option<String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub cmd : Option<Cmd>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub link_name : Option<String>,
}

#[derive(Debug, Deserialize, Serialize, Clone)]
pub struct Cmd{
    pub exec : String,
    pub args : Vec<String>
}

#[derive(Debug, Deserialize, Serialize, Clone)]
pub struct Cmake{
    #[serde(skip_serializing_if = "Option::is_none")]
    pub gen : Option<bool>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub recursive_glob : Option<bool>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub sources_path : Option<String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub include_path : Option<String>
}

/*
#[derive(Debug, Deserialize, Serialize, Clone)]
pub enum CmakeSources{
    Glob,
    RecursiveGlob
}*/

/* this works
#[derive(Debug, Serialize)]
#[serde(untagged)]
pub enum modulesSource{
    Git{git_url : String, git_branch : Option<String>},
    Curl{curl_url : String},
}*/

#[derive(Debug, Serialize, Clone)]
#[serde(untagged)]
pub enum ModuleSource{
    Git(ModuleSourceGit),
    Curl(ModuleSourceCurl),
}

#[derive(Debug, Deserialize, Serialize, Clone)]
#[serde(untagged)]
pub enum ModuleSourceCurl{
    CurlUrl{ curl_url : String},
    CurlUrls{ curl_urls : Vec<String>}
}

#[derive(Debug, Deserialize, Serialize, Clone)]
pub struct ModuleSourceGit{
    pub git_url : String,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub git_branch : Option<String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub git_path : Option<String>
}

#[derive(Debug)]
pub enum ModuleSourceError {
    BothUrlsProvided,
    NoUrlProvided,
    UnknownField(String),
    Custom(String),
}

impl fmt::Display for ModuleSourceError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            ModuleSourceError::BothUrlsProvided => write!(f, "Cannot have both 'git_url' and 'curl_url'"),
            ModuleSourceError::NoUrlProvided => write!(f, "Must have either 'git_url' or 'curl_url'"),
            ModuleSourceError::UnknownField(ref field) => write!(f, "Unknown field: {}", field),
            ModuleSourceError::Custom(ref msg) => write!(f, "Custom error: {}", msg),
        }
    }
}

impl<'de> Deserialize<'de> for ModuleSource{
    fn deserialize<D>(deserializer: D) -> core::result::Result<Self, D::Error>
        where
            D: serde::Deserializer<'de> {
        struct ModulesSourceVisitor;
        impl <'de> Visitor<'de> for ModulesSourceVisitor{
            type Value = ModuleSource;
            fn expecting(&self, formatter: &mut std::fmt::Formatter) -> std::fmt::Result {
                formatter.write_str("use git_url or curl_url(s), not both")
            }
            fn visit_map<A>(self,mut map: A) -> core::result::Result<Self::Value, A::Error>
                where
                    A: MapAccess<'de>, {
                let mut git_url = None;
                let mut git_branch = None;
                let mut git_path = None;
                let mut curl_url = None;
                let mut curl_urls = None;

                while let Some(key) = map.next_key::<String>()?{
                    match key.as_str(){
                        "git_url" => git_url = Some(map.next_value()?),
                        "git_branch" => git_branch = Some(map.next_value()?),
                        "git_path" => git_path = Some(map.next_value()?),

                        "curl_url" => curl_url = Some(map.next_value()?),
                        "curl_urls" => curl_urls = Some(map.next_value()?),
                        _ => return Err(A::Error::unknown_field(&key, &["git_url", "curl_url", "curl_urls", "git_branch"])),
                    }
                }

                match (git_url, git_branch, git_path, curl_url, curl_urls){
                    (Some(git_url), git_branch, git_path, None, None) => Ok(ModuleSource::Git(ModuleSourceGit{ git_url, git_branch, git_path})),
                    (None, None, None,Some(curl_url), None) => Ok(ModuleSource::Curl(ModuleSourceCurl::CurlUrl { curl_url })),
                    (None, None, None, None, Some(curl_urls)) => Ok(ModuleSource::Curl(ModuleSourceCurl::CurlUrls { curl_urls })),

                    //incorrect git_branch
                    (None, Some(_), _, _, _) => Err(A::Error::custom("git branch without git url")),
                    (None, _, Some(_), _, _) => Err(A::Error::custom("git path without git url")),

                    //incorrect curl
                    (_, _,_, Some(_), Some(_)) => Err(A::Error::custom("cannot have both curl_url and curl urls")),

                    //incorrect git curl
                    (Some(_), _,_, Some(_), _) | (Some(_),_, _, _, Some(_)) => Err(A::Error::custom("cannot have both git url and curl_url(s)")),

                    (None,None,None,None,None) => Err(A::Error::custom("git url or curl url(s) must be set"))
                }
            }
        }
        deserializer.deserialize_map(ModulesSourceVisitor)
    }
}
    


#[derive(Clone)]
pub struct Config{
    pub asharis_root : PathBuf,
    pub project_paths : ProjectPaths,
    pub cmd_path : PathBuf,
    pub flags : Flags,
}

#[derive(Clone)]
pub struct Flags{
    pub project_name : &'static str,
    pub project_name_first_upper : &'static str,
    pub cmake : CMakeFlags,
}

#[derive(Clone)]
pub struct CMakeFlags{
    pub add_command : &'static str,
    pub modules_include_paths : &'static str,
    pub link_modules : &'static str,
    pub add_subdirectories : &'static str,
    pub sources_path : &'static str,
    pub glob_type : &'static str,
    pub compile_definitions : &'static str,

}

#[derive(Clone)]
pub struct ProjectPaths{
    pub root : PathBuf,
    pub build : PathBuf,
    pub output : PathBuf,
    pub config_file : PathBuf,
    pub modules : PathBuf,
    pub src : PathBuf
}

impl Config{
    pub fn new() -> Result<Config>{
        let asharis_root : PathBuf = PathBuf::from(env::var("ASHARIS_ROOT").map_err(|e| format!("failed to find ASHARIS_ROOT in environment variables: {:?}", e))?);
        let cmd_path = env::current_dir().map_err(|e| format!("environment variable pwd not set: {}", e))?;
        let project_root = {
            let parent_dir = {
                let mut hold = cmd_path.clone();
                hold.pop();
                hold.pop();
                hold
            };
            if fs::exists(parent_dir.join("config.yaml"))?{
                parent_dir
            }else if fs::exists(cmd_path.join("config.yaml"))?{
                cmd_path.clone()
            }else{
                Err("config.yaml file couldn't be found")?
            }
        };
        Ok(Config {
            asharis_root,
            project_paths : ProjectPaths{
                root : project_root.clone(),
                build : project_root.join("build"),
                output : project_root.join("output"),
                config_file : project_root.join("config.yaml"),
                modules : project_root.join("modules"),
                src : project_root.join("src")
            },
            cmd_path,
            flags : Flags{
                project_name : "%PROJECT_NAME%",
                project_name_first_upper : "%PROJECT_NAME_FIRST_UPPER%",
                cmake : CMakeFlags{ 
                    add_command : "%ADD%",
                    modules_include_paths : "%MODULES_INCLUDE_PATHS%",
                    link_modules : "%LINK_MODULES%",
                    add_subdirectories : "%ADD_SUBDIRECTORIES%",
                    sources_path : "%SOURCES_PATH%",
                    glob_type : "%GLOB_TYPE%",
                    compile_definitions : "%COMPILE_DEFINITIONS%"
                }
            }
        })
    }
    pub fn get_git2_repo(self : &Self) -> Result<Repository>{
        Repository::open(&self.project_paths.root).map_err(|e| format!("Failed to open git repository: {e}").into())
    }
    pub fn get_all_installed_modules(self : &Self)->Result<Vec<String>>{
        let mut modules = Vec::new();
        for entry in fs::read_dir(&self.project_paths.modules)?{
            let entry = entry?;
            if entry.metadata()?.is_dir(){
                if let Some(name) = entry.file_name().to_str(){
                    modules.push(name.to_string());
                }
            }
        }
        Ok(modules)
    }
    pub fn get_all_installed_modules_pair(self : &Self)->Result<HashMap<String, Module>>{
        let mut modules = HashMap::new();
        let config_file = ConfigFile::new_from_file(self)?;
        let config_tree_modules = config_file.get_all_submodules_pair(true);
        for entry in fs::read_dir(&self.project_paths.modules)?{
            let entry = entry?;
            if entry.metadata()?.is_dir(){
                if let Some(name) = entry.file_name().to_str(){
                    if let Some((name, module)) = config_tree_modules.get_key_value(name){
                        modules.insert(name.clone(), module.clone());
                    } 
                }
            }
        }
        Ok(modules)
    }
    pub fn change_root(self : &mut Self, root : PathBuf){
        self.project_paths.root = root;
        self.project_paths.build = self.project_paths.root.join("build");
        self.project_paths.output = self.project_paths.root.join("output");
        self.project_paths.config_file = self.project_paths.root.join("config.yaml");
        self.project_paths.modules =  self.project_paths.root.join("modules");
        self.project_paths.src = self.project_paths.root.join("src")
    }
}

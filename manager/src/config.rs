use std::path::PathBuf;
use std::env;
use std::collections::HashMap;
use serde::{Deserialize, Serialize};
use serde::de::{Visitor, MapAccess};
use serde::de::Error as SerdeError;
use std::fmt::{self, format};
use std::fs;

use crate::utils::{self, get_cmake_project_name};

use crate::Result;

#[derive(Debug, Deserialize, Serialize)]
pub struct ConfigFile{
    pub project : String,
    pub build : String,
    pub builds : HashMap<String, String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub build_ops : Option<HashMap<String, Build>>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub modules : Option<HashMap<String, Module>>,

    #[serde(skip)]
    path : PathBuf
}

pub struct CmakeModule{
    pub module_name : String,
    pub project_name : String
}

impl ConfigFile{
    pub fn new_from_file(config : &Config) -> Result<ConfigFile>{
        let file_source = fs::read_to_string(config.project_root.join("config.yaml"))
            .map_err(|e| format!("Failed to read config.yaml: {e}"))?;
        let mut file = serde_yaml::from_str::<ConfigFile>(&file_source).map_err(|e| format!("Failed to parse config.yaml: {e}"))?;
        file.path = config.project_root.join("config.yaml");
        Ok(file)
    }
    pub fn write(self : &Self) -> Result<()>{
        let source = serde_yaml::to_string(self)?;
        fs::write(&self.path, source)?;
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
    pub fn get_all_cmake_modules(&self, project_root : &PathBuf) -> Result<Vec<CmakeModule>>{
        let mut cmake_modules = Vec::new();
        if let Some(modules) = &self.modules{
            for module in modules.keys(){
                let cmake_path = project_root.join("modules").join(module).join("CMakeLists.txt");
                if fs::exists(&cmake_path).map_err(|e| format!("Failed to check if cmake exists: {e}"))?{
                    
                    cmake_modules.push(CmakeModule{
                        module_name : module.clone(),
                        project_name : get_cmake_project_name(project_root.join("modules").join(module)).map_err(|e| format!("Failed to get cmake project name for {module}: {e}"))?
                    });
                }
            }
        }
        Ok(cmake_modules)
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
    pub sources_path : Option<String>
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
        struct modulesSourceVisitor;
        impl <'de> Visitor<'de> for modulesSourceVisitor{
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
        deserializer.deserialize_map(modulesSourceVisitor)
    }
}
    


#[derive(Clone)]
pub struct Config{
    pub asharis_root : PathBuf,
    pub project_root : PathBuf,
    pub project_name_flag : &'static str,
    pub project_name_first_upper_flag : &'static str,
    pub cmake_add_command_flag : &'static str,
    pub cmake_modules_include_paths_flag : &'static str,
    pub cmake_link_modules_flag : &'static str,
    pub cmake_add_subdirectories_flag : &'static str,
    pub cmake_sources_path_flag : &'static str,
    pub cmake_glob_type_flag : &'static str,
}

impl Config{
    pub fn new() -> Result<Config>{
        let asharis_root : PathBuf = PathBuf::from(env::var("ASHARIS_ROOT").map_err(|e| format!("failed to find ASHARIS_ROOT in environment variables: {:?}", e))?);
        let project_root = env::current_dir().map_err(|e| format!("environment variable pwd not set: {}", e))?;
        Ok(Config {
            asharis_root,
            project_root,
            project_name_flag : "%PROJECT_NAME%",
            project_name_first_upper_flag : "%PROJECT_NAME_FIRST_UPPER%",
            cmake_add_command_flag : "%ADD%",
            cmake_modules_include_paths_flag : "%MODULES_INCLUDE_PATHS%",
            cmake_link_modules_flag : "%LINK_MODULES%",
            cmake_add_subdirectories_flag : "%ADD_SUBDIRECTORIES%",
            cmake_sources_path_flag : "%SOURCES_PATH%",
            cmake_glob_type_flag : "%GLOB_TYPE%"
        })
    }
}
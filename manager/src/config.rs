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
    pub target : String,
    pub targets : HashMap<String, String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub build : Option<HashMap<String, Build>>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub vendor : Option<HashMap<String, Vendor>>
}

pub struct CmakeModule{
    pub module_name : String,
    pub project_name : String
}

impl ConfigFile{
    pub fn get_all_git_submodules<'a>(&'a self) -> Vec<&'a str>{
        if let Some(vendor) = &self.vendor{
            let mut submodules = Vec::new();
            for (name, specs) in vendor{
                match specs{
                    Vendor::GitUrl(_) => {
                        submodules.push(name.as_str());
                    }
                    Vendor::Spec(specs) => {
                        match specs.source {
                            VendorSource::Curl(_)=>{}
                            VendorSource::Git(_)=>{
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
        let mut modules = Vec::new();
        if let Some(vendor) = &self.vendor{
            for module in vendor.keys(){
                let cmake_path = project_root.join("vendor").join(module).join("CMakeLists.txt");
                if fs::exists(&cmake_path).map_err(|e| format!("Failed to check if cmake exists: {e}"))?{
                    
                    modules.push(CmakeModule{
                        module_name : module.clone(),
                        project_name : get_cmake_project_name(project_root.join("vendor").join(module)).map_err(|e| format!("Failed to get cmake project name for {module}: {e}"))?
                    });
                }
            }
        }
        Ok(modules)
    }
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Build{
    #[serde(skip_serializing_if = "Option::is_none")]
    pub rebuild : Option<Vec<String>>
}

#[derive(Debug, Deserialize, Serialize, Clone)]
#[serde(untagged)]
pub enum Vendor{
    GitUrl(String),
    Spec(VendorSpec)
}

#[derive(Debug, Deserialize, Serialize, Clone)]
pub struct VendorSpec{
    #[serde(flatten)]
    pub source : VendorSource,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub src_path : Option<String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub cmake_path : Option<String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub include_path : Option<String>,
}

/* this works
#[derive(Debug, Serialize)]
#[serde(untagged)]
pub enum VendorSource{
    Git{git_url : String, git_branch : Option<String>},
    Curl{curl_url : String},
}*/

#[derive(Debug, Serialize, Clone)]
#[serde(untagged)]
pub enum VendorSource{
    Git(VendorSourceGit),
    Curl(VendorSourceCurl),
}

#[derive(Debug, Deserialize, Serialize, Clone)]
#[serde(untagged)]
pub enum VendorSourceCurl{
    CurlUrl{ curl_url : String},
    CurlUrls{ curl_urls : Vec<String>}
}

#[derive(Debug, Deserialize, Serialize, Clone)]
pub struct VendorSourceGit{
    pub git_url : String,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub git_branch : Option<String>
}

#[derive(Debug)]
pub enum VendorSourceError {
    BothUrlsProvided,
    NoUrlProvided,
    UnknownField(String),
    Custom(String),
}

impl fmt::Display for VendorSourceError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            VendorSourceError::BothUrlsProvided => write!(f, "Cannot have both 'git_url' and 'curl_url'"),
            VendorSourceError::NoUrlProvided => write!(f, "Must have either 'git_url' or 'curl_url'"),
            VendorSourceError::UnknownField(ref field) => write!(f, "Unknown field: {}", field),
            VendorSourceError::Custom(ref msg) => write!(f, "Custom error: {}", msg),
        }
    }
}

impl<'de> Deserialize<'de> for VendorSource{
    fn deserialize<D>(deserializer: D) -> core::result::Result<Self, D::Error>
        where
            D: serde::Deserializer<'de> {
        struct VendorSourceVisitor;
        impl <'de> Visitor<'de> for VendorSourceVisitor{
            type Value = VendorSource;
            fn expecting(&self, formatter: &mut std::fmt::Formatter) -> std::fmt::Result {
                formatter.write_str("use git_url or curl_url(s), not both")
            }
            fn visit_map<A>(self,mut map: A) -> core::result::Result<Self::Value, A::Error>
                where
                    A: MapAccess<'de>, {
                let mut git_url = None;
                let mut git_branch = None;
                let mut curl_url = None;
                let mut curl_urls = None;

                while let Some(key) = map.next_key::<String>()?{
                    match key.as_str(){
                        "git_url" => git_url = Some(map.next_value()?),
                        "git_branch" => git_branch = Some(map.next_value()?),
                        "curl_url" => curl_url = Some(map.next_value()?),
                        "curl_urls" => curl_urls = Some(map.next_value()?),
                        _ => return Err(A::Error::unknown_field(&key, &["git_url", "curl_url", "curl_urls", "git_branch"])),
                    }
                }

                match (git_url, git_branch, curl_url, curl_urls){
                    (Some(git_url), git_branch, None, None) => Ok(VendorSource::Git(VendorSourceGit{ git_url, git_branch })),
                    (None, None, Some(curl_url), None) => Ok(VendorSource::Curl(VendorSourceCurl::CurlUrl { curl_url })),
                    (None, None, None, Some(curl_urls)) => Ok(VendorSource::Curl(VendorSourceCurl::CurlUrls { curl_urls })),

                    //incorrect git_branch
                    (None, Some(_), _, _) => Err(A::Error::custom("git branch without git url")),

                    //incorrect curl
                    (_, _, Some(_), Some(_)) => Err(A::Error::custom("cannot have both curl_url and curl urls")),

                    //incorrect git curl
                    (Some(_), _, Some(_), _) | (Some(_), _, _, Some(_)) => Err(A::Error::custom("cannot have both git url and curl_url(s)")),

                    (None,None,None,None) => Err(A::Error::custom("git url or curl url(s) must be set"))
                }
            }
        }
        deserializer.deserialize_map(VendorSourceVisitor)
    }
}
    


#[derive(Clone)]
pub struct Config{
    pub asharis_root : PathBuf,
    pub project_root : PathBuf,
    pub project_name_flag : &'static str,
    pub project_name_first_upper_flag : &'static str,
    pub cmake_add_command_flag : &'static str,
    pub cmake_vendor_include_paths_flag : &'static str,
    pub cmake_vendor_link_libraries_flag : &'static str,
    pub cmake_add_subdirectories_flag : &'static str,
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
            cmake_vendor_include_paths_flag : "%VENDOR_INCLUDE_PATHS%",
            cmake_vendor_link_libraries_flag : "%VENDOR_LINK_LIBRARIES%",
            cmake_add_subdirectories_flag : "%ADD_SUBDIRECTORIES%"
        })
    }
}
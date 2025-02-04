use std::path::PathBuf;
use std::env;

#[derive(Clone)]
pub struct Config{
    pub asharis_root : PathBuf,
    pub project_root : PathBuf,
    pub project_name_flag : &'static str,
    pub project_name_first_upper_flag : &'static str,
}

impl Config{
    pub fn new() -> Result<Config, String>{
        let asharis_root : PathBuf = PathBuf::from(env::var("ASHARIS_ROOT").map_err(|e| format!("failed to find ASHARIS_ROOT in environment variables: {:?}", e))?);
        let project_root = env::current_dir().map_err(|e| format!("environment variable pwd not set: {}", e))?;
        Ok(Config {
            asharis_root,
            project_root,
            project_name_flag : "%PROJECT_NAME%",
            project_name_first_upper_flag : "%PROJECT_NAME_FIRST_UPPER"
        })
    }
}
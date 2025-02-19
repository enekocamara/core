use std::{fs, path::PathBuf};

use tokio::io::copy_bidirectional_with_sizes;

use crate::config::Config;
use crate::Result;
pub struct CMakeTemplate{
    path_to_template : PathBuf,
}

pub struct GeneratePattern<'a>{
    pub project_name : &'a str,
    pub add_command : &'a str,
    pub include_paths : Option<&'a Vec<String>>,
    pub link_modules : Option<&'a Vec<String>>,
    pub subdirectories : Option<&'a  Vec<String>>,
    pub sources_path : Option<&'a str>,
    pub recursive_glob : Option<bool>
}

impl CMakeTemplate{
    pub fn new(config : &Config) -> CMakeTemplate{
        CMakeTemplate{
            path_to_template : config.asharis_root.join("resources").join("TemplateCMakeLists.txt")
        }
    }

    pub fn generate_to_string(self: &Self, config : &Config,  pattern : &GeneratePattern)
         -> Result<String>{
        let source = fs::read_to_string(&self.path_to_template)?;

        let mut modified = source
            .replace(&config.project_name_flag, pattern.project_name);
        
        let add_command = match pattern.add_command{
            "static_library" => {
                format!("add_library({} STATIC ${{SOURCES}})", pattern.project_name)
            }
            "dynamic_library" => {
                format!("add_library({} DYNAMIC ${{SOURCES}})", pattern.project_name)
            }
            "executable" => {
                format!("add_executable({} ${{SOURCES}})", pattern.project_name)
            }
            other => {
                return Err(format!("target type unavailable {}, use static_library, dynamic_library or executable.", other).into());
            }
        };
        modified = modified.replace(config.cmake_add_command_flag, add_command.as_str());

        if let Some(include_paths) = pattern.include_paths{
            let include_paths = include_paths.iter()
                .map(|path| format!("\t${{CMAKE_SOURCE_DIR}}/modules/{path}"))
                .collect::<Vec<_>>()
                .join("\n");
            modified = modified.replace(config.cmake_modules_include_paths_flag, &include_paths);
        }else{
            modified = modified.replace(config.cmake_modules_include_paths_flag, "");
        }
        if let Some(link_modules) = pattern.link_modules{
            let link_modules = link_modules.join(" ");
            modified = modified.replace(config.cmake_link_modules_flag, &link_modules);
        }else{
            modified = modified.replace(config.cmake_link_modules_flag, "");
        }
        if let Some(sources_path) = pattern.sources_path{
            modified = modified.replace(config.cmake_sources_path_flag, sources_path);
        }else{
            modified = modified.replace(config.cmake_sources_path_flag, "src");
        }
        if matches!(pattern.recursive_glob, None | Some(true)){
            modified = modified.replace(config.cmake_glob_type_flag, "GLOB_RECURSE");
        }else{
            modified = modified.replace(config.cmake_glob_type_flag, "GLOB");
        }

        if let Some(subdirectories) = pattern.subdirectories{
            let subdirectories = format!(
                "\n{}",
                subdirectories
                    .iter()
                    .map(|m| format!("add_subdirectory(${{CMAKE_SOURCE_DIR}}/modules/{m})"))
                    .collect::<Vec<_>>()
                    .join("\n")
            );
            modified = modified.replace(config.cmake_add_subdirectories_flag, &subdirectories);
        }else{
            modified = modified.replace(config.cmake_add_subdirectories_flag, "");
        }

        Ok(modified)
    }
    pub fn generate_to_file(self: &Self, config : &Config, file_path : PathBuf, pattern : &GeneratePattern)
        -> Result<()>{
        let contents = self.generate_to_string(config, pattern)?;
        fs::write(file_path.join("CMakeLists.txt"), contents)?;
        Ok(())
    }
}
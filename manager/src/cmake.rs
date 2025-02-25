use std::{fs, path::PathBuf};

use tokio::io::copy_bidirectional_with_sizes;

use crate::config::{Config, ConfigFile, Module};
use crate::Result;

/*pub struct CMakeTemplate{
    path_to_template : PathBuf,
}*/

pub struct GeneratePattern<'a>{
    pub project_name : &'a str,
    pub add_command : &'a str,
    pub include_paths : Option<&'a Vec<String>>,
    pub link_modules : Option<&'a Vec<String>>,
    pub subdirectories : Option<&'a  Vec<String>>,
    pub sources_path : Option<&'a str>,
    pub recursive_glob : Option<bool>,
    pub cmp_def : Option<Vec<String>>
}
fn get_path_to_template_cmakelists(config : &Config) -> PathBuf{
    config.asharis_root.join("resources").join("TemplateCMakeLists.txt")
}

//impl CMakeTemplate{
//    pub fn new(config : &Config) -> CMakeTemplate{
//        CMakeTemplate{
//            path_to_template : config.asharis_root.join("resources").join("TemplateCMakeLists.txt")
//        }
//    }
    pub fn generate_to_file_from_path(config : &Config, path_to_dir : &PathBuf) -> Result<()>{
        fs::write(path_to_dir.join("CMakeLists.txt"), generate_to_string_from_path(config, path_to_dir)?)?;
        Ok(())
    }

    pub fn generate_to_string_from_path(config : &Config, path_to_dir : &PathBuf) -> Result<String>{
        let config_file = ConfigFile::new_from_path(config, &path_to_dir.join("config.yaml"))?;
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
        let cmake_file_string = generate_to_string(config,
            &GeneratePattern{   
                project_name : &config_file.project,
                add_command : config_file.builds.get(&config_file.project).ok_or("library not in builds")?.as_str(),
                include_paths: Some(&cmake_include_paths),
                link_modules : Some(&cmake_link_modules),
                subdirectories : Some(&cmake_modules),
                sources_path : None,
                recursive_glob : None,
                cmp_def : config_file.cmp_defs.clone()
           })?;
       Ok(cmake_file_string)

    }

    pub fn generate_to_string(config : &Config,  pattern : &GeneratePattern)
         -> Result<String>{
        let source = fs::read_to_string(get_path_to_template_cmakelists(config))?;

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

        if let Some(cmp_defs) = &pattern.cmp_def{
            modified = modified.replace(config.cmake_compile_definitions_flag, cmp_defs.join(" ").as_str());
        }else{
            modified = modified.replace(config.cmake_compile_definitions_flag, "");
        }

        Ok(modified)
    }
    pub fn generate_to_file(config : &Config, file_path : PathBuf, pattern : &GeneratePattern)
        -> Result<()>{
        let contents = generate_to_string(config, pattern)?;
        fs::write(file_path.join("CMakeLists.txt"), contents)?;
        Ok(())
    }
use std::ops::Deref;
use std::{fs, path::PathBuf};

use crate::args::BinaryType;
use crate::config::{Config, ConfigFile, Module, ProjectConfig};
use crate::Result;


pub struct GeneratePattern<'a>{
    pub project_name : &'a str,
    pub add_command : BinaryType,
    pub include_paths : Option<&'a Vec<String>>,
    pub link_modules : Option<&'a Vec<String>>,
    pub subdirectories : Option<&'a  Vec<String>>,
    pub sources_path : Option<&'a str>,
    pub recursive_glob : Option<bool>,
    pub cmp_def : Option<Vec<String>>
}
fn get_path_to_template_cmakelists(config : &ProjectConfig) -> PathBuf{
    config.asharis_root.join("resources").join("TemplateCMakeLists.txt")
}

//impl CMakeTemplate{
//    pub fn new(config : &Config) -> CMakeTemplate{
//        CMakeTemplate{
//            path_to_template : config.asharis_root.join("resources").join("TemplateCMakeLists.txt")
//        }
//    }
    pub fn generate_to_file_from_path(config : &ProjectConfig, path_to_dir : &PathBuf) -> Result<()>{
        fs::write(path_to_dir.join("CMakeLists.txt"), generate_to_string_from_path(config, path_to_dir)?)?;
        Ok(())
    }

    pub fn generate_to_string_from_path(config : &ProjectConfig, path_to_dir : &PathBuf) -> Result<String>{
        let config_file = ConfigFile::new_from_path(&config.project_paths, &path_to_dir.join("config.yaml"))?;
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
        let (cmake_modules,  cmake_link_modules) = {
            let all_cmake_mods = config_file.get_all_cmake_modules(&config.project_paths.root)?;
            if path_to_dir.deref() == config.project_paths.root{
                let cmake_modules = all_cmake_mods.iter()
                    .map(|m| m.module_name.clone()).collect::<Vec<_>>();
                if let Some(modules) = config_file.modules{
                    let cmake_link_modules = all_cmake_mods.iter()
                        .filter(|module| modules.contains_key(&module.module_name)).map(|m| m.project_name.clone()).collect::<Vec<_>>();
                    (cmake_modules, cmake_link_modules)
                }else{
                    (cmake_modules, Vec::new())
                }
            }else{
                if let Some(modules) = config_file.modules{
                    let cmake_modules= Vec::new();
                    //let cmake_modules = all_cmake_mods.iter()
                      //  .filter(|module| modules.contains_key(&module.module_name) ).map(|m| m.module_name.clone()).collect::<Vec<_>>();
                    let cmake_link_modules = all_cmake_mods.iter()
                        .filter(|module| modules.contains_key(&module.module_name)).map(|m| m.project_name.clone()).collect::<Vec<_>>();
                    (cmake_modules, cmake_link_modules)
                }else{
                    (Vec::new(), Vec::new())
                }
            }
        };
        let cmake_file_string = generate_to_string(config,
            &GeneratePattern{   
                project_name : &config_file.project,
                add_command : config_file.targets.get(&config_file.project).ok_or("library not in builds")?.clone(),
                include_paths: Some(&cmake_include_paths),
                link_modules : Some(&cmake_link_modules),
                subdirectories : Some(&cmake_modules),
                sources_path : None,
                recursive_glob : None,
                cmp_def : config_file.cmp_defs.clone()
           })?;
       Ok(cmake_file_string)

    }

    pub fn generate_to_string(config : &ProjectConfig,  pattern : &GeneratePattern)
         -> Result<String>{
        let source = fs::read_to_string(get_path_to_template_cmakelists(config))?;

        let mut modified = source
            .replace(&config.flags.project_name, pattern.project_name);
        
        let add_command = match pattern.add_command{
            BinaryType::StaticLibrary => {
                format!("add_library({} STATIC)", pattern.project_name)
            }
            BinaryType::DynamicLibrary => {
                format!("add_library({} DYNAMIC)", pattern.project_name)
            }
            BinaryType::Executable => {
                format!("add_executable({})", pattern.project_name)
            }
        };
        modified = modified.replace(config.flags.cmake.add_command, add_command.as_str());

        if let Some(include_paths) = pattern.include_paths{
            let include_paths = include_paths.iter()
                .map(|path| format!("\t${{CMAKE_SOURCE_DIR}}/modules/{path}"))
                .collect::<Vec<_>>()
                .join("\n");
            modified = modified.replace(config.flags.cmake.modules_include_paths, &include_paths);
        }else{
            modified = modified.replace(config.flags.cmake.modules_include_paths, "");
        }
        if let Some(link_modules) = pattern.link_modules{
            let link_modules = link_modules.join(" ");
            modified = modified.replace(config.flags.cmake.link_modules, &link_modules);
        }else{
            modified = modified.replace(config.flags.cmake.link_modules, "");
        }
        if let Some(sources_path) = pattern.sources_path{
            modified = modified.replace(config.flags.cmake.sources_path, sources_path);
        }else{
            modified = modified.replace(config.flags.cmake.sources_path, "src");
        }
        if matches!(pattern.recursive_glob, None | Some(true)){
            modified = modified.replace(config.flags.cmake.glob_type, "GLOB_RECURSE");
        }else{
            modified = modified.replace(config.flags.cmake.glob_type, "GLOB");
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
            modified = modified.replace(config.flags.cmake.add_subdirectories, &subdirectories);
        }else{
            modified = modified.replace(config.flags.cmake.add_subdirectories, "");
        }

        if let Some(cmp_defs) = &pattern.cmp_def{
            modified = modified.replace(config.flags.cmake.compile_definitions, cmp_defs.join(" ").as_str());
        }else{
            modified = modified.replace(config.flags.cmake.compile_definitions, "");
        }

        Ok(modified)
    }
    pub fn generate_to_file(config : &ProjectConfig, file_path : PathBuf, pattern : &GeneratePattern)
        -> Result<()>{
        let contents = generate_to_string(config, pattern)?;
        fs::write(file_path.join("CMakeLists.txt"), contents)?;
        Ok(())
    }
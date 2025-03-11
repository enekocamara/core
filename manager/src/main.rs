use std::{fs, sync::Arc};

mod error;

pub use self::error::{Error,Result};

mod args;
mod spinner;
mod tasks;
mod config;
mod utils;
mod modules;
mod cmake;
mod syris;


use args::{Cli, SyrisCommands, TargetCommands};
use colored::Colorize;
use scopeguard::defer;
use spinner::Spinner;
use config::{Config, ConfigFile};
use syris::new_syris_project;
use modules::{add_module, get_available_modules, remove_module};
use clap::Parser;
use indicatif::MultiProgress;

#[tokio::main]
async fn main(){
    if let Err(err) = try_main().await{
        println!("{err:?}");
        std::process::exit(1);
    }
}

async fn try_main() -> Result<()>{
    let cli = Cli::parse();
    let config : Config = Config::new()?;
    let multi = Arc::new(MultiProgress::new());
    match cli.command{
        args::Commands::New(project) => {
            if let Err(e) = tasks::new_project(config, project, multi).await{
                Err(format!("Failed to create new project: {e}"))?;
            };
        }
        args::Commands::Init => {
            if let Err(e) = tasks::init_project(config,  multi).await{
                Err(format!("Failed to create new project: {e}"))?;
            };
        }
        args::Commands::Build(options) => {
            let config_file = ConfigFile::new_from_file(&config)?;
            if let Err(e) = tasks::build_project(&config, &options, multi, &config_file, tasks::BuildProjectOpts::WriteOutputAlways){
                Err(format!("Failed to build: {e}"))?;
            };
        }
        args::Commands::Run(options) => {
            if let Err(e) = tasks::run_project(&config, &options, multi){
                Err(format!("Failed to run project: {e}"))?;
            };
        }
        args::Commands::CmakeBuild =>{
            tasks::build_cmake_project(&config, multi)?;
        }
        args::Commands::Clean =>{
            let config_file = ConfigFile::new_from_file(&config)?;
            if let Err(e) = tasks::clean_project(&config,&config_file, multi){
                println!("{} to clean {}: Reason:\n\t{}", "Failed".red(), config_file.project.blue(), e);
            }else{
                println!("{} {}", "Cleaned".green(), config_file.project.blue());
            }
        }
        args::Commands::Module(module_command) => match module_command{
            args::ModuleCommands::List => {
                let config_file = ConfigFile::new_from_file(&config)?;
                if let Some(modules) = &config_file.modules{
                    for name in modules.keys(){
                        println!("  * {}", name.blue());
                    }
                }
            }
            args::ModuleCommands::ListAll => {
                let modules = config.get_all_installed_modules()?;
                for name in modules{
                    println!("  * {}", name.blue());
                }
            }
            args::ModuleCommands::ListAvailable => {
                let available_modules = get_available_modules(&config)?;
                for name in available_modules.keys(){
                    println!("  * {}", name.blue());
                }
            }
            args::ModuleCommands::Add(module) => {
                if let Err(e) = add_module(&config, &module, multi.clone()){
                    Err(format!("Failed to add module {}: {e}", module.name))?;
                }
                let _ = tasks::build_cmake_project(&config, multi);
            }
            args::ModuleCommands::Remove(module) => {
                let mut config_file = ConfigFile::new_from_file(&config)
                    .map_err(|e| format!("Failed to remove module {}: {e}", module.name.blue()))?;
                if let Err(e) = remove_module(&config, &module.name, multi, &mut config_file){
                    Err(format!("Failed to remove module {}:{e}", module.name.blue()))?;
                }
                println!("{} module {} removed.", "Success".green(), module.name.blue());
            }
            args::ModuleCommands::RemoveAll => {
                let mut config_file = ConfigFile::new_from_file(&config)
                    .map_err(|e| format!("Failed to remove modules: {e}"))?;
                let mut removed_modules =  Vec::new();
                if let Some(modules) = config_file.modules.clone(){
                    for (name, _) in &modules{
                        remove_module(&config, name, multi.clone(), &mut config_file)?;
                        removed_modules.push(name.clone());
                    }
                    println!("{} modules [{}] removed.", "Success".green(), removed_modules.iter().map(|module_name| module_name.blue().to_string()).collect::<Vec<String>>().join(", "));
                }else{
                    println!("No modules to be removed");
                }
            }
        }
        args::Commands::Target(target) => match &target{
            TargetCommands::Add(target)=>{
               tasks::add_target(&config, target)?;
            },
            TargetCommands::Remove(target)=>{
                todo!();
            },
            TargetCommands::RemoveAll=>{
                todo!()
            }
            TargetCommands::Current { name } => {
                let mut config_file = ConfigFile::new_from_file(&config)?;
                if config_file.current_target == *name{
                    println!("{} is already the current target", name.blue());

                }else if config_file.targets.contains_key(name){
                    config_file.current_target = name.clone();
                    config_file.write()?;
                    println!("{} {} is now the current target", "Success".green(), name.blue());
                }else{
                    Err(format!("Target {} not found in targets.\n\tAvailable targets: [{}]", name.blue(),
                        config_file.targets.iter().map(|(name, _)| name.blue().to_ascii_lowercase()).collect::<Vec<_>>().join(",")))?
                }
            }
        }
        args::Commands::Update => {
            if let Err(e) = cmake::generate_to_file_from_path(&config, &config.cmd_path) {
                Err(format!("Failed to create config file {e}"))?;
            }
        }
        args::Commands::Syris(syris_command) => match syris_command{
            SyrisCommands::New(new_command)=>{
                let spiner  = Spinner::new("creating project", Some(multi.clone()));
                defer!(spiner.finish(););
                new_syris_project(config, new_command, multi.clone()).await?;
            },
            SyrisCommands::Build(_) => {
                tasks::build_cmake_project(&config, multi.clone())?;
            },
        }
        args::Commands::Cmd { module_name, cmd_name , args} => {
            let config_file = ConfigFile::new_from_file(&config)?;
            if let Some(module_config_file) = config_file.get_config_file(&module_name){
                if let Some(commands) = &module_config_file.command{
                    if let Some(command) = commands.get(&cmd_name){
                        command.run(&config, &module_config_file.get_dir_path(), args)
                            .map_err(|e| format!("Failed to run command {cmd_name}: {e}"))?
                    } else {
                        Err(format!("Project {} has no command named {}", module_name.blue(), cmd_name))?
                    }
                }else{
                    Err(format!("Project {} has no commands", module_name.blue()))?
                }
            }else{
                Err(format!("Project {} not found", module_name.blue()))?
            }
        }
        args::Commands::Test => {
            todo!();
            let config_file = ConfigFile::new_from_file(&config)?;
            let testing_dir = config.project_paths.root.join("Testing");
            if !fs::exists(&testing_dir)?{
                fs::create_dir(&testing_dir)?;
            }
            let _ = std::process::Command::new("ctest")
                .args(["-C", "Debug", "--test-dir"])
                .current_dir(&config.project_paths.build)
                .status()?;
            let log_file = config.project_paths.build.join("Testing").join("Temporary").join("LastTest.log");
            let log_file_dst = config.project_paths.root.join("Testing").join("LastTest.log");
            if !fs::exists(&log_file)?{
                Err(format!("Log file was not generated at: {:?}", log_file))?
            }
            fs::copy(log_file, log_file_dst)?;
        }
        args::Commands::TestAll => {
            let testing_dir = config.project_paths.root.join("Testing");
            if !fs::exists(&testing_dir)?{
                fs::create_dir(&testing_dir)?;
            }
            let _ = std::process::Command::new("ctest")
                .args(["-C", "Debug"])
                .current_dir(&config.project_paths.build)
                .status()?;
            let log_file = config.project_paths.build.join("Testing").join("Temporary").join("LastTest.log");
            let log_file_dst = config.project_paths.root.join("Testing").join("LastTest.log");
            if !fs::exists(&log_file)?{
                Err(format!("Log file was not generated at: {:?}", log_file))?
            }
            fs::copy(log_file, log_file_dst)?;
        }
    }
    Ok(())
}

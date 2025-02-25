use std::{sync::Arc, time::Instant};

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


use args::{BuildProject, Cli, Commands, RemoveModule, SyrisCommands};
use colored::Colorize;
use scopeguard::defer;
use spinner::Spinner;
use config::{Config, ConfigFile};
use tasks::{build_cmake_project, build_project, clean_project, init_project, new_project, run_project, BuildProjectOpts};
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
        Commands::New(project) => {
            if let Err(e) = new_project(config, project, multi).await{
                Err(format!("Failed to create new project: {e}"))?;
            };
        }
        Commands::Init => {
            if let Err(e) = init_project(config,  multi).await{
                Err(format!("Failed to create new project: {e}"))?;
            };
        }
        Commands::Build(build) => {
            let config_file = ConfigFile::new_from_file(&config)?;
            let start = Instant::now();
            if let Err(e) = build_project(&config, build.clone(), multi, &config_file, BuildProjectOpts::WriteOutputAlways){
                Err(format!("Failed to build: {e}"))?;
            };
            let elapsed = start.elapsed();
            //println!("{}: {} built [target {:#?}] in {:#?}", "Success".green(), config_file.project, build.target, elapsed);
        }
        Commands::Run(run) => {
            if let Err(e) = run_project(&config, run, multi){
                Err(format!("Failed to run project: {e}"))?;
            };
        }
        Commands::CmakeBuild =>{
            build_cmake_project(&config, multi)?;
        }
        Commands::Clean =>{
            let config_file = ConfigFile::new_from_file(&config)?;
            if let Err(e) = clean_project(&config,&config_file, multi){
                println!("{} to clean {}: Reason:\n\t{}", "Failed".red(), config_file.project.blue(), e);
            }else{
                println!("{} {}", "Cleaned".green(), config_file.project.blue());
            }
        }
        Commands::List => {
            let config_file = ConfigFile::new_from_file(&config)?;
            if let Some(modules) = &config_file.modules{
                for name in modules.keys(){
                    println!("  * {}", name.blue());
                }
            }
        }
        Commands::ListInstalled => {
            let modules = config.get_all_installed_modules()?;
            for name in modules{
                println!("  * {}", name.blue());
            }
        }
        Commands::ListAvailable => {
            let available_modules = get_available_modules(&config)?;
            for name in available_modules.keys(){
                println!("  * {}", name.blue());
            }
        }
        Commands::Update => {
            if let Err(e) = cmake::generate_to_file_from_path(&config, &config.project_paths.root) {
                Err(format!("Failed to create config file {e}"))?;
            }
        }
        Commands::Add(module) => {
            if let Err(e) = add_module(&config, &module, multi.clone()){
                Err(format!("Failed to add module {}: {e}", module.name))?;
            }
            let _ = build_cmake_project(&config, multi);
        }
        Commands::Remove(module)=>{
            let mut config_file = ConfigFile::new_from_file(&config)
                .map_err(|e| format!("Failed to remove modules: {e}"))?;
            match &module{
                RemoveModule::Name { name } => {
                    if let Err(e) = remove_module(&config, name, multi, &mut config_file){
                        Err(format!("Failed to remove module {}:{e}", name))?;
                    }
                    println!("{} module {} removed.", "Success".green(), name.blue());
                }
                RemoveModule::All => {
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
        }
        Commands::Syris(syris_command) => match syris_command{
            SyrisCommands::New(new_command)=>{
                let spiner  = Spinner::new("creating project", Some(multi.clone()));
                defer!(spiner.finish(););
                new_syris_project(config, new_command, multi.clone()).await?;
            },
            SyrisCommands::Build(_) => {
                build_cmake_project(&config, multi.clone())?;
            },
        }
    }
    Ok(())
}

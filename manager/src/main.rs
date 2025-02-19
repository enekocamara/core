use std::path::PathBuf;
use std::fs;
use std::process::Command;
use std::sync::Arc;

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


use args::{Cli, Commands, RemoveModule, SyrisCommands};
use scopeguard::defer;
use spinner::Spinner;
use config::{Config, ConfigFile};
use tasks::{generate_cmake_from_conf, new_project, build_project, run_project, build_cmake_project, clean_project, init_project};
use syris::new_syris_project;
use modules::{add_module, get_available_modules, remove_module};
use clap::Parser;

use indicatif::MultiProgress;

#[tokio::main]
async fn main() -> Result<()>{
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
            if let Err(e) = build_project(&config, build, multi){
                Err(format!("Failed to build: {e}"))?;
            };
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
            clean_project(&config, multi)?;
        }
        Commands::List => {
            let config_file = ConfigFile::new_from_file(&config)?;
            if let Some(modules) = &config_file.modules{
                for name in modules.keys(){
                    println!("{name}");
                }
            }
        }
        Commands::ListInstalled => {
            let modules = config.get_all_installed_modules()?;
            for name in modules{
                println!("{name}");
            }
        }
        Commands::ListAvailable => {
            let available_modules = get_available_modules(&config)?;
            for name in available_modules.keys(){
                println!("{name}");
            }
        }
        Commands::Update => {
            if let Err(e) = generate_cmake_from_conf(&config) {
                Err(format!("Failed to create config file {e}"))?;
            }
        }
        Commands::Add(module) => {
            if let Err(e) = add_module(config, &module, multi){
                Err(format!("Failed to add module {}: {e}", module.name))?;
            }
        }
        Commands::Remove(module)=>{
            let mut config_file = ConfigFile::new_from_file(&config).map_err(|e| format!("Failed to remove modules: {e}"))?;
            match &module{
                RemoveModule::Name { name } => {
                    if let Err(e) = remove_module(&config, name, multi, &mut config_file){
                        Err(format!("Failed to remove module {}:{e}", name))?;
                    }
                }
                RemoveModule::All => {
                    if let Some(modules) = config_file.modules.clone(){
                        for (name, _) in &modules{
                            remove_module(&config, name, multi.clone(), &mut config_file)?;
                        }
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

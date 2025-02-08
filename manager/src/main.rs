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


use args::{Cli, Commands, NewProject,NewSyrisProject, SyrisCommands};
use scopeguard::defer;
use spinner::Spinner;
use config::Config;
use tasks::{generate_cmake_from_conf, init_entry_point, init_source, init_syris, init_vs_conf, pip_glad_install, init_syris_source, add_library_to_config, remove_library};

use clap::Parser;

use indicatif::MultiProgress;


async fn new_project(mut config : Config, project : NewProject, multi : Arc<MultiProgress>) -> Result<()>{
    let project_spinner = Spinner::new(format!("Creating project {}...", &project.name), Some(multi.clone()));
    let tasks_spinner = Spinner::new("Creating project dir...", Some(multi.clone()));
    config.project_root = config.project_root.join(PathBuf::from(&project.name));
    if config.project_root.exists(){
        Err("Error already exists")?;
    }
    fs::create_dir(&config.project_root)?;
    tasks_spinner.change_message("creating src dir...");
    let src_folder = config.project_root.join("src");
    fs::create_dir(&src_folder)?;
    
    tasks_spinner.change_message("initializing git...");
    let output = Command::new("git")
        .arg("init")
        .current_dir(&config.project_root)
        .output()?;
    if !output.status.success(){
        Err("Failed to initialize git")?;
    }
    tasks_spinner.change_message("adding gitignore...");
    let gitignore_src = config.asharis_root.join("resources").join(".gitignore");
    let gitignore_dst = config.project_root.join(".gitignore");
    fs::copy(gitignore_src, gitignore_dst)?;

    init_source(config.clone(), project.clone(), multi.clone()).await?;

    tasks_spinner.finish();
    //MAIN CMAKELISTS
    generate_cmake_from_conf(config.clone()).await.map_err(|e| format!("Failed to generate cmake: {e}"))?;

    build_project(&config, multi.clone()).await?;
    project_spinner.finish();
    Ok(())
}

async fn new_syris_project(mut config : Config, project : NewSyrisProject, multi : Arc<MultiProgress>) -> Result<()>{
    let spinner = Spinner::new("Creating directory...", Some(multi.clone()));
    config.project_root = config.project_root.join(PathBuf::from(&project.name));
    if config.project_root.exists(){
        Err("Error already exists")?;
    }
    fs::create_dir(&config.project_root)?;
    spinner.change_message("creating src dir...");
    let src_folder = config.project_root.join("src");
    fs::create_dir(&src_folder)?;
    
    spinner.change_message("initializing git...");
    let output = Command::new("git")
        .arg("init")
        .current_dir(&config.project_root)
        .output()?;
    if !output.status.success(){
        Err("failed to initialize git")?;
    }

    let entry_point = tokio::spawn(init_entry_point(config.clone(), project.clone(), multi.clone()));
    let source = tokio::spawn(init_syris_source(config.clone(), project.clone(), multi.clone()));
    let syris = tokio::spawn(init_syris(config.clone(), project.clone(), multi.clone()));
    let vs_conf = tokio::spawn(init_vs_conf(config.clone(), project.clone(), multi.clone()));

    let mut failed : bool = false;
    let mut errors : Vec<Error>  = Vec::new();

    let _= entry_point.await.map_err(|e| {
        failed = true;
        errors.push(format!("Failed to join async entry point task: {e}").into());
    }).and_then(|r| r.map_err(|e| {
        failed = true;
        errors.push(e);
    }));

    let _ = source.await.map_err(|e| {
        failed = true;
        errors.push(format!("Failed to join async source task: {e}").into());
    }).and_then(|r| r.map_err(|e| {
        failed = true;
        errors.push(e);
    }));

    let _ = syris.await.map_err(|e| {
        failed = true;
        errors.push(format!("Failed to join async syris task: {e}").into());
    }).and_then(|r| r.map_err(|e| {
        failed = true;
        errors.push(e);
    }));

    let _ = vs_conf.await.map_err(|e| {
        failed = true;
        errors.push(format!("Failed to join async vs conf: {e}").into());
    }).and_then(|r| r.map_err(|e| {
        failed = true;
        errors.push(e);
    }));

    if failed{
        let mut acc_err  = String::from("Errors generating project:\n");
        errors.into_iter().fold(&mut acc_err, |acc_err , error| {
            acc_err.push_str(&format!("\t{error}\n"));
            acc_err
        });
        Err(acc_err)?;
    }
    //MAIN CMAKELISTS
    spinner.change_message("setting main cmakelists...");
    let main_cmakelists_file_src = config.asharis_root.join("resources").join("MainCMakeLists.txt");
    let main_cmakelists_file_dst = config.project_root.join("CMakeLists.txt");

    let contents = fs::read_to_string(main_cmakelists_file_src)?;

    let modified_content = contents.replace(&config.project_name_flag, &project.name);

    fs::write(main_cmakelists_file_dst, modified_content)?;

    spinner.finish();
    pip_glad_install(&config, multi.clone()).await?;
    build_project(&config, multi.clone()).await?;
    Ok(())
}

async fn build_project(config : &Config, multi : Arc<MultiProgress>) -> Result<()>{
    let spinner = Spinner::new("Generating cmake build files", Some(multi));
    defer!(spinner.finish(););
    if !fs::exists(config.project_root.join("build"))?{
        fs::create_dir(config.project_root.join("build"))?;
    }
    
    let output = Command::new("cmake")
        .arg("..")
        .current_dir(&config.project_root.join("build"))
        .output()?;
    if !output.status.success(){
        Err(format!("failed to to build cmake project, {}", String::from_utf8_lossy(&output.stderr)))?;

    }
    Ok(())
}



#[tokio::main]
async fn main() -> Result<()>{
    let cli = Cli::parse();
    let config : Config = Config::new()?;
    let multi = Arc::new(MultiProgress::new());
    match cli.command{
        Commands::New(project) => {
            if let Err(e) = new_project(config.clone(), project, multi).await{
                Err(format!("Failed to create new project: {e}"))?;
            };
        }
        Commands::Update => {
            if let Err(e) = generate_cmake_from_conf(config).await {
                Err(format!("Failed to create config file {e}"))?;
            }
        }
        Commands::Add(library) => {
            if let Err(e) = add_library_to_config(config, library, multi).await{
                Err(format!("Failed to add library: {e}"))?;
            }
        }
        Commands::Remove(library)=>{
            if let Err(e) = remove_library(config, library, multi).await{
                Err(format!("Failed to remove library {e}"))?;
            }
        }
        Commands::Syris(syris_command) => match syris_command{
            SyrisCommands::New(new_command)=>{
                let spiner  = Spinner::new("creating project", Some(multi.clone()));
                new_syris_project(config, new_command, multi.clone()).await.unwrap();
                spiner.finish();
            },
            SyrisCommands::Build(_) => {
                build_project(&config, multi.clone()).await;
            },
        }
    }
    Ok(())
}

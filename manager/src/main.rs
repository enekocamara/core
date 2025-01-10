use std::env;
use std::path::{PathBuf,Path};
use std::fs;
use clap::Parser;
use std::process::{Command, Output};

mod args;
use args::{Cli, Commands, NewProject};

fn new_project(project : NewProject){
    match env::current_dir(){
        Ok(path) => {
            let project_root : PathBuf = path.join(PathBuf::from(project.name));
            let asharis_root : PathBuf = {
                match env::var("ASHARIS_ROOT"){
                    Ok(root)=> PathBuf::from(root),
                    Err(e)=>{
                        eprintln!("failed to find ASHARIS_ROOT in environment variables: {}",e);
                        return;
                    }
                }
            };
            println!("project root will be: {:?}", project_root);
            if project_root.exists(){
                println!("Error already exists");
                return;
            }
            if let Err(_) = fs::create_dir(&project_root){
                println!("failed to create folder");
                return;
            }
            
            let output = Command::new("git")
                .arg("init")
                .current_dir(&project_root)
                .output()
                .unwrap();
            if !output.status.success(){
                eprintln!("failed to initialize git");
                return;
            }
            let syris_url = "https://github.com/enekocamara/Syris";
            let syris_folder = project_root.join("vendor").join("syris");
            println!("syris folder: '{:?}'", syris_folder);
            let output = Command::new("git")
                .args(["submodule", "add", syris_url, "vendor/syris"])
                .current_dir(&project_root)
                .output()
                .unwrap();
            if !output.status.success(){
                eprintln!("failed to add submodule {:?}", output.stderr);
                return;
            }
            let src_folder = project_root.join("src");
            if let Err(_) = fs::create_dir(&src_folder){
                println!("failed to create folder");
                return;
            }
        },
        Err(e) => {
            println!("environment variable pwd not set: {}", e);
        }
    }
}

fn main() {
    let cli = Cli::parse();
    match cli.command{
        Commands::New(value)=>{
            new_project(value);
        }
    }
}

use std::{default, env, path};
use std::path::{PathBuf,Path};
use std::fs;
use std::process::{Command, Output};
use std::sync::Arc;

mod args;
mod spinner;
mod tasks;
mod config;
mod utils;

use args::{Cli, Commands, NewProject};
use spinner::Spinner;
use config::Config;
use tasks::{init_entry_point, init_source, init_syris, init_vs_conf, pip_glad_install};

use clap::Parser;

use fs_extra::dir::{self, create, CopyOptions};
use fs_extra::error::{Error,ErrorKind};

use indicatif::{MultiProgress, ProgressBar, ProgressStyle};
use tokio::time::{sleep, Duration};



async fn new_project(mut config : Config, project : NewProject, multi : Arc<MultiProgress>) -> Result<(), String>{
    let spinner = Spinner::new("Creating directory...", Some(multi.clone()));
    config.project_root = config.project_root.join(PathBuf::from(&project.name));
    if config.project_root.exists(){
        return Err("Error already exists".to_string());
    }
    if let Err(_) = fs::create_dir(&config.project_root){
        return Err("failed to create folder".to_string());
        
    }
    spinner.change_message("creating src dir...");
    let src_folder = config.project_root.join("src");
    if let Err(_) = fs::create_dir(&src_folder){
        return Err("failed to create folder".to_string());
    }
    
    spinner.change_message("initializing git...");
    let output = Command::new("git")
        .arg("init")
        .current_dir(&config.project_root)
        .output()
        .unwrap();
    if !output.status.success(){
        return Err("failed to initialize git".to_string());
    }

    let entry_point = tokio::spawn(init_entry_point(config.clone(), project.clone(), multi.clone()));
    let source = tokio::spawn(init_source(config.clone(), project.clone(), multi.clone()));
    let syris = tokio::spawn(init_syris(config.clone(), project.clone(), multi.clone()));
    let vs_conf = tokio::spawn(init_vs_conf(config.clone(), project.clone(), multi.clone()));

    let mut failed : bool = false;
    let mut errors : Vec<String> = Vec::new();

    let _= entry_point.await.map_err(|e| {
        failed = true;
        errors.push(format!("Failed to join async entry point task: {e}"));
    }).and_then(|r| r.map_err(|e| {
        failed = true;
        errors.push(e);
    }));

    let _ = source.await.map_err(|e| {
        failed = true;
        errors.push(format!("Failed to join async source task: {e}"));
    }).and_then(|r| r.map_err(|e| {
        failed = true;
        errors.push(e);
    }));

    let _ = syris.await.map_err(|e| {
        failed = true;
        errors.push(format!("Failed to join async syris task: {e}"));
    }).and_then(|r| r.map_err(|e| {
        failed = true;
        errors.push(e);
    }));

    let _ = vs_conf.await.map_err(|e| {
        failed = true;
        errors.push(format!("Failed to join async vs conf: {e}"));
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
        return Err(acc_err);
    }
    /*
    spinner.change_message("adding submodule syris...");
    let syris_url = "https://github.com/enekocamara/Syris";
    let syris_folder = config.project_root.join("vendor").join("syris");
    let output = Command::new("git")
        .args(["submodule", "add", syris_url, "vendor/syris"])
        .current_dir(&config.project_root)
        .output()
        .unwrap();
    if !output.status.success(){
        eprintln!("failed to add submodule {:?}", output.stderr);
        return;
    }

    spinner.change_message("initializing submodules recursive...");
    let output = Command::new("git")
        .args(["submodule", "update", "--init", "--recursive"])
        .current_dir(&syris_folder)
        .output()
        .expect("Failed to update submodules");

    if !output.status.success(){
        eprintln!("failed to add submodule {:?}", output.stderr);
        return;
    }

    spinner.change_message("creating src dir...");
    let src_folder = config.project_root.join("src");
    if let Err(_) = fs::create_dir(&src_folder){
        println!("failed to create folder");
        return;
    }

    let project_name_flag = "%PROJECT_NAME%";
    */
    //ENTRYPOINT
    /*/
    spinner.change_message("setting entrypoint...");
    let entry_point_src = config.asharis_root.join("resources").join("EntryPoint").join("src");
    copy_dir_rec(&entry_point_src, &config.project_root.join("EntryPoint")).unwrap();
    let entry_point_cmakelists_file_src = config.asharis_root.join("resources").join("EntryPoint").join("CMakeLists.txt");
    let entry_point_cmakelists_file_dst = config.project_root.join("EntryPoint").join("CMakeLists.txt");

    let contents = fs::read_to_string(entry_point_cmakelists_file_src).unwrap();

    let modified_content = contents.replace(project_name_flag, &project.name);

    fs::write(entry_point_cmakelists_file_dst, modified_content).unwrap();

    */
    //VS CONF
    /*
    spinner.change_message("setting vs conf...");
    let vs_conf_src = config.asharis_root.join(".vscode");
    copy_dir_rec(&vs_conf_src, &config.project_root).unwrap();
    */
    //MAIN CMAKELISTS
    spinner.change_message("setting main cmakelists...");
    let main_cmakelists_file_src = config.asharis_root.join("resources").join("MainCMakeLists.txt");
    let main_cmakelists_file_dst = config.project_root.join("CMakeLists.txt");

    let contents = fs::read_to_string(main_cmakelists_file_src).unwrap();

    let modified_content = contents.replace(&config.project_name_flag, &project.name);

    fs::write(main_cmakelists_file_dst, modified_content).unwrap();

    //SRC
    /*
    spinner.change_message("setting src contents...");
    fs::create_dir(config.project_root.join("src").join(&project.name)).unwrap();
    let project_cmakelists_file_src = config.asharis_root.join("resources").join("Template").join("TemplateCMakeLists.txt");
    let project_cmakelists_file_dst = config.project_root.join("src").join("CMakeLists.txt");
    let contents = fs::read_to_string(project_cmakelists_file_src).unwrap();

    let modified_content = contents.replace(project_name_flag, &project.name);
    fs::write(project_cmakelists_file_dst, modified_content).unwrap();

    let project_name_first_upper_flag = "%PROJECT_NAME_FIRST_UPPER%";
    let project_name_first_upper = first_uppercase(&project.name);

    let template_hpp_file_src = config.asharis_root.join("resources").join("Template").join("Template.hpp");
    let template_hpp_file_dst = config.project_root.join("src").join(&project.name).join(format!("{}App.hpp", &project_name_first_upper));
    let contents = fs::read_to_string(template_hpp_file_src).unwrap();

    let modified_content = contents.replace(project_name_first_upper_flag, &project_name_first_upper);
    let modified_content = modified_content.replace(project_name_flag, &project.name);
//    println!("Path : {}App.hpp", &project_name_first_upper);
    fs::write(template_hpp_file_dst, modified_content).unwrap();

    let template_cpp_file_src = config.asharis_root.join("resources").join("Template").join("Template.cpp");
    let template_cpp_file_dst = config.project_root.join("src").join(&project.name).join(format!("{}App.cpp", &project_name_first_upper));
    let contents = fs::read_to_string(template_cpp_file_src).unwrap();

    let modified_content = contents.replace(project_name_first_upper_flag, &project_name_first_upper);
    let modified_content = modified_content.replace(project_name_flag, &project.name);
    fs::write(template_cpp_file_dst, modified_content).unwrap();

    */

    /*
    //GIT
    spinner.change_message("adding gitignore...");
    let gitignore_src = config.asharis_root.join(".gitignore");
    let gitignore_dst = config.project_root.join(".gitignore");
    fs::copy(gitignore_src, gitignore_dst).unwrap();

    //IMGUI CMAKE
    spinner.change_message("adding imgui cmakelists...");
    let imgui_src = config.asharis_root.join("resources").join("ImGuiCMakeLists.txt");
    let imgui_dst = config.project_root.join("vendor").join("syris").join("vendor").join("imgui_docking").join("CMakeLists.txt");
    
    fs::copy(imgui_src, imgui_dst).unwrap();

    //GLAD CMAKE
    spinner.change_message("adding glad cmakelists...");
    let glad_src = config.asharis_root.join("resources").join("GladCMakeLists.txt");
    let glad_dst = config.project_root.join("vendor").join("syris").join("vendor").join("glad").join("CMakeLists.txt");
    
    fs::copy(glad_src, glad_dst).unwrap();
    */
    spinner.finish();
    pip_glad_install(&config, multi.clone()).await?;
    build_project(&config, multi.clone()).await;
    Ok(())
}

async fn build_project(config : &Config, multi : Arc<MultiProgress>){
    let spinner = Spinner::new("Generating cmake build files", Some(multi));
    if !fs::exists(config.project_root.join("build")).unwrap(){
        fs::create_dir(config.project_root.join("build")).unwrap();
    }
    
    let output = Command::new("cmake")
        .arg("..")
        .current_dir(&config.project_root.join("build"))
        .output()
        .unwrap();
    if !output.status.success(){
        eprintln!("failed to to build cmake project, {}", String::from_utf8_lossy(&output.stderr));
        return;
    }
    spinner.finish();
}

#[tokio::main]
async fn main() {
    let cli = Cli::parse();
    let config : Config = Config::new().unwrap();
    let multi = Arc::new(MultiProgress::new());
    let spiner  = Spinner::new("creating project", Some(multi.clone()));
    match cli.command{
        Commands::New(new_command)=>{
            new_project(config, new_command, multi.clone()).await.unwrap();
        },
        Commands::Build(_) => {
            build_project(&config, multi.clone()).await;
        },
        Commands::Glad(_) => {
            pip_glad_install(&config, multi.clone()).await.unwrap();
        }
    }
    spiner.finish();
}

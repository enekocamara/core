use std::fs;
use std::process::Command;
use std::sync::Arc;
use indicatif::MultiProgress;
use std::path::PathBuf;


use crate::spinner::Spinner;
use crate::config::Config;
use crate::args::NewSyrisProject;
use crate::{Result,utils, Error};
use crate::tasks::{init_vs_conf, pip_glad_install, build_cmake_project};

pub async fn init_syris(config : Config, project : NewSyrisProject, multi : Arc<MultiProgress>) -> Result<()>{
    let spinner = Spinner::new("initializing syris",Some(multi));
    let syris_url = "https://github.com/enekocamara/Syris";
    let syris_folder = config.project_paths.modules.join("syris");
    //println!("syris folder: '{:?}'", syris_folder);
    let output = Command::new("git")
        .args(["submodule", "add", syris_url, "modules/syris"])
        .current_dir(&config.project_paths.root)
        .output()
        .map_err(|e| format!("failed to run command {}", e))?;
    if !output.status.success(){
        Err(format!("failed to add submodule {}", String::from_utf8_lossy(&output.stderr)))?;
    }

//    println!("Submodule updates...");
    spinner.change_message("initializing submodules recursive...");
    let output = Command::new("git")
        .args(["submodule", "update", "--init", "--recursive"])
        .current_dir(&syris_folder)
        .output()
        .map_err(|e| format!("failed to run command {}", e))?;

    if !output.status.success(){
        Err(format!("failed to add submodule {}", String::from_utf8_lossy(&output.stderr)))?;
    }

    //GIT
    spinner.change_message("adding gitignore...");
    let gitignore_src = config.asharis_root.join("resources").join(".gitignore");
    let gitignore_dst = config.project_paths.root.join(".gitignore");
    fs::copy(gitignore_src, gitignore_dst).map_err(|e| format!("failed to copy .gitignore: {}", e))?;

    //IMGUI CMAKE
    spinner.change_message("adding imgui cmakelists...");
    let imgui_src = config.asharis_root.join("resources").join("ImGuiCMakeLists.txt");
    let imgui_dst = config.project_paths.modules.join("syris").join("modules").join("imgui_docking").join("CMakeLists.txt");
    
    fs::copy(imgui_src, imgui_dst).map_err(|e| format!("failed to copy imgui cmakelists: {}", e))?;

    //GLAD CMAKE
    spinner.change_message("adding glad cmakelists...");
    let glad_src = config.asharis_root.join("resources").join("GladCMakeLists.txt");
    let glad_dst = config.project_paths.modules.join("syris").join("modules").join("glad").join("CMakeLists.txt");
    
    fs::copy(glad_src, glad_dst).map_err(|e| format!("failed to copy glad cmakelists: {}", e))?;

    spinner.finish();
    Ok(())
}

pub async fn init_entry_point(config : Config, project : NewSyrisProject, multi : Arc<MultiProgress>)->Result<()>{
    let spinner = Spinner::new("initializing entry point", Some(multi));
    spinner.change_message("setting entrypoint...");
    let entry_point_src = config.asharis_root.join("resources").join("EntryPoint").join("src");
    utils::copy_dir_rec(&entry_point_src, &config.project_paths.root.join("EntryPoint")).map_err(|e| format!("failed to copy dir entrypoint: {e}"))?;
    let entry_point_cmakelists_file_src = config.asharis_root.join("resources").join("EntryPoint").join("CMakeLists.txt");
    let entry_point_cmakelists_file_dst = config.project_paths.root.join("EntryPoint").join("CMakeLists.txt");

    let contents = fs::read_to_string(entry_point_cmakelists_file_src).map_err(|e| format!("Failed to read entrypoint cmakelists: {e}"))?;

    let modified_content = contents.replace(config.flags.project_name, &project.name);

    fs::write(entry_point_cmakelists_file_dst, modified_content).map_err(|e| format!("failed to write modified entrypoint cmakelists file: {e}"))?;
    Ok(())
}

pub async fn init_syris_source(config : Config, project : NewSyrisProject, multi : Arc<MultiProgress>) -> Result<()>{
    let spinner  = Spinner::new("setting src contents...", Some(multi.clone()));
    fs::create_dir(config.project_paths.src.join(&project.name)).map_err(|e| format!("Failed to create src dir: {e}"))?;
    let project_cmakelists_file_src = config.asharis_root.join("resources").join("Template").join("TemplateCMakeLists.txt");
    let project_cmakelists_file_dst = config.project_paths.src.join("CMakeLists.txt");
    let contents = fs::read_to_string(project_cmakelists_file_src).map_err(|e| format!("Failed to read src cmakelists.txt: {e}"))?;

    let modified_content = contents.replace(config.flags.project_name, &project.name);
    fs::write(project_cmakelists_file_dst, modified_content).map_err(|e| format!("Failed to write modified src cmakelists: {e}"))?;

    let project_name_first_upper_flag = "%PROJECT_NAME_FIRST_UPPER%";
    let project_name_first_upper = utils::first_uppercase(&project.name);

    let template_hpp_file_src = config.asharis_root.join("resources").join("Template").join("Template.hpp");
    let template_hpp_file_dst = config.project_paths.src.join(&project.name).join(format!("{}App.hpp", &project_name_first_upper));
    let contents = fs::read_to_string(template_hpp_file_src).map_err(|e| format!("Failed to read template hpp file: {e}"))?;

    let modified_content = contents.replace(project_name_first_upper_flag, &project_name_first_upper);
    let modified_content = modified_content.replace(config.flags.project_name, &project.name);
//    println!("Path : {}App.hpp", &project_name_first_upper);
    fs::write(template_hpp_file_dst, modified_content).map_err(|e| format!("Failed to write modified template hpp file: {e}"))?;

    let template_cpp_file_src = config.asharis_root.join("resources").join("Template").join("Template.cpp");
    let template_cpp_file_dst = config.project_paths.src.join(&project.name).join(format!("{}App.cpp", &project_name_first_upper));
    let contents = fs::read_to_string(template_cpp_file_src).map_err(|e| format!("Failed to read template cpp file: {e}"))?;

    let modified_content = contents.replace(project_name_first_upper_flag, &project_name_first_upper);
    let modified_content = modified_content.replace(config.flags.project_name, &project.name);
    fs::write(template_cpp_file_dst, modified_content).map_err(|e| format!("failed to write modified template cpp file: {e}"))?;
    Ok(())
}
pub async fn new_syris_project(mut config : Config, project : NewSyrisProject, multi : Arc<MultiProgress>) -> Result<()>{
    let spinner = Spinner::new("Creating directory...", Some(multi.clone()));
    config.project_paths.root = config.project_paths.root.join(PathBuf::from(&project.name));
    if config.project_paths.root.exists(){
        Err("Error already exists")?;
    }
    fs::create_dir(&config.project_paths.root)?;
    spinner.change_message("creating src dir...");
    fs::create_dir(&config.project_paths.src)?;
    
    spinner.change_message("initializing git...");
    let output = Command::new("git")
        .arg("init")
        .current_dir(&config.project_paths.root)
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
    let main_cmakelists_file_dst = config.project_paths.root.join("CMakeLists.txt");

    let contents = fs::read_to_string(main_cmakelists_file_src)?;

    let modified_content = contents.replace(&config.flags.project_name, &project.name);

    fs::write(main_cmakelists_file_dst, modified_content)?;

    spinner.finish();
    pip_glad_install(&config, multi.clone()).await?;
    build_cmake_project(&config, multi.clone())?;
    Ok(())
}

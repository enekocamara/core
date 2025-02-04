use indicatif::{MultiProgress,ProgressBar, ProgressStyle};
use tokio::time::{ sleep, Duration};
use std::fmt::format;
use std::fs;
use std::process::Command;
use std::sync::Arc;


use crate::spinner::Spinner;
use crate::config::Config;
use crate::args::NewProject;
use crate::utils;

pub async fn init_syris(config : Config, project : NewProject, multi : Arc<MultiProgress>) -> Result<(),String>{
    let spinner = Spinner::new("initializing syris",Some(multi));
    let syris_url = "https://github.com/enekocamara/Syris";
    let syris_folder = config.project_root.join("vendor").join("syris");
    //println!("syris folder: '{:?}'", syris_folder);
    let output = Command::new("git")
        .args(["submodule", "add", syris_url, "vendor/syris"])
        .current_dir(&config.project_root)
        .output()
        .map_err(|e| format!("failed to run command {}", e))?;
    if !output.status.success(){
        return Err(format!("failed to add submodule {}", String::from_utf8_lossy(&output.stderr)));
    }

//    println!("Submodule updates...");
    spinner.change_message("initializing submodules recursive...");
    let output = Command::new("git")
        .args(["submodule", "update", "--init", "--recursive"])
        .current_dir(&syris_folder)
        .output()
        .map_err(|e| format!("failed to run command {}", e))?;

    if !output.status.success(){
        return Err(format!("failed to add submodule {}", String::from_utf8_lossy(&output.stderr)));
    }

    //GIT
    spinner.change_message("adding gitignore...");
    let gitignore_src = config.asharis_root.join("resources").join(".gitignore");
    let gitignore_dst = config.project_root.join(".gitignore");
    fs::copy(gitignore_src, gitignore_dst).map_err(|e| format!("failed to copy .gitignore: {}", e))?;

    //IMGUI CMAKE
    spinner.change_message("adding imgui cmakelists...");
    let imgui_src = config.asharis_root.join("resources").join("ImGuiCMakeLists.txt");
    let imgui_dst = config.project_root.join("vendor").join("syris").join("vendor").join("imgui_docking").join("CMakeLists.txt");
    
    fs::copy(imgui_src, imgui_dst).map_err(|e| format!("failed to copy imgui cmakelists: {}", e))?;

    //GLAD CMAKE
    spinner.change_message("adding glad cmakelists...");
    let glad_src = config.asharis_root.join("resources").join("GladCMakeLists.txt");
    let glad_dst = config.project_root.join("vendor").join("syris").join("vendor").join("glad").join("CMakeLists.txt");
    
    fs::copy(glad_src, glad_dst).map_err(|e| format!("failed to copy glad cmakelists: {}", e))?;

    spinner.finish();
    Ok(())
}

pub async fn init_entry_point(config : Config, project : NewProject, multi : Arc<MultiProgress>)->Result<(), String>{
    let spinner = Spinner::new("initializing entry point", Some(multi));
    spinner.change_message("setting entrypoint...");
    let entry_point_src = config.asharis_root.join("resources").join("EntryPoint").join("src");
    utils::copy_dir_rec(&entry_point_src, &config.project_root.join("EntryPoint")).map_err(|e| format!("failed to copy dir entrypoint: {e}"))?;
    let entry_point_cmakelists_file_src = config.asharis_root.join("resources").join("EntryPoint").join("CMakeLists.txt");
    let entry_point_cmakelists_file_dst = config.project_root.join("EntryPoint").join("CMakeLists.txt");

    let contents = fs::read_to_string(entry_point_cmakelists_file_src).map_err(|e| format!("Failed to read entrypoint cmakelists: {e}"))?;

    let modified_content = contents.replace(config.project_name_flag, &project.name);

    fs::write(entry_point_cmakelists_file_dst, modified_content).map_err(|e| format!("failed to write modified entrypoint cmakelists file: {e}"))?;
    Ok(())
}

pub async fn init_vs_conf(config : Config, project : NewProject, multi : Arc<MultiProgress>) -> Result<(), String>{
    let spinner = Spinner::new("setting vs conf...", Some(multi.clone()));
    let vs_conf_src = config.asharis_root.join(".vscode");
    utils::copy_dir_rec(&vs_conf_src, &config.project_root).map_err(|e| format!("Failed to copy /.vscode config: {e}"))?;
    spinner.finish();
    Ok(())
}

pub async fn init_source(config : Config, project : NewProject, multi : Arc<MultiProgress>) -> Result<(), String>{
    let spinner  = Spinner::new("setting src contents...", Some(multi.clone()));
    fs::create_dir(config.project_root.join("src").join(&project.name)).map_err(|e| format!("Failed to create src dir: {e}"))?;
    let project_cmakelists_file_src = config.asharis_root.join("resources").join("Template").join("TemplateCMakeLists.txt");
    let project_cmakelists_file_dst = config.project_root.join("src").join("CMakeLists.txt");
    let contents = fs::read_to_string(project_cmakelists_file_src).map_err(|e| format!("Failed to read src cmakelists.txt: {e}"))?;

    let modified_content = contents.replace(config.project_name_flag, &project.name);
    fs::write(project_cmakelists_file_dst, modified_content).map_err(|e| format!("Failed to write modified src cmakelists: {e}"))?;

    let project_name_first_upper_flag = "%PROJECT_NAME_FIRST_UPPER%";
    let project_name_first_upper = utils::first_uppercase(&project.name);

    let template_hpp_file_src = config.asharis_root.join("resources").join("Template").join("Template.hpp");
    let template_hpp_file_dst = config.project_root.join("src").join(&project.name).join(format!("{}App.hpp", &project_name_first_upper));
    let contents = fs::read_to_string(template_hpp_file_src).map_err(|e| format!("Failed to read template hpp file: {e}"))?;

    let modified_content = contents.replace(project_name_first_upper_flag, &project_name_first_upper);
    let modified_content = modified_content.replace(config.project_name_flag, &project.name);
//    println!("Path : {}App.hpp", &project_name_first_upper);
    fs::write(template_hpp_file_dst, modified_content).map_err(|e| format!("Failed to write modified template hpp file: {e}"))?;

    let template_cpp_file_src = config.asharis_root.join("resources").join("Template").join("Template.cpp");
    let template_cpp_file_dst = config.project_root.join("src").join(&project.name).join(format!("{}App.cpp", &project_name_first_upper));
    let contents = fs::read_to_string(template_cpp_file_src).map_err(|e| format!("Failed to read template cpp file: {e}"))?;

    let modified_content = contents.replace(project_name_first_upper_flag, &project_name_first_upper);
    let modified_content = modified_content.replace(config.project_name_flag, &project.name);
    fs::write(template_cpp_file_dst, modified_content).map_err(|e| format!("failed to write modified template cpp file: {e}"))?;
    Ok(())
}

pub async fn pip_glad_install(config : &Config, multi : Arc<MultiProgress>) -> Result<(), String>{
    // Step 1: Create and activate the virtual environment
    let spinner = Spinner::new("generating glad files", Some(multi));
    //println!("Creating virtual environment...");
    let folders_to_glad = ["vendor", "syris", "vendor", "glad"];
    let path_to_glad = folders_to_glad.iter().fold(config.project_root.clone(), |path, folder| path.join(folder));
    spinner.change_message("creating python venv");
    let outupt = Command::new("python")
        .args(["-m", "venv", "venv"])
        .current_dir(&path_to_glad)
        .output()
        .map_err(|e| format!("Failed to run command: {e}"))?;
    if !outupt.status.success(){
        return Err("failed to create python env".to_string());
    }

    // Step 2: Install glad2 in the virtual environment
    spinner.change_message("installing glad2..");
    let output = Command::new("pip")
        .args(["install", "glad2"])
        .current_dir(path_to_glad.join("venv").join("Scripts"))
        .output()
        .map_err(|e| format!("Failed to run command: {e}"))?;
    if !output.status.success(){
        return Err("failed to create python env".to_string());
    }

    // Step 3: Run glad2 to generate files
    spinner.change_message("generating glad2 files with glad2 generator...");
    let output = Command::new("python")
        .args(["-m", "glad", "--api", "gl:core=4.6", "--out-path", path_to_glad.join("src").to_str().unwrap(), "c"])
        .current_dir(path_to_glad.join("venv").join("Scripts"))
        .output()
        .map_err(|e| format!("Failed to run command: {e}"))?;
    if !output.status.success(){
        return Err(format!("failed to generate glad2, [{}]", String::from_utf8_lossy(&output.stderr)));
    }

    // Step 4: Clean up or notify user
    spinner.change_message("removing python venv...");
    fs::remove_dir_all(path_to_glad.join("venv")).map_err(|e| format!("Failed to remove python venv directory: {e}"))?;
    /*let glad_cmakelists_file_src = config.asharis_root.join("resources").join("GladCMakeLists.txt");
    let glad_cmakelists_file_dst = path_to_glad.join("CMakeLists.txt");

    fs::copy(glad_cmakelists_file_src, glad_cmakelists_file_dst).unwrap();*/
    spinner.finish();
    Ok(())
}

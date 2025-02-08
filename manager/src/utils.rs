use std::env::current_dir;
use std::fmt::format;
use std::path::{PathBuf, Path};
use fs_extra::dir::{self,CopyOptions};
use std::fs;
use std::process::Command;
use regex::Regex;

use crate::Result;

//not losing my time with this heavily based on this code
//https://stackoverflow.com/questions/38406793/why-is-capitalizing-the-first-letter-of-a-string-so-convoluted-in-rust
pub fn first_uppercase(s: &str) -> String {
    let mut c = s.chars();
    c.next().unwrap().to_uppercase().collect::<String>() + c.as_str()
}
pub fn copy_dir_rec(src : &PathBuf, dst : &PathBuf) -> Result<u64>{
    if !src.exists() {
        Err(fs_extra::error::Error::new(fs_extra::error::ErrorKind::NotFound, std::format!("{:?} not found", src).as_str()))?;
    }
    if !dst.exists() {
        fs::create_dir(dst).map_err(|e| fs_extra::error::Error::new(fs_extra::error::ErrorKind::Other, std::format!("failed to create dst dir {}", e).as_str()))?;
    }
    // Define copy options
    let mut options = CopyOptions::new();
    options.overwrite = true; // Overwrite existing files
    options.copy_inside = true; // Copy contents inside the folder
    // Copy the directory
    dir::copy(src, dst, &options).map_err(|e| format!("Failed to copy directory: {e}").into())
}

pub fn insert_after_flag(text : &mut String, flag : &str, text_to_be_inserted : &str) -> Result<()>{
    if let Some(flag_pos) = text.find(flag){
        text.insert_str(flag_pos + flag.len(), text_to_be_inserted);
        return Ok(())
    }
    Err(format!("'{flag}' not found in string"))?
}

pub fn get_cmake_project_name(path_to_cmake_dir : PathBuf) -> Result<String>{
    let cmake_content = fs::read_to_string(path_to_cmake_dir.join("CMakeLists.txt")).map_err(|e| format!("Failed to read cmakefile: {e}"))?;
    let re = Regex::new(r#"project\(\s*([^\s\)]+)"#).unwrap();
    re.captures(&cmake_content)
        .ok_or(format!("Failed to find project(*)"))?
        .get(1).map(|m| m.as_str().to_string())
        .ok_or(format!("Failed to get project name").into())
}
use std::path::PathBuf;
use fs_extra::dir::{self,CopyOptions};
use std::fs;

//not losing my time with this heavily based on this code
//https://stackoverflow.com/questions/38406793/why-is-capitalizing-the-first-letter-of-a-string-so-convoluted-in-rust
pub fn first_uppercase(s: &str) -> String {
    let mut c = s.chars();
    c.next().unwrap().to_uppercase().collect::<String>() + c.as_str()
}
pub fn copy_dir_rec(src : &PathBuf, dst : &PathBuf) -> Result<u64, fs_extra::error::Error>{
    if !src.exists() {
        return Err(fs_extra::error::Error::new(fs_extra::error::ErrorKind::NotFound, std::format!("{:?} not found", src).as_str()));
    }
    if !dst.exists() {
        fs::create_dir(dst).map_err(|e| fs_extra::error::Error::new(fs_extra::error::ErrorKind::Other, std::format!("failed to create dst dir {}", e).as_str()))?;
    }
    // Define copy options
    let mut options = CopyOptions::new();
    options.overwrite = true; // Overwrite existing files
    options.copy_inside = true; // Copy contents inside the folder
    // Copy the directory
    dir::copy(src, dst, &options)
}
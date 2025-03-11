use clap::{Parser,Subcommand,Args,ValueEnum};
use serde::Deserialize;
use colored::*;
use serde::Serialize;

use crate::ConfigFile;
use crate::Result;


#[derive(Parser)]
#[command(author,version,about, long_about=None)]
pub struct Cli {
    #[command(subcommand)]
    pub command: Commands
}


#[derive(Subcommand)]
pub enum Commands{
    ///creates a new project and directory
    New(NewProject),
    ///initializes directory for a  project
    Init,
    ///builds cmake
    CmakeBuild,
    ///builds project
    Build(CMakeOptions),
    ///cleans project from build files
    Clean,
    ///updates cmakelists file to match current config.yaml
    Update,
    ///runs project (builds if necesary) Debug default
    Run(CMakeOptions),
    
    ///module related commands
    #[command(subcommand)]
    Module(ModuleCommands),
    ///target related commands
    #[command(subcommand)]
    Target(TargetCommands),

    ///runs a config command
    Cmd{
        module_name : String,
        cmd_name : String,
        #[arg(trailing_var_arg = true, allow_hyphen_values = true)]
        args : Option<Vec<String>>
    },
    ///runs a specific test
    Test,
    ///runs all the tests
    TestAll,
    ///syris related commands
    #[command(subcommand)]
    Syris(SyrisCommands),
}


#[derive(Subcommand)]
pub enum ModuleCommands{
    ///adds a module
    Add(AddModule),
    ///removes a module
    Remove(RemoveModule),
    ///removes all modules
    RemoveAll,
    ///lists project modules
    List,
    ///lists all dependent modules
    ListAll,
    ///lists available modules
    ListAvailable,
}
#[derive(Subcommand)]
pub enum TargetCommands{
    ///adds a target
    Add(AddTarget),
    ///removes a module
    Remove(RemoveTarget),
    ///removes all targets
    RemoveAll,
    ///changes the current target
    Current{
        #[arg(short='n', long)]
        name : String
    }
}

#[derive(Subcommand)]
pub enum SyrisCommands{
    ///create new project
    New(NewSyrisProject),
    ///builds project
    Build(CMakeOptions)
    
}
#[derive(Args, Clone)]
pub struct RuntimeCmd{
    #[arg(short = 'n', long)]
    pub name : Vec<String>
}

#[derive(Args, Clone)]
pub struct NewProject{
    #[arg(short = 'n', long)]
    pub name : String
}
#[derive(Args, Clone)]
pub struct CMakeOptions{
    #[arg(short = 'c', long)]
    pub config : Option<String>,
    #[arg(short = 't', long)]
    pub target2 : Option<String>
}
impl Default for CMakeOptions{
    fn default() -> CMakeOptions{
        CMakeOptions{config : None, target2 : None}
    }
}
impl CMakeOptions{
    pub fn get_target<'a>(&'a self, config_file : &'a ConfigFile) -> Result<&'a String>{
        match &self.target2{
            Some(target) => {
                if let Some(_) = config_file.targets.get(target){
                    Ok(target)
                }else{
                    Err(format!("Target {} not recognised", target.blue()))?
                }
            }
            None => Ok(&config_file.current_target),
        }
    }
    pub fn get_config(&self) -> Result<&'static str>{
        match &self.config{
            Some(build_config) => match build_config.as_str(){
                "Debug" => Ok("Debug"),
                "Release" => Ok("Release"),
                other => Err(format!("Target {other} not recognised. Use Debug or Release"))?
            }
            None => Ok("Debug")
        }
    }
}

#[derive(Debug, Clone, ValueEnum, Serialize, Deserialize, PartialEq)]
pub enum BinaryType{
    DynamicLibrary,
    StaticLibrary,
    Executable
}

impl ToString for BinaryType{
    fn to_string(&self) -> String {
        match &self {
            BinaryType::DynamicLibrary => "dynamic-library".to_string(),
            BinaryType::StaticLibrary => "static-library".to_string(),
            BinaryType::Executable => "executable".to_string(),
        }
    }
}

#[derive(Args, Clone)]
pub struct AddModule{
    #[arg(short = 'n', long)]
    pub name : String
}

#[derive(Args, Clone)]
pub struct RemoveModule{
    #[arg(short = 'n', long)]
    pub name : String
}
#[derive(Args, Clone, Debug)]
pub struct AddTarget{
    #[arg(short = 'n', long)]
    pub name : String,
    #[arg(short = 'b', long, default_value = "executable")]
    pub binary_type : BinaryType
}

#[derive(Args, Clone)]
pub struct RemoveTarget{
    #[arg(short = 'n', long)]
    pub name : String
}

#[derive(Args, Clone)]
pub struct NewSyrisProject{
    #[arg(short = 'n', long)]
    pub name : String
}

#[derive(Args)]
pub struct Glad{
}
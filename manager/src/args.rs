use std::default;

use clap::{Parser,Subcommand,Args};
use serde::Deserialize;

#[derive(Parser)]
#[command(author,version,about, long_about=None)]
pub struct Cli {
    #[command(subcommand)]
    pub command: Commands
}


#[derive(Subcommand)]
pub enum Commands{
    ///syris related commands
    #[command(subcommand)]
    Syris(SyrisCommands),
    ///creates a new project and directory
    New(NewProject),
    ///initializes directory for a  project
    Init,
    ///adds a module
    Add(AddModule),
    ///removes a module
    #[command(subcommand)]
    Remove(RemoveModule),
    //builts projcet Debug default
    Build(BuildProject),
    //builds cmake
    CmakeBuild,
    //runs project (builds if necesary) Debug default
    Run(RunProject),
    //cleans project from build files
    Clean,
    //lists current modules
    List,
    ///lists all installed modules
    ListInstalled,
    //lists available modules
    ListAvailable,
    //updates cmakelists
    Update,
}

#[derive(Subcommand)]
pub enum SyrisCommands{
    ///create new project
    New(NewSyrisProject),
    ///builds project
    Build(BuildProject)
    
}

#[derive(Args, Clone)]
pub struct NewProject{
    #[arg(short = 'n', long)]
    pub name : String
}
#[derive(Args, Clone)]
pub struct BuildProject{
    #[arg(short = 't', long)]
    pub target : Option<String>
}
impl Default for BuildProject{
    fn default() -> BuildProject{
        BuildProject{target : None}
    }
}

#[derive(Args, Clone)]
pub struct RunProject{
    #[arg(short = 't', long)]
    pub target : Option<String>
}
#[derive(Args, Clone)]
pub struct AddModule{
    #[arg(short = 'n', long)]
    pub name : String
}

#[derive(Subcommand, Clone)]
pub enum RemoveModule{
    Name{
        name : String
    },
    All
}

#[derive(Args, Clone)]
pub struct NewSyrisProject{
    #[arg(short = 'n', long)]
    pub name : String
}

#[derive(Args)]
pub struct Glad{
}
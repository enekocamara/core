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
    ///config check
    Config(Config),
    ///makes a cmakelists
    New(NewProject),
    ///adds a library
    Add(AddLibrary),
    ///removes a library
    Remove(RemoveLibrary),
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
pub struct AddLibrary{
    #[arg(short = 'n', long)]
    pub name : String
}

#[derive(Args, Clone)]
pub struct RemoveLibrary{
    #[arg(short = 'n', long)]
    pub name : String
}

#[derive(Args, Clone)]
pub struct NewSyrisProject{
    #[arg(short = 'n', long)]
    pub name : String
}
#[derive(Args, Clone)]
pub struct NewProject{
    #[arg(short = 'n', long)]
    pub name : String
}

#[derive(Args)]
pub struct BuildProject{
}

#[derive(Args)]
pub struct Glad{
}

#[derive(Args)]
pub struct Config{
    
}


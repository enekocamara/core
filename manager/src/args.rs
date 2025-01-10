use clap::{Parser,Subcommand,Args};


#[derive(Parser)]
#[command(author,version,about, long_about=None)]
pub struct Cli {
    #[command(subcommand)]
    pub command: Commands
}

#[derive(Subcommand)]
pub enum Commands{
    ///create new project
    New(NewProject)
}

#[derive(Args)]
pub struct NewProject{
    #[arg(short = 'n', long)]
    pub name : String
}
use std::fmt::{self, Formatter};
use colored::*;
use derive_more::{From, Display};
use git2;
use url;
use reqwest;
use fs_extra;

pub type Result<T> = core::result::Result<T, Error>;
//pub type Error = Box<dyn std::error::Error + Send + Sync>;

#[derive(From, Display)]
pub enum Error{
    #[from]
    Custom(String),

    //git2
    #[from]
    Git2(git2::Error),

    //url
    #[from]
    Url(url::ParseError),

    //reqwest
    #[from]
    Reqwest(reqwest::Error),

    //fs extra
    #[from]
    FsExtra(fs_extra::error::Error),

    #[from]
    Io(std::io::Error),
}

impl Error {
    pub fn custom(val: impl std::fmt::Display) -> Self{
        Self::Custom(val.to_string())
    }
}

impl From<&str> for Error{
    fn from(val : &str) -> Self{
        Self::Custom(val.to_string())
    }
}

impl core::fmt::Debug for Error{
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        write!(f, "{}: {self}", "Error".red().bold())
    }
}
/* 
impl core::fmt::Display for Error{
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        write!(f, "{}: {self:?}", "Error red?".red().bold())
    }
}*/

/*pub struct ColoredError(Error);

impl fmt::Display for ColoredError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}: {}", "Error".red().bold(), self.0)
    }
}
impl fmt::Debug for ColoredError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", self.0)
    }
}

impl From<Error> for ColoredError {
    fn from(err: Error) -> Self {
        ColoredError(err)
    }
}
impl From<String> for ColoredError {
    fn from(err: String) -> Self {
        ColoredError(Box::new(std::io::Error::new(std::io::ErrorKind::Other, err)))
    }
}
impl From<&str> for ColoredError {
    fn from(err: &str) -> Self {
        ColoredError(Box::new(std::io::Error::new(std::io::ErrorKind::Other, err.to_string())))
    }
}
*/
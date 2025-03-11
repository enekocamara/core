use indicatif::{MultiProgress,ProgressBar, ProgressStyle};
use tokio::time::Duration;
use std::sync::Arc;
use std::borrow::Cow;
pub struct Spinner{
    spinner : ProgressBar,
}

impl Spinner {
    pub fn new(message : impl Into<Cow<'static, str>>, multi_progress : Option<Arc<MultiProgress>>) -> Spinner{
        let mut spinner = ProgressBar::new_spinner();
        if let Some(multi_progress) = multi_progress{
            spinner = multi_progress.add(spinner);
        }
        spinner.set_style(ProgressStyle::default_spinner()
            .tick_strings(&["⠁", "⠂", "⠄", "⡀", "⢀", "⠠", "⠐", "⠈", ""])
            .template("{spinner} {msg}")
            .unwrap());
        spinner.enable_steady_tick(Duration::from_millis(100));
        spinner.set_message(message);
        Spinner{
            spinner
        }
    }

    pub fn finish(self : &Self){
        self.spinner.finish_and_clear();
    }
    pub fn change_message(self : &Self, message : impl Into<Cow<'static, str>>){
        self.spinner.set_message(message);
    }
}

pub struct MultiText{
    progress_bar : ProgressBar,
    text : String 
}

impl MultiText{
    pub fn new(message : String, multi_progress : Option<Arc<MultiProgress>>) -> MultiText{
        let mut text_bar = ProgressBar::new_spinner();
        if let Some(multi_progress) = multi_progress{
            text_bar = multi_progress.add(text_bar);
        }
        text_bar.set_style(ProgressStyle::default_spinner()
            .template("{msg}")
            .unwrap());
        text_bar.enable_steady_tick(Duration::from_millis(100));
        let text : String = message.clone();
        text_bar.set_message(message);
        MultiText{
            progress_bar : text_bar,
            text : text
        }
    }
    pub fn push_str(self : &mut Self, text : &str) {
        self.text.push_str(text);
        self.progress_bar.set_message(self.text.clone());
    }
    pub fn finish(self : &Self){
        self.progress_bar.finish();
    }
}
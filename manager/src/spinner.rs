use indicatif::{MultiProgress,ProgressBar, ProgressStyle};
use tokio::time::{ sleep, Duration};
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
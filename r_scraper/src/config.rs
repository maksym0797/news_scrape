use dotenv::dotenv;

#[derive(Debug, Clone)]
pub struct Config {
    pub db_url: String,
}

pub struct ConfigLoader {}

impl ConfigLoader {
    pub fn load() -> Config {
        dotenv().ok();
        Config::new()
    }
}

impl Config {
    pub fn new() -> Self {
        let db_url = std::env::var("DB_URL").expect("DB_URL must be set");

        Self { db_url }
    }
}

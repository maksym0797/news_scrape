use dotenv::dotenv;

#[derive(Debug, Clone)]
pub struct Config {
    pub db_url: String,
    pub web_driver_url: Option<String>,
}

pub struct ConfigLoader {}

impl ConfigLoader {
    pub fn load() -> Config {
        dotenv().ok();

        let db_url = ConfigLoader::get_var("DB_URL", true).unwrap();
        let web_driver_url = ConfigLoader::get_var("WEB_DRIVER_URL", false);

        Config {
            db_url,
            web_driver_url,
        }
    }

    fn get_var(name: &str, is_required: bool) -> Option<String> {
        let var = match is_required {
            true => std::env::var(name)
                .map(|v| ConfigLoader::validate_empty_string(&v))
                .unwrap_or(None),
            false => std::env::var(name)
                .map(|v| ConfigLoader::validate_empty_string(&v))
                .unwrap_or(None),
        };

        if is_required && var.is_none() {
            panic!("{} must be set", name);
        }

        var
    }

    fn validate_empty_string(value: &str) -> Option<String> {
        if value.is_empty() {
            None
        } else {
            Some(value.to_string())
        }
    }
}

mod config;
mod db;
mod parsing;
mod web_driver;

use parsing::SourceParser;
use std::{fmt::Display, sync::Arc, time::Duration};
use tokio::task::JoinHandle;

use web_driver::{Browser, SessionManager, WebDriver};

#[derive(Debug)]
struct ParsedResponse {
    url: String,
    text: Vec<String>,
}

impl Display for ParsedResponse {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "Parsed response for url: {}. The results are: {}",
            self.url,
            self.text.join(", ")
        )
    }
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let config = config::ConfigLoader::load();

    let runtime = tokio::runtime::Builder::new_multi_thread()
        .worker_threads(4)
        .enable_all()
        .build()?;

    runtime.block_on(async {
        let mut tasks: Vec<
            JoinHandle<Result<ParsedResponse, Box<dyn std::error::Error + Send + Sync>>>,
        > = Vec::new();

        let db = db::DB::new(&config.db_url).await?;

        let user_sources = db.get_sources().await?;
        println!("Got {} sources", user_sources.len());
        let client = reqwest::Client::builder()
            .timeout(Duration::from_secs(5))
            .build()
            .unwrap();

        let driver = Arc::new(WebDriver::default());
        let session_manager = Arc::new(SessionManager::default());
        let max_sessions = if user_sources.len() < 5 {
            user_sources.len()
        } else {
            5
        };

        println!("Spawning {max_sessions} sessions");
        for _ in 0..max_sessions {
            let session_id = driver.create_session(Browser::CHROME).await?;
            println!("Created session with ID: {session_id}");
            session_manager.push(session_id).await;
        }

        for source in user_sources {
            tasks.push(tokio::spawn({
                let client_ref = client.clone();
                let driver_ref = driver.clone();
                let session_manager_ref = session_manager.clone();

                async move {
                    let url = source.link;

                    if !source.is_eager {
                        let response = client_ref.get(&url).send().await?;
                        let content = response.text().await?;
                        Ok(ParsedResponse {
                            url,
                            text: SourceParser::from(source.name.as_str()).parse(&content),
                        })
                    } else {
                        let session_id = session_manager_ref.aquire().await;
                        println!("Task for {} is using ID {}", &url, &session_id);

                        driver_ref.navigate_to_url(&session_id, &url).await?;
                        let content = driver_ref.get_session_url_content(&session_id).await?;

                        session_manager_ref.release(session_id).await;
                        Ok(ParsedResponse {
                            url,
                            text: SourceParser::from(source.name.as_str()).parse(&content),
                        })
                    }
                }
            }));
        }

        for task in tasks {
            match task.await {
                Ok(result) => match result {
                    Ok(parsed_response) => println!("{parsed_response}"),
                    Err(e) => eprintln!("Task failed: {:?}", e),
                },
                Err(e) => eprintln!("Join failed: {:?}", e),
            }
        }

        for session_id in Arc::try_unwrap(session_manager).unwrap().take().await {
            println!("Will delete session with ID: {session_id}");
            driver.delete_session(&session_id).await?;
        }

        Ok(())
    })
}

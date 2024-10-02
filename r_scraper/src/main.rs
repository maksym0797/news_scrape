mod config;
mod db;
mod logger;
mod parsing;
mod web_driver;

use db::{RawPost, DB};

use parsing::SourceParser;
use std::{sync::Arc, time::Duration};
use tokio::task::JoinHandle;

use web_driver::{Browser, SessionManager, WebDriver};

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let config = config::ConfigLoader::load();

    let runtime = tokio::runtime::Builder::new_multi_thread()
        .worker_threads(4)
        .enable_all()
        .build()?;

    runtime.block_on(async {
        let mut tasks: Vec<JoinHandle<Result<RawPost, Box<dyn std::error::Error + Send + Sync>>>> =
            Vec::new();

        let db = DB::new(&config.db_url).await?;

        let user_sources = db.get_sources().await?;

        info!("Got {} sources", user_sources.len());

        let client = reqwest::Client::builder()
            .timeout(Duration::from_secs(5))
            .build()
            .unwrap();

        let driver = Arc::new(WebDriver::new(config.web_driver_url));
        let session_manager = Arc::new(SessionManager::default());
        let max_sessions = if user_sources.len() < 5 {
            user_sources.len()
        } else {
            5
        };

        info!("Spawning {max_sessions} sessions");
        for _ in 0..max_sessions {
            let session_id = driver.create_session(Browser::Chrome).await?;
            info!("Created session with ID: {session_id}");
            session_manager.push(session_id).await;
        }

        for source in user_sources {
            info!("Will spawn thread to process source: {}", source.link);
            tasks.push(tokio::spawn({
                let client_ref = client.clone();
                let driver_ref = driver.clone();
                let session_manager_ref = session_manager.clone();

                async move {
                    let url = source.link;

                    if !source.is_eager {
                        let response = client_ref.get(&url).send().await?;
                        let content = response.text().await?;
                        let parse_result = SourceParser::from(source.name.as_str()).parse(&content);
                        return Ok(RawPost::new(
                            source.source_id,
                            parse_result.title,
                            parse_result.content,
                            None,
                        ));
                    }

                    let session_id = session_manager_ref.aquire().await;
                    info!("Task for {} is using ID {}", &url, &session_id);

                    driver_ref.navigate_to_url(&session_id, &url).await?;
                    let content = driver_ref.get_session_url_content(&session_id).await?;

                    session_manager_ref.release(session_id).await;
                    let parse_result = SourceParser::from(source.name.as_str()).parse(&content);

                    Ok(RawPost::new(
                        source.source_id,
                        parse_result.title,
                        parse_result.content,
                        None,
                    ))
                }
            }));
        }

        let mut posts = Vec::with_capacity(tasks.len());
        for task in tasks {
            match task.await {
                Ok(result) => match result {
                    Ok(parsed_response) => {
                        posts.push(parsed_response);
                    }
                    Err(e) => error!("Task failed: {:?}", e),
                },
                Err(e) => error!("Join failed: {:?}", e),
            }
        }

        info!("Will create {} raw posts", posts.len());
        db.create_raw_posts(&posts).await?;
        info!("Created raw posts");

        for session_id in Arc::try_unwrap(session_manager).unwrap().take().await {
            info!("Will delete session with ID: {session_id}");
            driver.delete_session(&session_id).await?;
        }

        Ok(())
    })
}

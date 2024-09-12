#![warn(unused_imports)]

mod parsing;
mod web_driver;

use parsing::html::parse_html;
use reqwest::Error;
use std::{fmt::Display, time::Duration};

use web_driver::{Browser, WebDriver};

#[derive(Debug)]
struct ParsedResponse<'a> {
    url: &'a str,
    text: Vec<String>,
}

impl<'a> Display for ParsedResponse<'a> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "Parsed response for url: {}. The results are: {}",
            self.url,
            self.text.join(", ")
        )
    }
}

fn main() -> Result<(), Error> {
    let runtime = tokio::runtime::Builder::new_multi_thread()
        .worker_threads(4)
        .enable_all()
        .build()
        .unwrap();

    let mut tasks = Vec::new();
    runtime.block_on(async {
        let urls = vec![
            "https://doc.rust-lang.org/rust-by-example/mod/super.html",
            "https://doc.rust-lang.org/rust-by-example/mod/split.html",
        ];
        let client = reqwest::Client::builder()
            .timeout(Duration::from_secs(5))
            .build()
            .unwrap();

        let blocking_req = tokio::task::spawn_blocking({
            move || {
                let driver = WebDriver::default();

                let session_id = driver.create_session(Browser::CHROME);

                if session_id.is_err() {
                    todo!("Handle error");
                }
                let session_id = session_id.unwrap();

                let navigation_result = driver.navigate_to_url(
                    &session_id,
                    "https://medium.com/coding-blocks/creating-user-database-and-adding-access-on-postgresql-8bfcd2f4a91e"
                );

                if navigation_result.is_err() {
                    todo!("Handle error");

                }

                let content = driver.get_session_url_content(&session_id);

                content
            }
        });

        let db_data = blocking_req.await;

        match db_data {
            Ok(content) => match content {
                Ok(c) => {
                    println!("Content: {:?}", parse_html(&c));
                }
                Err(e) => {
                    println!("Error: {e:?}");
                }
            },
            Err(e) => {
                println!("Error : {e}")
            }
        }


        for url in urls {
            tasks.push(tokio::spawn({
                let cloned = client.clone();
                async move {
                    println!("Shit from inside tokio::spawn");
                    let response = cloned.get(url).send().await.unwrap();
                    let body = response.text().await.unwrap();
                    return ParsedResponse {
                        url,
                        text: parse_html(&body),
                    };
                }
            }));
        }

        for task in tasks {
            match task.await {
                Ok(result) => println!("Task completed. {result}",),
                Err(e) => eprintln!("Task failed: {:?}", e),
            }
        }

        Ok(())
    })
}

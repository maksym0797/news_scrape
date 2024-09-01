mod parsing;

use parsing::html::parse_html;
use reqwest::Error;
use std::time::Duration;

#[tokio::main]
async fn main() -> Result<(), Error> {
    let url = "https://doc.rust-lang.org/rust-by-example/mod/split.html";
    let client = reqwest::Client::builder()
        .timeout(Duration::from_secs(5))
        .build()
        .unwrap();

    let response = client.get(url).send().await?;
    let body = response.text().await?;

    println!("HTML: {:?}", parse_html(&body));

    Ok(())
}

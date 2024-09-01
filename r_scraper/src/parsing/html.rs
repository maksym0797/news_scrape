use scraper::{Html, Selector};

pub fn parse_html(html: &str) -> Vec<String> {
    let document = Html::parse_document(html);
    let selector = Selector::parse("h1").unwrap();

    document
        .select(&selector)
        .into_iter()
        .map(|e| e.text().collect::<Vec<_>>().concat())
        .collect::<Vec<_>>()
}

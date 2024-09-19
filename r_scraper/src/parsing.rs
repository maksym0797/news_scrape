use scraper::{Html, Selector};

pub fn parse_html(html: &str) -> Vec<String> {
    let document = Html::parse_document(html);
    let title_selector = Selector::parse(".pw-post-title").unwrap();
    let body_selector = Selector::parse(".pw-post-body-paragraph").unwrap();

    let mut title = document
        .select(&title_selector)
        .map(|e| e.text().collect::<Vec<_>>().concat())
        .collect::<Vec<_>>();

    let body = document
        .select(&body_selector)
        .map(|e| e.text().collect::<Vec<_>>().concat());

    title.extend(body);

    title
}

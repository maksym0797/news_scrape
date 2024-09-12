use scraper::{Html, Selector};

pub fn parse_html(html: &str) -> Vec<String> {
    let document = Html::parse_document(html);
    let title_selector = Selector::parse(".pw-post-title").unwrap();
    let body_selector = Selector::parse(".pw-post-body-paragraph").unwrap();

    document
        .select(&title_selector)
        .into_iter()
        .map(|e| {
            println!("{e:?}");
            e.text().collect::<Vec<_>>().concat()
        })
        .collect::<Vec<_>>()
}

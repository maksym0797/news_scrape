use scraper::{Html, Selector};

pub enum SourceParser {
    Medium,
    NotSpecific,
}

impl From<&str> for SourceParser {
    fn from(source_name: &str) -> Self {
        match source_name.to_ascii_lowercase().as_str() {
            "medium" => SourceParser::Medium,
            _ => SourceParser::NotSpecific,
        }
    }
}

impl SourceParser {
    pub fn parse(&self, html: &str) -> Vec<String> {
        match self {
            SourceParser::Medium => self.parse_medium(html),
            SourceParser::NotSpecific => {
                self.parse_html(html, vec![Selector::parse("title").unwrap()])
            }
        }
    }

    fn parse_medium(&self, html: &str) -> Vec<String> {
        let title_selector = Selector::parse(".pw-post-title").unwrap();
        let body_selector = Selector::parse(".pw-post-body-paragraph").unwrap();

        self.parse_html(html, vec![title_selector, body_selector])
    }

    pub fn parse_html(&self, html: &str, selectors: Vec<Selector>) -> Vec<String> {
        let document = Html::parse_document(html);

        selectors
            .iter()
            .flat_map(|selector| {
                document
                    .select(selector)
                    .map(|e| e.text().collect::<Vec<_>>().concat())
                    .collect::<Vec<_>>()
            })
            .collect()
    }
}

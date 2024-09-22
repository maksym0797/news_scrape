use scraper::{Html, Selector};

pub struct ParserResult {
    pub title: String,
    pub content: Option<String>,
}

trait Parser {
    fn parse(&self, html: &str) -> ParserResult;
}
pub struct MediumParser;

impl Parser for MediumParser {
    fn parse(&self, html: &str) -> ParserResult {
        let title_selector = Selector::parse(".pw-post-title").unwrap();
        let body_selector = Selector::parse("[data-selectable-paragraph]").unwrap();

        let document = Html::parse_document(html);

        let title: String = document
            .select(&title_selector)
            .map(|e| e.text().collect::<Vec<_>>().concat())
            .collect::<Vec<_>>()
            .join("");

        let content = document
            .select(&body_selector)
            .map(|e| e.text().collect::<Vec<_>>().concat())
            .collect::<Vec<_>>()
            .join("\n");

        ParserResult {
            title,
            content: Some(content),
        }
    }
}

pub struct TitleParser;

impl Parser for TitleParser {
    fn parse(&self, html: &str) -> ParserResult {
        let title_selector = Selector::parse("title").unwrap();

        let document = Html::parse_document(html);

        let title = document
            .select(&title_selector)
            .map(|e| e.text().collect::<Vec<_>>().concat())
            .collect::<Vec<String>>()
            .join(" ");

        ParserResult {
            title,
            content: None,
        }
    }
}

pub enum SourceParser {
    Medium(MediumParser),
    NotSpecific(TitleParser),
}

impl From<&str> for SourceParser {
    fn from(source_name: &str) -> Self {
        match source_name.to_ascii_lowercase().as_str() {
            "medium" => SourceParser::Medium(MediumParser {}),
            _ => SourceParser::NotSpecific(TitleParser {}),
        }
    }
}

impl SourceParser {
    pub fn parse(&self, html: &str) -> ParserResult {
        match self {
            SourceParser::Medium(m) => m.parse(html),
            SourceParser::NotSpecific(t) => t.parse(html),
        }
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_medium_parser() {
        let parser = MediumParser {};
        let html = r#"
            <html>
                <head>
                    <title>Test title</title>
                </head>
                <body>
                    <h1 class="pw-post-title">Test title</h1>
                    <div class="pw-post-body-paragraph"data-selectable-paragraph="">Test content</div>
                    <div class="pw-post-body-paragraph" data-selectable-paragraph="">Test content 2</div>
                    <p id="7264" class="pw-post-body-paragraph mr ms fq mt b mu mv mw mx my mz na nb nc nd ne nf ng nh ni nj nk nl nm nn no fj bk" data-selectable-paragraph=""><strong class="mt fr"><em class="np">NOTE</em></strong><em class="np">: Right off the bat — this is valid as on March 2017, running on Ubuntu 16.04.2, with PostgreSQL 9.6</em></p>
                </body>
            </html>
        "#;

        let result = parser.parse(html);

        assert_eq!(result.title, "Test title");
        assert_eq!(
            result.content,
            Some("Test content\nTest content 2\nNOTE: Right off the bat — this is valid as on March 2017, running on Ubuntu 16.04.2, with PostgreSQL 9.6".to_string())
        );
    }

    #[test]
    fn test_title_parser() {
        let parser = TitleParser {};
        let html = r#"
            <html>
                <head>
                    <title>Test title</title>
                </head>
                <body>
                    <h1>Test title</h1>
                    <p>Test content</p>
                </body>
            </html>
        "#;

        let result = parser.parse(html);

        assert_eq!(result.title, "Test title");
        assert_eq!(result.content, None);
    }
}

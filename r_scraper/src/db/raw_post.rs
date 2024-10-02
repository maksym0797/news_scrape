use sqlx::types::{chrono, Uuid};

#[derive(Debug)]
pub struct RawPost {
    pub id: Uuid,
    pub source_id: Uuid,
    pub title: String,
    pub content: String,
    pub published_at: chrono::DateTime<chrono::Utc>,
}

impl RawPost {
    pub fn new(
        source_id: Uuid,
        title: String,
        content: Option<String>,
        published_at: Option<chrono::DateTime<chrono::Utc>>,
    ) -> Self {
        Self {
            id: Uuid::new_v4(),
            source_id,
            title,
            content: content.unwrap_or_default(),
            published_at: published_at.unwrap_or_else(chrono::Utc::now),
        }
    }
}

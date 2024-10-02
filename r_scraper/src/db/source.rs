use sqlx::types::Uuid;

#[derive(sqlx::FromRow, Debug)]
pub struct Source {
    pub source_id: Uuid,
    pub link: String,
    pub name: String,
    pub is_eager: bool,
}

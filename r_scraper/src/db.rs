use std::result::Result;

use sqlx::{
    types::{chrono, Uuid},
    Pool, Postgres,
};

#[derive(sqlx::FromRow, Debug)]
pub struct Source {
    pub source_id: Uuid,
    pub link: String,
    pub name: String,
    pub is_eager: bool,
}

#[derive(Debug)]
pub struct RawPost {
    pub id: Uuid,
    pub source_id: Uuid,
    pub title: String,
    pub content: String,
    pub published_at: chrono::DateTime<chrono::Utc>,
}

pub struct DB {
    pool: Pool<Postgres>,
}

impl DB {
    pub async fn new(db_url: &str) -> Result<Self, sqlx::Error> {
        let pool = sqlx::postgres::PgPoolOptions::new()
            .max_connections(5)
            .connect(db_url)
            .await?;

        Ok(Self { pool })
    }

    pub async fn get_sources(&self) -> Result<Vec<Source>, sqlx::Error> {
        let rows = sqlx::query_as::<_, Source>("SELECT s.link as link, s.id as source_id, st.is_eager, st.name as name FROM sources as s JOIN source_types as st ON s.source_type_id = st.id")
            .fetch_all(&self.pool)
            .await?;

        Ok(rows)
    }

    pub async fn create_raw_posts(&self, posts: &[RawPost]) -> Result<(), sqlx::Error> {
        let l = posts.len();
        let mut ids = Vec::with_capacity(l);
        let mut source_ids = Vec::with_capacity(l);
        let mut titles = Vec::with_capacity(l);
        let mut contents = Vec::with_capacity(l);
        let mut published_ats = Vec::with_capacity(l);

        for post in posts {
            ids.push(post.id);
            source_ids.push(post.source_id);
            titles.push(post.title.as_str());
            contents.push(post.content.as_str());
            published_ats.push(post.published_at);
        }
        sqlx::query(
            r"INSERT INTO raw_posts (id, source_id, title, content, published_at, created_at, updated_at) 
            SELECT * FROM UNNEST(
                $1::uuid[],
                $2::uuid[],
                $3::text[],
                $4::text[],
                $5::timestamptz[],
                array_fill($6::timestamptz, ARRAY[cardinality($5)]),
                array_fill($7::timestamptz, ARRAY[cardinality($5)])
            )",
        )
        .bind(&ids)
        .bind(&source_ids)
        .bind(&titles)
        .bind(&contents)
        .bind(&published_ats)
        .bind(chrono::Utc::now())
        .bind(chrono::Utc::now())
        .execute(&self.pool)
        .await?;

        Ok(())
    }
}

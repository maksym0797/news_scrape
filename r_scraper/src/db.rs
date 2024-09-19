use sqlx::{types::Uuid, Pool, Postgres};

#[derive(sqlx::FromRow, Debug)]
pub struct UserSource {
    pub user_id: Uuid,
    pub link: String,
    pub is_eager: bool,
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

    pub async fn get_sources(&self) -> Result<Vec<UserSource>, sqlx::Error> {
        let rows = sqlx::query_as::<_, UserSource>("SELECT u.id as user_id, s.link as link, st.is_eager FROM users as u JOIN sources as s ON u.id = s.user_id JOIN source_types as st ON s.source_type_id = st.id")
            .fetch_all(&self.pool)
            .await?;

        Ok(rows)
    }
}

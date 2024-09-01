package model

import (
	"time"

	"github.com/google/uuid"
	"gorm.io/gorm"
)

type Post struct {
	gorm.Model
	ID          uuid.UUID `gorm:"type:uuid;"`
	Title       string    `json:"title" gorm:"type:varchar(255);index"`
	Description string    `json:"description"`
	SourceID    uuid.UUID `json:"source_id"`
	Source      Source    `json:"source" gorm:"foreignKey:SourceID;constraint:OnUpdate:CASCADE,OnDelete:SET NULL;"`
	Link        string    `json:"link"`
	Topics      []Topic   `gorm:"many2many:post_topics;"`
	Users       []User    `gorm:"many2many:user_posts;"`
	CreatedAt   time.Time `json:"created_at"`
	UpdatedAt   time.Time `json:"updated_at"`
}

func (post *Post) BeforeCreate(tx *gorm.DB) (err error) {
	// UUID version 4
	post.ID = uuid.New()
	return
}

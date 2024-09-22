package model

import (
	"time"

	"github.com/google/uuid"
	"gorm.io/gorm"
)

type RawPost struct {
	gorm.Model
	ID          uuid.UUID `gorm:"type:uuid;"`
	Title       string    `json:"title"`
	Content     string    `json:"content"`
	SourceID    uuid.UUID `json:"source_id"`
	Source      Source    `json:"source" gorm:"foreignKey:SourceID;constraint:OnUpdate:CASCADE,OnDelete:SET NULL;"`
	PublishedAt time.Time `json:"published_at"`
	CreatedAt   time.Time `json:"created_at"`
	UpdatedAt   time.Time `json:"updated_at"`
}

func (post *RawPost) BeforeCreate(tx *gorm.DB) (err error) {
	// UUID version 4
	post.ID = uuid.New()
	return
}

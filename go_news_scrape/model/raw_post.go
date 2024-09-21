package model

import (
	"time"

	"github.com/google/uuid"
	"gorm.io/gorm"
)

type RawPost struct {
	gorm.Model
	ID        uuid.UUID `gorm:"type:uuid;"`
	Title     string    `json:"title"`
	Content   string    `json:"content"`
	CreatedAt time.Time `json:"created_at"`
	UpdatedAt time.Time `json:"updated_at"`
}

func (post *RawPost) BeforeCreate(tx *gorm.DB) (err error) {
	// UUID version 4
	post.ID = uuid.New()
	return
}

package model

import (
	"time"

	"github.com/google/uuid"
	"gorm.io/gorm"
)

type Topic struct {
	gorm.Model
	ID        uuid.UUID `gorm:"type:uuid;"`
	Name      string    `gorm:"type:varchar(255);" json:"topic_name"`
	CreatedAt time.Time `json:"created_at"`
	UpdatedAt time.Time `json:"updated_at"`
}

func (post *Topic) BeforeCreate(tx *gorm.DB) (err error) {
	// UUID version 4
	post.ID = uuid.New()
	return
}

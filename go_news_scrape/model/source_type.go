package model

import (
	"github.com/google/uuid"
	"gorm.io/gorm"
)

type SourceType struct {
	gorm.Model
	ID      uuid.UUID `gorm:"type:uuid;"`
	Name    string    `json:"name"`
	Link    string    `json:"link"`
	IsEager bool      `json:"isEager"` // tells scraper the resource can just fetch raw html without js rendering
}

func (post *SourceType) BeforeCreate(tx *gorm.DB) (err error) {
	// UUID version 4
	post.ID = uuid.New()
	return
}

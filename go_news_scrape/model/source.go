package model

import (
	"github.com/google/uuid"
	"gorm.io/gorm"
)

type Source struct {
	gorm.Model
	ID           uuid.UUID `gorm:"type:uuid;"`
	Name         string    `json:"name"`
	Link         string    `json:"link"`
	SourceTypeID uuid.UUID `json:"source_type_id"`
	SourceType   SourceType
}

func (post *Source) BeforeCreate(tx *gorm.DB) (err error) {
	// UUID version 4
	post.ID = uuid.New()
	return
}

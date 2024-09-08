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
	UserId       uuid.UUID `json:"user_id"`
	User         User      `json:"user" gorm:"foreignKey:UserId;constraint:OnUpdate:CASCADE,OnDelete:CASCADE;"`
	SourceType   SourceType
}

func (post *Source) BeforeCreate(tx *gorm.DB) (err error) {
	// UUID version 4
	post.ID = uuid.New()
	return
}

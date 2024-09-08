package handler

import (
	"github.com/gofiber/fiber/v2"
	"github.com/mkostynskyi0797/go_news_scrape/database"
	"github.com/mkostynskyi0797/go_news_scrape/model"
)

func GetSourceTypes(c *fiber.Ctx) error {
	db := database.DB.Db
	var sourceTypes []model.SourceType
	db.Find(&sourceTypes)
	return c.Status(fiber.StatusCreated).JSON(fiber.Map{
		"data": sourceTypes,
	})
}

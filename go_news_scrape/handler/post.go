package handler

import (
	"github.com/gofiber/fiber/v2"
	"github.com/mkostynskyi0797/go_news_scrape/database"
	"github.com/mkostynskyi0797/go_news_scrape/model"
)

func GetSinglePost(c *fiber.Ctx) error {
	db := database.DB.Db
	id := c.Params("id")

	var post model.RawPost
	if err := db.Where("ID = ?", id).First(&post).Error; err != nil {
		return c.Status(fiber.StatusInternalServerError).JSON(fiber.Map{"status": "error", "message": "Internal server error", "data": err.Error()})
	}
	return c.Status(fiber.StatusOK).JSON(fiber.Map{"status": "success", "message": "Post found", "data": post})
}

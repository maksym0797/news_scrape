package handler

import (
	"github.com/gofiber/fiber/v2"
	"github.com/mkostynskyi0797/go_news_scrape/database"
	"github.com/mkostynskyi0797/go_news_scrape/model"
)

// AddSourceHandler handles the POST request to add a new user_source
func AddUserSource(c *fiber.Ctx) error {
	var source model.Source
	db := database.DB.Db
	if err := c.BodyParser(&source); err != nil {
		return c.Status(fiber.StatusBadRequest).JSON(fiber.Map{
			"error": "Invalid request body",
		})
	}

	user, err := GetUserById(c.Locals("userId").(string))

	if err != nil {
		return c.Status(fiber.StatusBadRequest).JSON(fiber.Map{
			"error": "Invalid request body",
		})
	}

	user.Sources = append(user.Sources, source)

	db.Save(&user)

	return c.Status(fiber.StatusCreated).JSON(fiber.Map{
		"message": "Source added successfully",
		"data":    source,
	})
}

// RemoveSourceHandler handles the DELETE request to remove a user_source
func RemoveUserSource(c *fiber.Ctx) error {
	db := database.DB.Db
	sourceId := c.Params("sourceId")
	userId := c.Locals("userId").(string)

	source := model.Source{}
	errSource := db.Where("id = ?", sourceId).First(&source).Error

	if errSource != nil {
		return c.Status(fiber.StatusBadRequest).JSON(fiber.Map{"status": "error", "message": "Source not found", "data": errSource.Error()})
	}

	err := db.Unscoped().Model(&model.Source{}).Where("user_id = ?", userId).Delete(&source).Error

	if err != nil {
		return c.Status(fiber.StatusInternalServerError).JSON(fiber.Map{"status": "error", "message": "Internal server error", "data": err.Error()})
	}

	return c.Status(fiber.StatusOK).JSON(fiber.Map{
		"message": "Source deleted successfully",
		"data":    sourceId,
	})
}

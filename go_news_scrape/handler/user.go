package handler

import (
	"log"

	"github.com/gofiber/fiber/v2"
	"github.com/mkostynskyi0797/go_news_scrape/database"
	"github.com/mkostynskyi0797/go_news_scrape/model"
	"golang.org/x/crypto/bcrypt"
	"gorm.io/gorm"
)

func hashPassword(password string) (string, error) {
	bytes, err := bcrypt.GenerateFromPassword([]byte(password), bcrypt.DefaultCost)
	return string(bytes), err
}

// Create a user
func CreateUser(c *fiber.Ctx) error {
	db := database.DB.Db
	var user model.User
	if err := c.BodyParser(&user); err != nil {
		log.Printf("Error parsing body: %v", err)
		return c.Status(fiber.StatusBadRequest).JSON(fiber.Map{"status": "error", "message": "Invalid input", "data": err.Error()})
	}

	if user.Email == "" || user.Password == "" {
		return c.Status(fiber.StatusBadRequest).JSON(fiber.Map{"status": "error", "message": "Username and password are required"})
	}

	_, err := GetUserByEmail(user.Email)
	if err != nil {
		if err != gorm.ErrRecordNotFound {
			log.Printf("Error checking existing user: %v", err)
			return c.Status(fiber.StatusInternalServerError).JSON(fiber.Map{"status": "error", "message": "Internal server error", "data": err.Error()})
		}
	} else {
		return c.Status(fiber.StatusBadRequest).JSON(fiber.Map{"status": "error", "message": "User with same email already exists"})
	}

	hashedPassword, err := hashPassword(user.Password)
	if err != nil {
		log.Printf("Error hashing password: %v", err)
		return c.Status(fiber.StatusInternalServerError).JSON(fiber.Map{"status": "error", "message": "Internal server error", "data": err.Error()})
	}
	user.Password = hashedPassword

	if err := db.Create(&user).Error; err != nil {
		log.Printf("Error creating user: %v", err)
		return c.Status(fiber.StatusInternalServerError).JSON(fiber.Map{"status": "error", "message": "Could not create user", "data": err.Error()})
	}

	return c.Status(fiber.StatusCreated).JSON(fiber.Map{"status": "success", "message": "User created", "data": user.ToUserResponse()})
}

// GetSingleUser from db
func GetSingleUser(c *fiber.Ctx) error {
	email := c.Params("email")

	user, err := GetUserByEmail(email)

	if err != nil {
		if err == gorm.ErrRecordNotFound {
			return c.Status(fiber.StatusNotFound).JSON(fiber.Map{"status": "error", "message": "User not found"})
		}
		log.Printf("Error finding user: %v", err)
		return c.Status(fiber.StatusInternalServerError).JSON(fiber.Map{"status": "error", "message": "Internal server error", "data": err.Error()})
	}

	return c.Status(fiber.StatusOK).JSON(fiber.Map{"status": "success", "message": "User found", "data": user.ToUserResponse()})
}

func GetUserByEmail(email string) (model.User, error) {
	db := database.DB.Db
	var user model.User
	if err := db.Where("email = ?", email).First(&user).Error; err != nil {
		return user, err
	}
	return user, nil
}

// update a user in db
func UpdateUser(c *fiber.Ctx) error {
	db := database.DB.Db
	type updateUser struct {
		Username string `json:"username"`
	}

	var input updateUser
	if err := c.BodyParser(&input); err != nil {
		log.Printf("Error parsing body: %v", err)
		return c.Status(fiber.StatusBadRequest).JSON(fiber.Map{"status": "error", "message": "Invalid input", "data": err.Error()})
	}

	id := c.Params("id")
	var user model.User
	if err := db.First(&user, "id = ?", id).Error; err != nil {
		if err == gorm.ErrRecordNotFound {
			return c.Status(fiber.StatusNotFound).JSON(fiber.Map{"status": "error", "message": "User not found"})
		}
		log.Printf("Error finding user: %v", err)
		return c.Status(fiber.StatusInternalServerError).JSON(fiber.Map{"status": "error", "message": "Internal server error", "data": err.Error()})
	}

	user.Username = input.Username
	if err := db.Save(&user).Error; err != nil {
		log.Printf("Error updating user: %v", err)
		return c.Status(fiber.StatusInternalServerError).JSON(fiber.Map{"status": "error", "message": "Could not update user", "data": err.Error()})
	}

	return c.Status(fiber.StatusOK).JSON(fiber.Map{"status": "success", "message": "User updated", "data": user.ToUserResponse()})
}

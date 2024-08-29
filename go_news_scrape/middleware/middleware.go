package middleware

import (
	"encoding/base64"
	"strings"

	"golang.org/x/crypto/bcrypt"

	"github.com/gofiber/fiber/v2"
	"github.com/mkostynskyi0797/go_news_scrape/handler"
)

func checkPasswordHash(password, hash string) bool {
	err := bcrypt.CompareHashAndPassword([]byte(hash), []byte(password))
	return err == nil
}

func BasicAuthMiddleware(c *fiber.Ctx) error {
	auth := c.Get("Authorization")

	if auth == "" || !strings.HasPrefix(auth, "Basic ") {
		return c.Status(fiber.StatusUnauthorized).SendString("Unauthorized")
	}

	payload, err := base64.StdEncoding.DecodeString(auth[len("Basic "):])
	if err != nil {
		return c.Status(fiber.StatusUnauthorized).SendString("Unauthorized")
	}

	parts := strings.SplitN(string(payload), ":", 2)
	if len(parts) != 2 {
		return c.Status(fiber.StatusUnauthorized).SendString("Unauthorized")
	}

	email, password := parts[0], parts[1]

	user, err := handler.GetUserByEmail(email)
	if err != nil {
		return c.Status(fiber.StatusUnauthorized).SendString("Unauthorized")
	}

	if !checkPasswordHash(password, user.Password) {
		return c.Status(fiber.StatusUnauthorized).SendString("Unauthorized")
	}

	return c.Next()
}

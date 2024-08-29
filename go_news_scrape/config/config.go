package config

import (
	"log"
	"os"

	"github.com/joho/godotenv"
)

func init() {
	// Load .env file once during package initialization
	err := godotenv.Load(".env")
	if err != nil {
		log.Fatalf("Error loading .env file: %v", err)
	}
}

// Config func to get env value from key
func Config(key string) string {
	return os.Getenv(key)
}

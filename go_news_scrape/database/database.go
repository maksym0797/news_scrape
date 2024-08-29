package database

import (
	"fmt"
	"log"
	"strconv"

	"github.com/joho/godotenv"
	"gorm.io/driver/postgres"
	"gorm.io/gorm"
	"gorm.io/gorm/logger"

	"github.com/mkostynskyi0797/go_news_scrape/config"
	"github.com/mkostynskyi0797/go_news_scrape/model"
)

type DbInstance struct {
	Db *gorm.DB
}

var DB DbInstance

type Config struct {
	DBHost     string
	DBUser     string
	DBPassword string
	DBName     string
	DBPort     uint64
}

func loadConfig() (*Config, error) {
	err := godotenv.Load(".env")
	if err != nil {
		return nil, fmt.Errorf("error loading .env file: %w", err)
	}

	port, err := strconv.ParseUint(config.Config("DB_PORT"), 10, 32)
	if err != nil {
		return nil, fmt.Errorf("error parsing DB_PORT: %w", err)
	}

	return &Config{
		DBHost:     config.Config("DB_HOST"),
		DBUser:     config.Config("DB_USER"),
		DBPassword: config.Config("DB_PASSWORD"),
		DBName:     config.Config("DB_NAME"),
		DBPort:     port,
	}, nil
}

// Connect function
func Connect() {
	cfg, err := loadConfig()
	if err != nil {
		log.Fatalf("Failed to load configuration: %v", err)
	}

	dsn := fmt.Sprintf("host=%s user=%s password=%s dbname=%s port=%d sslmode=disable TimeZone=Asia/Shanghai",
		cfg.DBHost, cfg.DBUser, cfg.DBPassword, cfg.DBName, cfg.DBPort)

	db, err := gorm.Open(postgres.Open(dsn), &gorm.Config{
		Logger: logger.Default.LogMode(logger.Info),
	})
	if err != nil {
		log.Fatalf("Failed to connect to database: %v", err)
	}

	log.Println("Connected to the database")
	db.Logger = logger.Default.LogMode(logger.Info)
	log.Println("Running migrations")
	if err := db.AutoMigrate(&model.User{}); err != nil {
		log.Fatalf("Failed to run migrations: %v", err)
	}

	DB = DbInstance{
		Db: db,
	}
}

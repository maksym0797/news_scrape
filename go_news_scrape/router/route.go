package router

import (
	"github.com/gofiber/fiber/v2"
	"github.com/mkostynskyi0797/go_news_scrape/handler"
	"github.com/mkostynskyi0797/go_news_scrape/middleware"
)

// SetupRoutes func
func SetupRoutes(app *fiber.App) {
	// grouping
	api := app.Group("/api")
	v1 := api.Group("/user")
	// user routes
	v1.Get("/email/:email", middleware.BasicAuthMiddleware, handler.GetSingleUser)
	v1.Post("/signup", handler.CreateUser)
	v1.Put("/:id", middleware.BasicAuthMiddleware, handler.UpdateUser)
	v1.Get("/sources", middleware.BasicAuthMiddleware, handler.GetUserSources)

	sourceAPI := api.Group("/source")
	//user-source routes
	sourceAPI.Post("/", middleware.BasicAuthMiddleware, handler.AddUserSource)
	sourceAPI.Delete("/:sourceId", middleware.BasicAuthMiddleware, handler.RemoveUserSource)
	sourceAPI.Get("/types", handler.GetSourceTypes)

	postAPI := api.Group("/raw_post")

	postAPI.Get("/id/:id", handler.GetSinglePost)
}

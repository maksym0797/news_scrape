cmake_minimum_required(VERSION 3.24)
project(cpp_scrape_consumer LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)

# Define the executable
find_package(Sqlpp11 REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(Boost REQUIRED)
find_package(nlohmann_json REQUIRED)

# Include directories (for header files)
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/reader
    ${PROJECT_SOURCE_DIR}/src/config
    ${PROJECT_SOURCE_DIR}/src/http
    ${PROJECT_SOURCE_DIR}/src/db
)
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.h")


add_executable(main ${SOURCES})


# Include the reader sources in the executable
target_sources(main PRIVATE ${READER_SOURCES})
target_link_libraries(main PRIVATE sqlpp11::sqlpp11)
target_link_libraries(main PRIVATE PostgreSQL::PostgreSQL)
target_link_libraries(main PRIVATE boost::boost)
target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json)

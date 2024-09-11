cmake_minimum_required(VERSION 3.24)
project(cpp_scrape_consumer LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)

# Define the executable
find_package(Sqlpp11 REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(Boost REQUIRED)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE sqlpp11::sqlpp11)
target_link_libraries(main PRIVATE PostgreSQL::PostgreSQL)
target_link_libraries(main PRIVATE boost::boost)

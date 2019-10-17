#g++ -std=c++11 main.cpp -o arkanoid  -lsfml-graphics -lsfml-window -lsfml-system

cmake_minimum_required(VERSION 3.1)

project(asteroids)


## If you want to link SFML statically
set(SFML_STATIC_LIBRARIES FALSE)

## In most cases better set in the CMake cache
# set(SFML_DIR "<sfml root prefix>/lib/cmake/SFML")

include_directories(.)
find_package(SFML 2.5 COMPONENTS graphics audio window system REQUIRED)
add_executable(asteroids main.cpp)
target_link_libraries(asteroids sfml-graphics sfml-audio sfml-window sfml-system)
add_subdirectory(tests)

cmake_minimum_required(VERSION 3.0)

project("Platformer" CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Box2D REQUIRED)
find_package(SFML 2 REQUIRED COMPONENTS network audio graphics window system)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${SFML_INCLUDE_DIR}
)
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -O2
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${BOX2D_LIBRARY}
    ${SFML_LIBRARIES}
    tinyxml
)

install(DIRECTORY "Media" DESTINATION ${CMAKE_BINARY_DIR})

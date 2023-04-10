cmake_minimum_required(VERSION 3.9)
project(skeleton)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -ggdb)

file(GLOB TARGET_SRC "./src/*.cpp" )
include_directories("./include")

# Setting up SFML
if (NOT LINUX AND WIN32)
    message(STATUS "Building for Windows!")
    set(SFML_DIR "c:/SFML/lib/cmake/SFML")
    set(SFML_DOC_DIR "C:/SFML/doc")
else()
    message(STATUS "Building for Linux!")
endif()

find_package(SFML 2.5.1 COMPONENTS audio graphics window system REQUIRED)
include_directories(${SFML_INCLUDES})

# Linking executable
add_executable(main ${TARGET_SRC})

target_link_libraries(main PRIVATE sfml-audio sfml-graphics sfml-window sfml-system ${CMAKE_DL_LIBS})


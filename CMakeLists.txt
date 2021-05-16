cmake_minimum_required(VERSION 3.14)

project(vrender)

set(CMAKE_CXX_STANDARD 17)
set(BINARY_NAME vrender)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(third_party)

if(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

    include_directories(${GTK3_INCLUDE_DIRS})
    link_directories(${GTK3_LIBRARY_DIRS})
    add_definitions(${GTK3_CFLAGS_OTHER})
endif(UNIX)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

include_directories("$ENV{VULKAN_SDK}/include")
link_directories("$ENV{VULKAN_SDK}/lib") 
link_directories("$ENV{VULKAN_SDK}/etc/explicit_layer.d")

find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})

include_directories("${PROJECT_SOURCE_DIR}/src")

message(${PROJECT_SOURCE_DIR})

find_package(Vulkan REQUIRED)

add_executable(${BINARY_NAME} ${HEADERS} ${SOURCES} ${LOGGING} ${LOGGING_H})

target_link_libraries(${BINARY_NAME} glfw)
target_link_libraries(${BINARY_NAME} vulkan)


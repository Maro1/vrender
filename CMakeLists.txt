cmake_minimum_required(VERSION 3.14)

project(vrender)

set(CMAKE_CXX_STANDARD 17)
set(BINARY_NAME vrender)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

include_directories("${PROJECT_SOURCE_DIR}/src")

find_package(Vulkan REQUIRED)

add_executable(${BINARY_NAME} ${HEADERS} ${SOURCES} ${LOGGING} ${LOGGING_H})

# glm
add_library(glm INTERFACE)
set(GLM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm)
target_sources(glm INTERFACE ${GLM_DIR}/glm/glm.hpp)
target_include_directories(glm INTERFACE ${GLM_DIR})

target_compile_definitions(glm INTERFACE
    GLM_FORCE_SWIZZLE
    GLM_FORCE_RADIANS
    GLM_FORCE_CTOR_INIT
    GLM_ENABLE_EXPERIMENTAL
)
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_definitions(glm INTERFACE GLM_FORCE_CXX14)
endif()

# spdlog
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/spdlog)

# GLFW
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_INSTALL OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw)
target_link_libraries(${BINARY_NAME} glm)
target_link_libraries(${BINARY_NAME} spdlog)
target_link_libraries(${BINARY_NAME} glfw)
target_link_libraries(${BINARY_NAME} vulkan)

# Compile shaders
find_program(GLSLC glslc)

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "src/shaders/*.frag"
    "src/shaders/*.vert"
    )

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "shader_bin/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "shader_bin/"
    COMMAND ${GLSLC} ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSLC})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
    )

add_dependencies(vrender Shaders)

add_custom_command(TARGET vrender POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:vrender>/shader_bin"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "shader_bin"
        "$<TARGET_FILE_DIR:vrender>/shader_bin"
        )


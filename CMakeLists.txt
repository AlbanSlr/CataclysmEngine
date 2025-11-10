cmake_minimum_required(VERSION 3.16)
include(FetchContent)

add_compile_definitions(MODEL_DIR="${CMAKE_CURRENT_SOURCE_DIR}/models/")
add_compile_definitions(SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders/")

set(NAME CataclysmEngine)

message(STATUS "using ${CMAKE_GENERATOR}")

project(${NAME} VERSION 0.0.1)

# Create the executable target first
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

# glm library
find_package(glm CONFIG QUIET) # Try to find glm via cmake first

if(NOT glm_FOUND) # If not found, download it
	FetchContent_Declare(
		glm
		GIT_REPOSITORY	https://github.com/g-truc/glm.git
		GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
	)

	FetchContent_MakeAvailable(glm)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm) # Link glm to your target

# Simple DirectMedia Layer (SDL3) library
find_package(SDL3 CONFIG QUIET) # Try to find SDL3 via cmake first
if(NOT SDL3_FOUND) # If not found, download it
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        release-3.2.0 #refs/tags/release-3.2.0
    )

    FetchContent_MakeAvailable(SDL3)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3) # Link SDL3 to your target

# Vulkan library
find_package(Vulkan REQUIRED)
if(NOT Vulkan_FOUND) # If not found, show error
    message(FATAL_ERROR "Vulkan SDK not found. Please install it from https://vulkan.lunarg.com/sdk/home")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan) # Link Vulkan to your target

# TinyObjLoader
if (NOT DEFINED TINYOBJ_PATH)
  set(TINYOBJ_PATH "${CMAKE_SOURCE_DIR}/external/tinyobjloader")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${TINYOBJ_PATH}
)

# Dear ImGui library
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.3
)

FetchContent_MakeAvailable(imgui)

# Create ImGui library target
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)

target_include_directories(imgui PUBLIC 
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC SDL3::SDL3 Vulkan::Vulkan)

target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

############## Build SHADERS #######################

# Find all vertex and fragment sources within shaders directory
find_program(GLSL_VALIDATOR glslangValidator HINTS 
  ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} 
  /usr/bin 
  /usr/local/bin 
  ${VULKAN_SDK_PATH}/Bin
  ${VULKAN_SDK_PATH}/Bin32
  $ENV{VULKAN_SDK}/Bin/ 
  $ENV{VULKAN_SDK}/Bin32/
)

# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/shaders/*.frag"
  "${PROJECT_SOURCE_DIR}/shaders/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)
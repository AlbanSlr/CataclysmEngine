cmake_minimum_required(VERSION 3.6...3.28 FATAL_ERROR)
cmake_policy(VERSION 3.6)

if(APPLE)
  include(./envMac.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
  message(STATUS "Using envMac.cmake : ${LOCAL_ENV}")
elseif(WIN32)
  include(./envWindows.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
  message(STATUS "Using envWindows.cmake : ${LOCAL_ENV}")
else()
  message(FATAL_ERROR "Unsupported platform")
endif()


set(NAME CataclysmEngine)

message(STATUS "using ${CMAKE_GENERATOR}")

project(${NAME} VERSION 0.0.1)

# 1. Set VULKAN_SDK_PATH in .env.cmake to target specific vulkan version
if (DEFINED VULKAN_SDK_PATH)
  set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include")
  set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib")
else()
  find_package(Vulkan REQUIRED) # throws error if could not find Vulkan
endif()
message(STATUS "Using vulkan include at: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")


# 2. set GLFW_INCLUDE_DIRS and GLFW_LIB based on GLFW_PATH in .env.cmake
if (DEFINED GLFW_PATH)
  message(STATUS "GLFW_PATH: ${GLFW_PATH}")
  set(GLFW_INCLUDE_DIRS "${GLFW_PATH}/include")
  message(STATUS "GLFW_INCLUDE_DIRS: ${GLFW_INCLUDE_DIRS}")
  if (MSVC)
    set(GLFW_LIB "${GLFW_PATH}/lib-vc2022") # using visual studio 2022
  else()
    set(GLFW_LIB "${GLFW_PATH}/build/src/libglfw.dylib")
    endif()
  message(STATUS "Found GLFW at: ${GLFW_PATH}")
  message(STATUS "Using GLFW include at: ${GLFW_INCLUDE_DIRS}")
  message(STATUS "Using GLFW lib at: ${GLFW_LIB}")
else()
  message(FATAL_ERROR "GLFW_PATH not set in .env.cmake")
endif()

include_directories(external)

# 3. check if GLM_PATH is set in .env.cmake
if (NOT TINYOBJ_PATH)
  message(FATAL_ERROR "TINYOBJ_PATH not set in .env.cmake")
endif()

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

if (WIN32)
  message(STATUS "CREATING BUILD FOR WINDOWS")

  if (USE_MINGW)
    target_include_directories(${PROJECT_NAME} PUBLIC
      ${MINGW_PATH}/include
    )
    target_link_directories(${PROJECT_NAME} PUBLIC
      ${MINGW_PATH}/lib
    )
  endif()

  target_include_directories(${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${Vulkan_INCLUDE_DIRS}
    ${TINYOBJ_PATH}
    ${GLFW_INCLUDE_DIRS}
    ${GLM_PATH}
    )

  target_link_directories(${PROJECT_NAME} PUBLIC
    ${Vulkan_LIBRARIES}
    ${GLFW_LIB}
  )

  target_link_libraries(${PROJECT_NAME} glfw3 vulkan-1)
elseif (UNIX)
    message(STATUS "CREATING BUILD FOR UNIX")
    target_include_directories(${PROJECT_NAME} PUBLIC
      ${PROJECT_SOURCE_DIR}/src
      ${Vulkan_INCLUDE_DIRS}
      ${TINYOBJ_PATH}
      ${GLFW_INCLUDE_DIRS}
    )
    target_link_libraries(${PROJECT_NAME} ${GLFW_LIB} ${Vulkan_LIBRARIES})
endif()


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
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)
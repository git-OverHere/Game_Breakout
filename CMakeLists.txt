# Minimum version of cmake for this to run
cmake_minimum_required(VERSION 3.10)

# Define variables useing set()
# Use using ${var}
set(APP_NAME Breakout)
set(EXE_NAME Breakout)

# Define project name
project(${APP_NAME} C CXX)

# Add all source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

# Create the executable file
add_executable(${EXE_NAME} ${SOURCES})

# Add condition if the build is for windows
if (WIN32)
    # Add resource to windows executable
    # In this case, the icon
    target_sources(${EXE_NAME} PRIVATE "src/app.rc")
endif()

# Include external libraries (e.g. glad, GLFW ...)
target_include_directories(${EXE_NAME} PRIVATE external)

# Handles linking GLFW to the project statically
target_compile_definitions(${EXE_NAME} PRIVATE GLFW_STATIC)
target_link_libraries(${EXE_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/external/GLFW/lib/libglfw3.a")

# Include my own headers
target_include_directories(${EXE_NAME} PRIVATE include)

# Copy assets/ folder
add_custom_command(TARGET ${EXE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${EXE_NAME}>/assets

    # # Remove shader asset
    # COMMAND ${CMAKE_COMMAND} -E remove_directory
    # $<TARGET_FILE_DIR:${EXE_NAME}>/assets/shaders
)

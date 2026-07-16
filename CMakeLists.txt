cmake_minimum_required(VERSION 3.15)
project(SoftwareRasterizer LANGUAGES CXX)

# Set Modern C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Route compiled binaries to a clean 'bin' folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 1. Gather files from the subdirectories INSIDE the inner folder
file(GLOB_RECURSE FOLDER_SOURCES 
    "SoftwareRasterizer/helper/*.cpp" "SoftwareRasterizer/helper/*.h" "SoftwareRasterizer/helper/*.hpp"
    "SoftwareRasterizer/math/*.cpp" "SoftwareRasterizer/math/*.h" "SoftwareRasterizer/math/*.hpp"
    "SoftwareRasterizer/Renderer/*.cpp" "SoftwareRasterizer/Renderer/*.h" "SoftwareRasterizer/Renderer/*.hpp"
    "SoftwareRasterizer/Thread/*.cpp" "SoftwareRasterizer/Thread/*.h" "SoftwareRasterizer/Thread/*.hpp"
    "SoftwareRasterizer/Window/*.cpp" "SoftwareRasterizer/Window/*.h" "SoftwareRasterizer/Window/*.hpp"
)

# 2. Explicitly add the files sitting directly in the inner folder
file(GLOB ROOT_SOURCES
    "SoftwareRasterizer/*.cpp" 
    "SoftwareRasterizer/*.h" 
    "SoftwareRasterizer/*.hpp"
)

# Combine them into one source list
set(SOURCES ${FOLDER_SOURCES} ${ROOT_SOURCES})

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Tell the compiler where to look for #include headers
# Pointing to the inner folder ensures #include "DataStructures.h" works
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/SoftwareRasterizer
)

# Handle Windows-specific definitions
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
        _CRT_SECURE_NO_WARNINGS 
        WIN32_LEAN_AND_MEAN
        UNICODE     # <--- ADD THIS
        _UNICODE    # <--- AND THIS
    )
endif()

# Copy the 'assets' folder from the inner folder to the build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/SoftwareRasterizer/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
)
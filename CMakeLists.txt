cmake_minimum_required(VERSION 3.15)
project(SoftwareRasterizer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Gather your engine source files (everything except main.cpp)
file(GLOB_RECURSE RASTERIZER_SOURCES 
    "SoftwareRasterizer/helper/*.cpp" "SoftwareRasterizer/helper/*.h"
    "SoftwareRasterizer/math/*.cpp" "SoftwareRasterizer/math/*.h"
    "SoftwareRasterizer/Renderer/*.cpp" "SoftwareRasterizer/Renderer/*.h"
    "SoftwareRasterizer/Thread/*.cpp" "SoftwareRasterizer/Thread/*.h"
    "SoftwareRasterizer/Window/*.cpp" "SoftwareRasterizer/Window/*.h"
    "SoftwareRasterizer/FrameBuffer.cpp" "SoftwareRasterizer/FrameBuffer.h"
)

# Build the engine as a STATIC LIBRARY, not an executable
add_library(SoftwareRasterizerLib STATIC ${RASTERIZER_SOURCES})

target_include_directories(SoftwareRasterizerLib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/SoftwareRasterizer
)

if(WIN32)
    target_compile_definitions(SoftwareRasterizerLib PUBLIC 
        _CRT_SECURE_NO_WARNINGS WIN32_LEAN_AND_MEAN UNICODE _UNICODE
    )
endif()

# Keep your own standalone demo executable too, if this file is built standalone
add_executable(SoftwareRasterizer SoftwareRasterizer/main.cpp)
target_link_libraries(SoftwareRasterizer PRIVATE SoftwareRasterizerLib)
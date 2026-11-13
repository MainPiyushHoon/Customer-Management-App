cmake_minimum_required(VERSION 3.20)
project(BrutalOps VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler optimization and warnings
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Directories
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

include_directories(${INCLUDE_DIR} ${THIRD_PARTY_DIR})

# Source definitions
file(GLOB_RECURSE APP_SOURCES 
    "${SRC_DIR}/*.cpp"
)

file(GLOB_RECURSE APP_HEADERS 
    "${INCLUDE_DIR}/*.hpp"
)

# Executable Target
add_executable(BrutalOps
    ${APP_SOURCES}
    ${APP_HEADERS}
)

target_include_directories(BrutalOps PUBLIC 
    ${INCLUDE_DIR}
    ${THIRD_PARTY_DIR}
)

# Platform Libraries (Windows MinGW / MSYS2)
if(WIN32)
    target_link_libraries(BrutalOps PRIVATE
        opengl32
        gdi32
        imm32
        dwmapi
        comdlg32
    )
endif()

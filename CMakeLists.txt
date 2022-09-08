cmake_minimum_required(VERSION 3.13)

project(LIBTOML VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED OFF)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

aux_source_directory(${PROJECT_SOURCE_DIR}/src TOML_IMPL_SRC)

add_library(toml STATIC "")

target_sources(toml
PRIVATE
    ${TOML_IMPL_SRC}
)

target_include_directories(toml
PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}
)

target_compile_options(toml
PRIVATE
    -Wall
    -g
)

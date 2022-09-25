cmake_minimum_required(VERSION 3.13)

project(LIBTOML VERSION 0.0.1 LANGUAGES C CXX)

# This project can use C11, but will gracefully decay down to C89.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED OFF)
set(CMAKE_C_EXTENSIONS OFF)

# This project requires C++11.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(LIBTOML_BUILD_TESTS "Build libtoml's unit tests" ON)

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


# unit test
if(LIBTOML_BUILD_TESTS)

    # using google test source library
    add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/googletest-1.10.0 googletest)

    enable_testing()

    function(libtoml_unit_test test_file)
    get_filename_component(test_target_name "${test_file}" NAME_WE)

    add_executable("${test_target_name}" "")
    target_sources("${test_target_name}"
    PRIVATE
        "${test_file}"
    )

    target_include_directories("${test_target_name}"
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/third_party/googletest-1.10.0/googletest/include
    )

    target_compile_options("${test_target_name}"
    PRIVATE
        -fno-access-control
    )

    target_link_libraries("${test_target_name}"
    PRIVATE
        toml
        gtest
    )

    target_compile_definitions("${test_target_name}"
    PRIVATE
        TEST_CASE_DIR="${PROJECT_SOURCE_DIR}/etc"
    )

    add_test(NAME "${test_target_name}" COMMAND "${test_target_name}" "${PROJECT_SOURCE_DIR}")
    endfunction(libtoml_unit_test)

    libtoml_unit_test("${PROJECT_SOURCE_DIR}/test/array_test.cc")
    libtoml_unit_test("${PROJECT_SOURCE_DIR}/test/boolean_test.cc")
    libtoml_unit_test("${PROJECT_SOURCE_DIR}/test/comment_test.cc")
    libtoml_unit_test("${PROJECT_SOURCE_DIR}/test/datetime_test.cc")
    libtoml_unit_test("${PROJECT_SOURCE_DIR}/test/float_test.cc")
    libtoml_unit_test("${PROJECT_SOURCE_DIR}/test/inline_table_test.cc")
    libtoml_unit_test("${PROJECT_SOURCE_DIR}/test/integer_test.cc")
    libtoml_unit_test("${PROJECT_SOURCE_DIR}/test/keys_test.cc")
    libtoml_unit_test("${PROJECT_SOURCE_DIR}/test/kvpair_test.cc")
    libtoml_unit_test("${PROJECT_SOURCE_DIR}/test/string_test.cc")
    libtoml_unit_test("${PROJECT_SOURCE_DIR}/test/table_test.cc")

endif(LIBTOML_BUILD_TESTS)

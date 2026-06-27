cmake_minimum_required(VERSION 3.14)

project(LIBTOML VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(LIBTOML_BUILD_TESTS "Build libtoml's unit tests" ON)
option(LIBTOML_BUILD_EXAMPLE "Build libtoml's example" ON)

# 显式源文件列表
set(TOML_SOURCES
    src/value.cc
    src/table.cc
    src/array.cc
    src/datetime.cc
    src/error.cc
    src/lexer.cc
    src/parser.cc
    src/writer.cc
    src/utf8.cc
)

add_library(toml STATIC ${TOML_SOURCES})

target_include_directories(toml
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${PROJECT_SOURCE_DIR}
)

# 跨平台编译选项
if(MSVC)
    target_compile_options(toml PRIVATE /W4 /WX /utf-8)
else()
    target_compile_options(toml PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install 规则
include(GNUInstallDirs)

install(TARGETS toml
    EXPORT libtoml-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/toml
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT libtoml-targets
    FILE libtoml-config.cmake
    NAMESPACE libtoml::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libtoml
)

# 单元测试
if(LIBTOML_BUILD_TESTS)
    enable_testing()

    # 自定义轻量测试框架
    set(TEST_UTIL_SRC test/util/testutil.cc)

    function(libtoml_unit_test test_file)
        get_filename_component(test_target_name "${test_file}" NAME_WE)

        add_executable(${test_target_name} ${test_file} ${TEST_UTIL_SRC})

        target_include_directories(${test_target_name}
            PRIVATE
                ${PROJECT_SOURCE_DIR}/include
                ${PROJECT_SOURCE_DIR}
        )

        target_compile_options(${test_target_name} PRIVATE
            $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fno-access-control>
        )

        target_link_libraries(${test_target_name}
            PRIVATE toml
        )

        target_compile_definitions(${test_target_name}
            PRIVATE TEST_CASE_DIR="${PROJECT_SOURCE_DIR}/etc"
        )

        add_test(NAME ${test_target_name} COMMAND ${test_target_name})
    endfunction()

    # 类型系统测试
    libtoml_unit_test(test/test_value.cc)
    libtoml_unit_test(test/test_table.cc)
    libtoml_unit_test(test/test_array.cc)
    libtoml_unit_test(test/test_datetime.cc)
    libtoml_unit_test(test/test_writer.cc)
    libtoml_unit_test(test/test_error.cc)

    # 解析测试
    libtoml_unit_test(test/array_of_table_test.cc)
    libtoml_unit_test(test/array_test.cc)
    libtoml_unit_test(test/boolean_test.cc)
    libtoml_unit_test(test/comment_test.cc)
    libtoml_unit_test(test/datetime_test.cc)
    libtoml_unit_test(test/float_test.cc)
    libtoml_unit_test(test/inline_array_test.cc)
    libtoml_unit_test(test/inline_table_test.cc)
    libtoml_unit_test(test/integer_test.cc)
    libtoml_unit_test(test/keys_test.cc)
    libtoml_unit_test(test/kvpair_test.cc)
    libtoml_unit_test(test/string_test.cc)
    libtoml_unit_test(test/table_test.cc)
endif()

# 示例
if(LIBTOML_BUILD_EXAMPLE)
    add_executable(sample example/sample.cc)
    target_include_directories(sample PRIVATE ${PROJECT_SOURCE_DIR}/include)
    target_link_libraries(sample PRIVATE toml)
endif()

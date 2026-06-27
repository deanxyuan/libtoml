# LIBTOML

A TOML v0.5.0 reader and writer in C++14.

## Features

- Full TOML v0.5.0 specification support
- All data types: strings, integers, unsigned integers, floats, booleans, datetimes, arrays, tables
- Inline tables and inline arrays
- Array of tables (`[[...]]`)
- Dotted keys (`a.b.c = value`)
- Multiline strings (basic and literal)
- Comments
- Date, time, and datetime parsing with timezone offset
- TOML and JSON serialization output
- Zero external dependencies
- Static library, C++14

## Requirements

- C++14 compatible compiler (GCC, Clang, MSVC)
- CMake 3.14+

## Building

```bash
cd libtoml && mkdir build && cd build
cmake ..
cmake --build . --config Release --parallel 2
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `LIBTOML_BUILD_TESTS` | ON | Build unit tests |
| `LIBTOML_BUILD_EXAMPLE` | ON | Build example |

### Quick Build with Tests

```bash
cmake .. -DLIBTOML_BUILD_TESTS=ON
cmake --build . --config RelWithDebInfo
ctest
```

## Integration

### CMake (subdirectory)

```cmake
add_subdirectory(libtoml)
target_link_libraries(my_app PRIVATE toml)
```

## Usage

### Parse a String

```cpp
#include <toml/toml.h>

auto result = TOML::parse_string(text, strlen(text));
if (!result.ok()) {
    std::cerr << result.error.to_string() << std::endl;
    return 1;
}
const auto& root = result.value;
```

### Parse a File

```cpp
auto result = TOML::parse_file("config.toml");
if (!result.ok()) {
    std::cerr << result.error.to_string() << std::endl;
    return 1;
}
```

### Access Values

```cpp
const auto& root = result.value;

// table access
const auto& server = root.as_table().at("server");
if (server.as_table().contains("host")) {
    const auto& host = server.as_table().at("host");
    if (host.is_string()) {
        std::string s = host.as_string();
    }
}

// array access
const auto& port = server.as_table().at("port");
if (port.is_array()) {
    for (size_t i = 0; i < port.as_array().size(); i++) {
        int64_t val = port.as_array().at(i).as_integer();
    }
}
```

### Serialize

```cpp
std::string toml = root.to_toml();   // TOML format
std::string json = root.to_json();   // JSON format
```

## API Reference

### Parsing Functions

| Function | Description |
|----------|-------------|
| `TOML::parse_file(path)` | Parse a TOML file, returns `ParseResult` |
| `TOML::parse_string(data, len)` | Parse a TOML string with length |
| `TOML::parse_string(str)` | Parse a `std::string` |

### ParseResult

| Member | Type | Description |
|--------|------|-------------|
| `value` | `Value` | Parsed root value |
| `error` | `Error` | Error info (empty message = success) |
| `ok()` | `bool` | `true` if parsing succeeded |

### Error

| Member | Type | Description |
|--------|------|-------------|
| `location` | `SourceLocation` | `{line, column}` of the error |
| `message` | `std::string` | Error description |
| `ok()` | `bool` | `true` if no error |
| `to_string()` | `std::string` | Formatted error string |

### Value

| Method | Description |
|--------|-------------|
| `type()` | Returns `TOML::Type` enum |
| `is_null()` / `is_boolean()` / `is_integer()` / `is_float()` / `is_string()` / `is_datetime()` / `is_table()` / `is_array()` | Type checks |
| `as_bool()` / `as_integer()` / `as_uinteger()` / `as_float()` / `as_string()` / `as_datetime()` / `as_table()` / `as_array()` | Type-safe access (throws `bad_value_access` on mismatch) |
| `to_toml()` | Serialize to TOML string |
| `to_json()` | Serialize to JSON string |
| `operator bool()` | `true` if not null |

### Table

| Method | Description |
|--------|-------------|
| `at(key)` | Access value by key (throws if missing) |
| `contains(key)` | Check if key exists |
| `count(key)` | Returns 1 if key exists, 0 otherwise |
| `insert(key, value)` | Insert a key-value pair |
| `erase(key)` | Remove a key |
| `size()` / `empty()` | Size info |
| `begin()` / `end()` | Iterator support |

### Array

| Method | Description |
|--------|-------------|
| `at(index)` | Access element by index |
| `push_back(value)` | Append an element |
| `size()` / `empty()` | Size info |
| `begin()` / `end()` | Iterator support |

### Types Enum

`TOML::Type` values: `kNull`, `kBoolean`, `kInteger`, `kUInteger`, `kFloat`, `kString`, `kDateTime`, `kTable`, `kArray`.

### Utility

| Function | Description |
|----------|-------------|
| `TOML::type_name(type)` | Get type name as string |

## Testing

```bash
cd build
ctest
```

Individual test executables:

| Test | Description |
|------|-------------|
| `test_value` | Value type system |
| `test_table` | Table type operations |
| `test_array` | Array type operations |
| `test_datetime` | DateTime type |
| `test_writer` | TOML/JSON serialization |
| `test_error` | Error handling |
| `test_parse_array` | Array parsing |
| `test_parse_array_of_table` | Array of tables parsing |
| `test_parse_boolean` | Boolean parsing |
| `test_parse_comment` | Comment handling |
| `test_parse_datetime` | Datetime parsing |
| `test_parse_float` | Float parsing |
| `test_parse_inline_array` | Inline array parsing |
| `test_parse_inline_table` | Inline table parsing |
| `test_parse_integer` | Integer parsing |
| `test_parse_keys` | Key parsing |
| `test_parse_kvpair` | Key-value pair parsing |
| `test_parse_string` | String parsing |
| `test_parse_table` | Table parsing |

## License

Apache License 2.0 -- see [LICENSE](LICENSE)

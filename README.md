# LIBTOML v2.0.0

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
- Configurable formatting (indent size, sorted keys)
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
| `LIBTOML_BUILD_EXAMPLES` | ON | Build examples |

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

### Parse and Read

```cpp
#include <toml/toml.h>
#include <iostream>

auto result = TOML::parse_file("config.toml");
if (!result.ok()) {
    // error location (line, column) for debugging
    std::cerr << result.error.to_string() << std::endl;
    return 1;
}

const auto& root = result.value;
const auto& server = root.as_table().at("server").as_table();
std::string host = server.at("host").as_string();
int64_t port = server.at("port").as_integer();
```

### Parse from String

```cpp
const char* text = R"(
[database]
host = "localhost"
port = 5432
)";

auto result = TOML::parse_string(text, strlen(text));
```

### Access Nested Values

```cpp
// array of tables
const auto& items = root.as_table().at("items").as_array();
for (size_t i = 0; i < items.size(); i++) {
    const auto& item = items.at(i).as_table();
    std::cout << item.at("name").as_string() << std::endl;
}
```

### Build and Serialize

```cpp
#include <toml/toml.h>
#include <iostream>

// build a Value tree programmatically
TOML::Table server;
server.insert("host", "example.com");
server.insert("port", static_cast<int64_t>(8080));

TOML::Table opts;
opts.insert("timeout", 30.5);
opts.insert("retry", static_cast<int64_t>(3));
server.insert("options", std::move(opts));

TOML::Table root;
root.insert("server", std::move(server));

// serialize to TOML (default)
std::cout << root.to_toml() << std::endl;

// serialize with format options
TOML::FormatOptions fmt;
fmt.indent = 2;
fmt.sorted_keys = true;
std::cout << TOML::to_toml(TOML::Value(root), fmt) << std::endl;

// serialize to JSON
std::cout << root.to_json() << std::endl;
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
| `to_string()` | `std::string` | Formatted error string with location |

### Value

| Method | Description |
|--------|-------------|
| `type()` | Returns `TOML::Type` enum |
| `is_null()` / `is_boolean()` / `is_integer()` / `is_uinteger()` / `is_float()` / `is_string()` / `is_datetime()` / `is_table()` / `is_array()` | Type checks |
| `as_bool()` / `as_integer()` / `as_uinteger()` / `as_float()` / `as_string()` / `as_datetime()` / `as_table()` / `as_array()` | Type-safe access (throws `bad_value_access` on mismatch) |
| `as_table_mut()` / `as_array_mut()` | Mutable access |
| `to_toml()` | Serialize to TOML string |
| `to_json()` | Serialize to JSON string |
| `operator bool()` | `true` if not null |

### Table

| Method | Description |
|--------|-------------|
| `at(key)` | Access value by key (throws if missing) |
| `operator[](key)` | Access or insert value |
| `contains(key)` | Check if key exists |
| `count(key)` | Returns 1 if key exists, 0 otherwise |
| `insert(key, value)` | Insert a key-value pair (fails if key exists) |
| `insert_or_assign(key, value)` | Insert or overwrite |
| `erase(key)` | Remove a key |
| `size()` / `empty()` | Size info |
| `begin()` / `end()` | Iterator support |
| `to_toml()` | Serialize table to TOML string |
| `to_json()` | Serialize table to JSON string |
| `is_inline()` / `set_inline(v)` | Inline table flag |

### Array

| Method | Description |
|--------|-------------|
| `at(index)` | Access element by index |
| `operator[](index)` | Access element by index |
| `push_back(value)` | Append an element |
| `insert(index, value)` | Insert at position |
| `erase(index)` | Remove element at position |
| `pop_back()` | Remove last element |
| `front()` / `back()` | Access first / last element |
| `size()` / `empty()` | Size info |
| `begin()` / `end()` | Iterator support |
| `to_toml()` | Serialize array to TOML string |
| `to_json()` | Serialize array to JSON string |

### FormatOptions

| Member | Type | Default | Description |
|--------|------|---------|-------------|
| `indent` | `size_t` | `4` | Indent size |
| `indent_char` | `char` | `' '` | Indent character |
| `sorted_keys` | `bool` | `false` | Sort keys alphabetically |

```cpp
TOML::FormatOptions fmt;
fmt.indent = 2;
fmt.sorted_keys = true;
std::string s = TOML::to_toml(TOML::Value(table), fmt);
```

### Serialization Functions

| Function | Description |
|----------|-------------|
| `value.to_toml()` | Serialize Value to TOML string |
| `value.to_json()` | Serialize Value to JSON string |
| `table.to_toml()` | Serialize Table to TOML string |
| `table.to_json()` | Serialize Table to JSON string |
| `TOML::to_toml(value)` | Free function, same as `value.to_toml()` |
| `TOML::to_toml(value, opts)` | Serialize with format options |
| `TOML::to_json(value)` | Free function, same as `value.to_json()` |

### Types Enum

`TOML::Type` values: `kNull`, `kBoolean`, `kInteger`, `kUInteger`, `kFloat`, `kString`, `kDateTime`, `kTable`, `kArray`.

### Utility

| Function | Description |
|----------|-------------|
| `TOML::type_name(type)` | Get type name as string |

## Examples

See the `examples/` directory:

- `reader.cc` — Parse TOML, access values, type checking
- `writer.cc` — Build Value tree, serialize to TOML/JSON, format options

```bash
cmake --build . --target reader writer
./reader
./writer
```

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

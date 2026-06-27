#include <iostream>

#include <toml/toml.h>

int main() {
    // 1. Build a Value tree programmatically
    TOML::Table server;
    server.insert("host", "example.com");
    server.insert("port", static_cast<int64_t>(8080));
    server.insert("enabled", true);

    TOML::Table opts;
    opts.insert("timeout", 30.5);
    opts.insert("retry", static_cast<int64_t>(3));
    server.insert("options", std::move(opts));

    TOML::Table w1;
    w1.insert("name", "alpha");
    w1.insert("cpu", static_cast<int64_t>(4));
    TOML::Table w2;
    w2.insert("name", "beta");
    w2.insert("cpu", static_cast<int64_t>(8));

    TOML::Array workers;
    workers.push_back(std::move(w1));
    workers.push_back(std::move(w2));

    TOML::Table root;
    root.insert("server", std::move(server));
    root.insert("workers", std::move(workers));

    // 2. Serialize to TOML
    std::cout << "=== TOML ===" << std::endl;
    std::cout << root.to_toml() << std::endl;

    // 3. TOML output with format options
    TOML::FormatOptions fmt;
    fmt.indent = 2;
    fmt.sorted_keys = true;
    std::cout << "=== TOML (sorted, 2-space indent) ===" << std::endl;
    std::cout << TOML::to_toml(TOML::Value(root), fmt) << std::endl;

    // 4. Serialize to JSON
    std::cout << "=== JSON ===" << std::endl;
    std::cout << root.to_json() << std::endl;

    return 0;
}

#include <cstring>
#include <iostream>

#include <toml/toml.h>

int main() {
    // 1. Parse from string
    const char* text = R"(
[server]
host = "example.com"
port = 8080
enabled = true

[server.options]
timeout = 30.5
retry = 3

[[workers]]
name = "alpha"
cpu = 4

[[workers]]
name = "beta"
cpu = 8
)";

    auto result = TOML::parse_string(text, strlen(text));
    if (!result.ok()) {
        std::cerr << "parse error: " << result.error.to_string() << std::endl;
        return 1;
    }

    const auto& root = result.value;

    // 2. Read scalar values
    const auto& server = root.as_table().at("server").as_table();
    std::cout << "host:    " << server.at("host").as_string() << std::endl;
    std::cout << "port:    " << server.at("port").as_integer() << std::endl;
    std::cout << "enabled: " << (server.at("enabled").as_bool() ? "true" : "false") << std::endl;

    // 3. Read nested table
    const auto& opts = server.at("options").as_table();
    std::cout << "timeout: " << opts.at("timeout").as_float() << std::endl;
    std::cout << "retry:   " << opts.at("retry").as_integer() << std::endl;

    // 4. Read array of tables
    const auto& workers = root.as_table().at("workers").as_array();
    std::cout << "\nworkers (" << workers.size() << "):" << std::endl;
    for (size_t i = 0; i < workers.size(); i++) {
        const auto& w = workers.at(i).as_table();
        std::cout << "  [" << i << "] "
                  << w.at("name").as_string()
                  << " (cpu=" << w.at("cpu").as_integer() << ")"
                  << std::endl;
    }

    // 5. Type checking
    std::cout << "\n--- type info ---" << std::endl;
    std::cout << "host type:    " << TOML::type_name(server.at("host").type()) << std::endl;
    std::cout << "port type:    " << TOML::type_name(server.at("port").type()) << std::endl;
    std::cout << "enabled type: " << TOML::type_name(server.at("enabled").type()) << std::endl;

    return 0;
}

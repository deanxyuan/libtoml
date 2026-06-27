#include <cstring>
#include <iostream>

#include <toml/toml.h>

int main() {
    const char* text =
        R"([server]
    host = "example.com"
    port = [ 8080, 8181, 8282 ]
)";

    // parse TOML string
    auto result = TOML::parse_string(text, strlen(text));
    if (!result.ok()) {
        std::cerr << "parse error: " << result.error.to_string() << std::endl;
        return 1;
    }

    const auto& root = result.value;

    // check key exists and access table
    if (!root.is_table() || !root.as_table().contains("server")) {
        std::cerr << "server section not found" << std::endl;
        return 1;
    }

    const auto& server = root.as_table().at("server");

    // get host
    if (server.as_table().contains("host")) {
        const auto& host = server.as_table().at("host");
        std::cout << "host type: " << TOML::type_name(host.type()) << std::endl;
        if (host.is_string()) {
            std::cout << "host value: " << host.as_string() << std::endl;
        }
    }

    // get port array
    if (server.as_table().contains("port")) {
        const auto& port = server.as_table().at("port");
        std::cout << "port type: " << TOML::type_name(port.type()) << std::endl;

        if (port.is_array()) {
            const auto& arr = port.as_array();
            for (size_t i = 0; i < arr.size(); i++) {
                const auto& value = arr.at(i);
                std::cout << "port[" << i << "] type: " << TOML::type_name(value.type()) << std::endl;
                if (value.is_integer()) {
                    std::cout << "port[" << i << "] = " << value.as_integer() << std::endl;
                }
            }
        }
    }

    // serialize back to TOML
    std::cout << "\n--- TOML output ---" << std::endl;
    std::cout << root.to_toml() << std::endl;

    return 0;
}

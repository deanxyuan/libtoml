#include <string.h>
#include <iostream>

#include <toml/toml.h>

int main(int argc, char *argv[]) {
    char buff[] =
        R"([server]
    host = "example.com"
    port = [ 8080, 8181, 8282 ]
)";

    std::string error_desc;
    //
    TOML::Node root = TOML::LoadFromData(buff, strlen(buff), &error_desc);
    std::cout << "error desc:" << error_desc << std::endl;

    // check key exists
    bool r = root.AsTable()->Exists("server");
    std::cout << "server exists result:" << r << std::endl;

    // or just call 'Get'
    TOML::Node server = root.AsTable()->Get("server");
    if (!server) {
        std::cout << "server does not exists" << std::endl;
        return 0;
    }
    std::cout << "server exists" << std::endl;
    // get host
    TOML::Node host = server.As<TOML::kTable>()->Get("host");
    std::cout << "host type: " << host.TypeString() << std::endl;
    if (host.Type() == TOML::kString) {
        std::string host_str = host.As<TOML::kString>()->Value();
        std::cout << "host value: " << host_str << std::endl;
    }
    // get port
    TOML::Node port = server.As<TOML::kTable>()->Get("port");
    std::cout << "port type: " << port.TypeString() << std::endl;

    if (port.Type() == TOML::kArray) {
        int count = static_cast<int>(port.AsArray()->size());
        for (int i = 0; i < count; i++) {
            TOML::Node value = port.AsArray()->At(i);
            std::cout << "value type: " << value.TypeString() << std::endl;
            if (value.Type() == TOML::kInteger) {
                int64_t num = value.As<TOML::kInteger>()->Value();
                std::cout << "port[" << i << "] is " << num << std::endl;
            }
            // or just get string
            std::string s = value.ToString();
            std::cout << "port[" << i << "] is " << s << std::endl;
        }
    }
    return 0;
}

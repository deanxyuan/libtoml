
#include <iostream>
#include "src/file_reader.h"

int main(int argc, char *[]) {
    TOML::FileReader reader(__FILE__);

    for (const auto &ctx : reader) {
        std::cout << ctx.Line() << ":" << ctx.Data() << std::endl;
    }

    std::cout << "Hello World" << std::endl;
    return 0;
}

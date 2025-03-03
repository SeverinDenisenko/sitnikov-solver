#include <iostream>

#include "yaml-cpp/yaml.h"

#include "sitnikov.hpp"

int main()
{
    try {
        YAML::Node config = YAML::LoadFile("config.yaml");

        std::cout << "Hello, Sitnikov Solver!" << std::endl;
        return 0;
    } catch (const std::exception& ex) {
        std::cout << "Internal error: " << ex.what() << std::endl;
    }
}

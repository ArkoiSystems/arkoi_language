#include "utils/utils.hpp"

#include <fstream>

std::string get_base_path(const std::string& path) {
    const auto last_dot = path.find_last_of('.');
    if (last_dot == std::string::npos || path.substr(last_dot) != ".ark") {
        throw std::invalid_argument("This is not a valid file path with '.ark' extension.");
    }

    return path.substr(0, last_dot);
}

std::string read_file(const std::string& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

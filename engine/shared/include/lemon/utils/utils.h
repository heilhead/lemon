#pragma once

#include <iostream>

namespace lemon::utils {
    template<typename ...Args>
    void print(Args&& ...args) {
        (std::cout << ... << args) << std::endl;
    }

    template<typename ...Args>
    void printErr(Args&& ...args) {
        (std::cerr << ... << args) << std::endl;
    }

    void halt(const char* message, int code = 1);
}

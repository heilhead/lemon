#pragma once

#include <iostream>
#include <mutex>

namespace lemon::utils {
    namespace {
        std::mutex printMut;
    }

    template<typename ...Args>
    void tprint(Args&& ...args) {
        std::lock_guard lg(printMut);
        (std::cout << ... << args) << std::endl;
    }

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

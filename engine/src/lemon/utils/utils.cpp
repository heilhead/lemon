#include <lemon/utils/utils.h>

void
lemon::utils::halt(const char* message, int code) {
    if (code > 0) {
        printErr(message);
    } else {
        print(message);
    }

    exit(code);
}
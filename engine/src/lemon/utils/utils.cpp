#include <lemon/utils/utils.h>

void
lemon::utils::halt(const char* message, int code) {
    if (message != nullptr) {
        if (code > 0) {
            logErr(message);
        } else {
            log(message);
        }
    }

    exit(code);
}
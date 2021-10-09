#include <lemon/shared/logger.h>
#include <lemon/shared/utils.h>

using namespace lemon;

void
lemon::utils::halt(const char* message, int code)
{
    if (message != nullptr) {
        if (code > 0) {
            logger::err(message);
        } else {
            logger::log(message);
        }
    }

    exit(code);
}

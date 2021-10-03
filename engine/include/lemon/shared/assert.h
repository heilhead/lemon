#pragma once

#include <lemon/utils/utils.h>

#ifndef NDEBUG
#define LEMON_ASSERT(condition, message)                                                                     \
    do {                                                                                                     \
        if (!(condition)) {                                                                                  \
            lemon::utils::logErr("Assertion `" #condition "` failed in ", __FILE__, " line ", __LINE__,      \
                                 ": ", message);                                                             \
            std::terminate();                                                                                \
        }                                                                                                    \
    } while (false)
#else
#define LEMON_ASSERT(condition, message)                                                                     \
    do {                                                                                                     \
    } while (false)
#endif

#define LEMON_TODO() LEMON_ASSERT(false, "not implemented")
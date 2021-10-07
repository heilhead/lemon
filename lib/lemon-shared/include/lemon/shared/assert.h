#pragma once

#include <lemon/shared/logger.h>

#ifndef NDEBUG
#define LEMON_ASSERT(condition)                                                                              \
    do {                                                                                                     \
        if (!(condition)) {                                                                                  \
            ::lemon::logger::err("Assertion `" #condition "` failed in ", __FILE__, " line ", __LINE__);     \
            ::std::terminate();                                                                              \
        }                                                                                                    \
    } while (false)
#else
#define LEMON_ASSERT(condition)                                                                              \
    do {                                                                                                     \
    } while (false)
#endif

#ifndef NDEBUG
#define LEMON_ASSERT_MSG(condition, message)                                                                 \
    do {                                                                                                     \
        if (!(condition)) {                                                                                  \
            ::lemon::logger::err("Assertion `" #condition "` failed in ", __FILE__, " line ", __LINE__,      \
                                 ": ", message);                                                             \
            ::std::terminate();                                                                              \
        }                                                                                                    \
    } while (false)
#else
#define LEMON_ASSERT_MSG(condition, message)                                                                 \
    do {                                                                                                     \
    } while (false)
#endif

#define LEMON_TODO() LEMON_ASSERT_MSG(false, "not implemented")

#pragma once

#include <coroutine>
#include <tl/expected.hpp>

#pragma warning(push)
#pragma warning(disable : 4005)
#include <folly/Portability.h>
#pragma warning(pop)

// Override folly's coroutine define to help retarded code analyzers figure out feature support.
#define FOLLY_HAS_COROUTINES 1

#pragma once

#include <coroutine>
#include <folly/Portability.h>

// Override folly's coroutine define to help retarded code analyzers figure out feature support.
#define FOLLY_HAS_COROUTINES 1

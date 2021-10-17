#pragma once

#include <coroutine>
#include <folly/Portability.h>

// Override folly's coroutine define to help retarded code analyzers figure out feature support.
#define FOLLY_HAS_COROUTINES 1

#include <folly/executors/IOThreadPoolExecutor.h>
#include <folly/executors/CPUThreadPoolExecutor.h>
#include <folly/experimental/coro/Task.h>
#include <folly/experimental/coro/BlockingWait.h>
#include <folly/experimental/coro/Collect.h>
#include <folly/experimental/coro/Baton.h>
#include <folly/hash/Hash.h>
#include <folly/small_vector.h>
#include <folly/AtomicHashMap.h>
#include <folly/PackedSyncPtr.h>

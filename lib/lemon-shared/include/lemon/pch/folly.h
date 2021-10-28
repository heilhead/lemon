#pragma once

#include <coroutine>

#pragma warning(push)
#pragma warning(disable : 4005)

#include <folly/Portability.h>
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

#pragma warning(pop)

#include <iostream>
#include <variant>
#include <vector>
#include <cassert>

#include "lemon/Scheduler.h"

#include <folly/experimental/coro/Generator.h>
#include <folly/experimental/coro/Task.h>
#include <folly/experimental/coro/BlockingWait.h>

//#include "lemon/engine.h"

folly::coro::Task<int> task42() {
    co_return 42;
}

folly::coro::Task<int> taskSlow43() {
    co_await folly::futures::sleep(std::chrono::seconds { 1 });
    co_return co_await task42() + 1;
}

int main(int argc, char* argv[]) {
    auto result = folly::coro::blockingWait(taskSlow43().scheduleOn(folly::getGlobalCPUExecutor().get()));
    std::cout << "result: " << result << std::endl;

    assert(43 == result);

    return 0;
}

#include <iostream>
#include <variant>
#include <vector>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>

#include "lemon/scheduler.h"

#include <folly/experimental/coro/BlockingWait.h>
#include <folly/experimental/coro/Collect.h>

#include "lemon/shared.h"
#include "lemon/shared/filesystem.h"

std::mutex mut;

template<typename ...Args>
void print(Args&& ...args) {
    std::lock_guard lg(mut);
    (std::cout << ... << args) << std::endl;
}

//void gameLoopPseudocode() {
//    while (true) {
//        gFrameManager.swap();
//
//        auto gameStageFuture = gScheduler.schedule([] {
//            co_await updateInput();
//            co_await updatePhysics();
//            co_await updateGameObjects();
//            co_await updateAI();
//            co_await updateAnimationData();
//            co_await prepareRenderData();
//        });
//
//        auto renderStageFuture = gScheduler.schedule([] {
//            co_await updateTransformMatrices();
//            co_await calculateCulling();
//            co_await prepareCommandLists();
//
//            dispatchRender();
//
//            co_await presentFrame();
//        });
//
//        gFrameManager.blockGameLoop(
//            gameStageFuture,
//            renderStageFuture,
//        );
//    }
//}

uint64_t fib(uint64_t n) {
    uint64_t a = 0, b = 1, c, i;
    if (n == 0)
        return a;
    for (i = 2; i <= n; i++) {
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}

using namespace lemon::scheduler;

uint64_t measureFib(uint64_t n, int p) {
    typedef std::chrono::steady_clock clock;
    typedef std::chrono::duration<float> duration;

    auto tStart = clock::now();
    auto result = fib(n);
    auto tStop = clock::now();

    duration dt = tStop - tStart;

    print("fib(", n, "): ", result, " (took ", dt, "s) (priority ", p, ")");

    return result;
}

Task<uint64_t> fibTask(int p) {
    co_return measureFib(1200000000, p);
}

Task<int> multiFibTask(int conc) {
    std::vector<TaskFuture<uint64_t, int>> tasks;

    for (int i = 0; i < conc; i++) {
        tasks.push_back(runCPUTask(fibTask(0), Priority::Low));
    }

    co_await folly::collectAll(tasks.begin(), tasks.end());
    co_return 3;
}

Task<lemon::HeapBuffer, lemon::io::Error> readBinaryFile(std::filesystem::path& path) {
    co_return lemon::io::readFile(path);
}

Task<std::string, lemon::io::Error> readTextFile(std::filesystem::path&& path) {
    // Run the blocking IO task on the designated IO thread pool.
    auto binary = co_await runIOTask(readBinaryFile(path));

    co_return binary.map([](auto& buffer) {
        return std::string(buffer.get<char>(), buffer.size());
    });
}

int main(int argc, char* argv[]) {
    std::unique_ptr<Scheduler> sched = std::make_unique<Scheduler>();

    auto result = folly::coro::blockingWait(
        // The main/decoding task runs on the CPU thread pool, while the inner IO task will run on a different thead pool.
        runCPUTask(readTextFile("C:\\git\\lemon\\engine\\core\\include\\lemon\\engine.h"))
    );

    if (result) {
        std::cout << "result ok: " << *result << std::endl;
    } else {
        std::cout << "result error: " << (int)result.error() << std::endl;
    }

    return 0;
}

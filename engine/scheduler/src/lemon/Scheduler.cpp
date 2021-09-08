#include "lemon/Scheduler.h"

void some_fn() {
}

//folly::coro::AsyncGenerator<int> lemon::scheduler::Scheduler::generateInts(int begin, int end) {
//    for (int i = begin; i < end; i++) {
//        co_await co_reschedule_on_current_executor;
//        co_yield i;
//    }
//}
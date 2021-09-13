#pragma once

#include <lemon/scheduler/Scheduler.h>
#include <lemon/utils/utils.h>

namespace lemon::scheduler {

    template<typename... Args>
    void
    coLog(Args&&... args) {
        auto threadName = Scheduler::getCurrentThreadName();

        lemon::utils::tprint("[", (threadName ? *threadName : "unknown_thread"), "] ",
                             std::forward<Args>(args)...);
    }

} // namespace lemon::scheduler
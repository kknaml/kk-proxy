#pragma once

#include <kkp/uring/awaiter/all.hpp>
#include <kkp/coro/coro_base.hpp>

namespace kkp {

    inline auto delay(io_uring *ring, __kernel_timespec *ts) -> coro::awaitable_result<void> auto {
        return uring::timeout{ring, ts, 1, IORING_TIMEOUT_ETIME_SUCCESS};
    }

    inline auto delay(io_uring *ring, long mills) -> coro::awaitable_result<void> auto {
        __kernel_timespec ts{};
        ts.tv_sec = mills / 1000;
        ts.tv_nsec = (mills % 1000) * 1'000'000;
        return delay(ring, &ts);
    }

    inline auto delay(__kernel_timespec *ts) -> task<> {
        auto *context = co_await coro::this_context;
        co_await delay(context->ring(), ts);
        co_return;
    }

    inline auto delay(long mills) -> task<> {
        auto *context = co_await coro::this_context;
        co_await delay(context->ring(), mills);
        co_return;
    }

} // namespace kkp

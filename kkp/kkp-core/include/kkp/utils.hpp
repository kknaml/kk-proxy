#pragma once

#include <kkp/uring/awaiter/all.hpp>
#include <kkp/coro/coro_base.hpp>

namespace kkp {

    inline auto delay(io_uring *ring, __kernel_timespec *ts) -> coro::awaitable_result<void> auto {
        return uring::timeout{ring, ts, 0, IORING_TIMEOUT_ETIME_SUCCESS};
    }

    inline auto delay(io_uring *ring, long mills) -> coro::awaitable_result<void> auto {
        __kernel_timespec ts{};
        ts.tv_sec = mills / 1000;
        ts.tv_nsec = (mills % 1000) * 1'000'000;
        return delay(ring, &ts);
    }

    inline auto delay(__kernel_timespec *ts) -> task<result<void>> {
        auto *context = co_await coro::this_context;
        co_return co_await delay(context->ring(), ts);
    }

    inline auto delay(long mills) -> task<result<void>> {
        auto *context = co_await coro::this_context;
        co_return co_await delay(context->ring(), mills);
    }

    std::string to_hex_string(std::span<unsigned char> str) noexcept;

} // namespace kkp

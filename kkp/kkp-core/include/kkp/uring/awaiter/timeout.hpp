#pragma once

#include <kkp/uring/base_uring_awaiter.hpp>

namespace kkp::uring {

    struct timeout : base_uring_awaiter {

        timeout(io_uring *ring, __kernel_timespec *ts, unsigned count, unsigned flags)
            : base_uring_awaiter(ring, io_uring_prep_timeout, &this->ts_, count, flags), ts_(*ts) {}

        result<void> await_resume() const noexcept {
            int res = this->data_.result_;
            if (res == -ETIME) [[likely]] return std::expected<void, std::error_code>{};
            return std::unexpected(std::error_code(-res, std::system_category()));
        }

        __kernel_timespec ts_{};
    };
}

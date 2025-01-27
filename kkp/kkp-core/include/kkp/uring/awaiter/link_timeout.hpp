#pragma once

#include <kkp/uring/base_uring_awaiter.hpp>

namespace kkp::uring {

    struct link_timeout : base_uring_awaiter {

        link_timeout(io_uring *ring, __kernel_timespec *ts, unsigned flags = 0)
            : base_uring_awaiter(ring, io_uring_prep_link_timeout, &this->ts_, flags), ts_(*ts) {

        }

        int await_resume() const noexcept {
            return this->data_.result_;
        }

        __kernel_timespec ts_;
    };

}

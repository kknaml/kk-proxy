#pragma once

#include <kkp/uring/base_uring_awaiter.hpp>

namespace kkp::uring {


    struct recv : base_uring_awaiter {

        recv(io_uring *ring, int fd, void *buf, size_t len, int flags)
            : base_uring_awaiter(ring, io_uring_prep_recv, fd, buf, len, flags) {}

        int await_resume() const noexcept {
            return this->data_.result_;
        }
    };
}

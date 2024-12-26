#pragma once

#include <kkp/uring/base_uring_awaiter.hpp>

namespace kkp::uring {

    struct write : base_uring_awaiter {


        write(io_uring *ring, int fd, void *buf, unsigned nbytes, __u64 offset)
        : base_uring_awaiter(ring, io_uring_prep_write, fd, buf, nbytes, offset) {}

        int await_resume() const noexcept {
            return this->data_.result_;
        }
    };

}

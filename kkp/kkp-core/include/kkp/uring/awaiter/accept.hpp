#pragma once

#include <kkp/uring/base_uring_awaiter.hpp>

namespace kkp::uring {

    struct accept : base_uring_awaiter {

        accept(io_uring *ring, int fd, sockaddr *addr, socklen_t *addrlen, int flags)
        : base_uring_awaiter(ring, io_uring_prep_accept, fd, addr, addrlen, flags) {}

        int await_resume() const noexcept {
            return this->data_.result_;
        }
    };

}



#pragma once

#include <kkp/uring/base_uring_awaiter.hpp>

namespace kkp::uring {

    struct connect : base_uring_awaiter {
        connect(io_uring *ring, int fd, const sockaddr *addr, socklen_t addrlen)
        : base_uring_awaiter(ring, io_uring_prep_connect, fd, addr, addrlen) {}

        result<int> await_resume() const noexcept {
            int res = this->data_.result_;
            if (res >= 0) return res;
            return make_sys_error<int>(res);
        }
    };

}

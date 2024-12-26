#pragma once

#include <kkp/uring/base_uring_awaiter.hpp>

namespace kkp::uring {

    struct read : base_uring_awaiter {

        read(io_uring *ring, int fd, void *buf, unsigned nbytes, __u64 offset)
        : base_uring_awaiter(ring, io_uring_prep_read, fd, buf, nbytes, offset) {}

        result<int> await_resume() const noexcept {
            int res = this->data_.result_;
            if (res >= 0) [[likely]] {
                return res;
            } else {
                if (-res >= 10086) {
                    // TODO
                    return std::unexpected(std::error_code(-res, std::system_category()));
                } else {
                    return std::unexpected(std::error_code(-res, std::system_category()));
                }
            }
        }
    };
}

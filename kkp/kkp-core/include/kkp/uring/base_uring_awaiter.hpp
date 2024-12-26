#pragma once

#include <kkp/coro/coro_base.hpp>
#include <kkp/uring/io_data.hpp>
#include <kkp/error_code.hpp>
#include <liburing.h>

namespace kkp::uring {

    struct base_uring_awaiter : coro::base_kk_awaiter, non_copy {

        template<typename F, typename ...Args>
        requires std::is_invocable_v<F, io_uring_sqe *, Args...>
        base_uring_awaiter(io_uring *ring, F &&f, Args ...args) noexcept {
            this->ring_ = ring;
            this->sqe_ = io_uring_get_sqe(ring);
            if (this->sqe_ != nullptr) [[likely]] {
                io_uring_sqe_set_data(this->sqe_, &this->data_);
                f(this->sqe_, args...);
            } else {
                // TODO
                this->data_.result_ = -10086;
            }
        }

        base_uring_awaiter(base_uring_awaiter &&other) noexcept
            : ring_(other.ring_), sqe_(other.sqe_), data_(other.data_) {
            other.ring_ = nullptr;
            other.sqe_ = nullptr;
            other.data_.handle_ = nullptr;
        }

        bool await_ready() const noexcept {
            return sqe_ == nullptr;
        }

        void await_suspend(std::coroutine_handle<> handle) noexcept {
            data_.handle_ = handle;
            int ret = io_uring_submit(ring_);
        }

    protected:
        io_uring *ring_;
        io_uring_sqe *sqe_;
        io_data data_{};
    };

} // namespace kkp::uring
#pragma once

#include <kkp/coro/coro_base.hpp>
#include <kkp/uring/awaiter/all.hpp>
#include <span>

namespace kkp::fs {

    class file : public non_copy {
    public:
        explicit file(io_uring *ring, std::string_view path, int flag = O_RDWR) : ring_(ring) {
            this->fd_ = open(path.data(), flag);
        }

        file(file &&other) noexcept : fd_(other.fd_), ring_(other.ring_), state_flag_(other.state_flag_) {
            other.fd_ = 0;
            other.ring_ = nullptr;
            other.state_flag_ = 0;
        }

        file &operator=(file &&other) noexcept {
            if (&other != this) {
                if (fd_ > 0) close(fd_);
                fd_ = other.fd_;
                ring_ = other.ring_;
                state_flag_ = other.state_flag_;
                other.fd_ = 0;
                other.ring_ = nullptr;
            }
            return *this;
        }


        ~file() noexcept {
            int fd = this->fd_;
            if (fd != 0) {
                close(fd);
            }
        }

        auto read(std::span<uint8_t> buf, int len) -> coro::awaitable_result<int> auto {
            return uring::read(ring_, fd_, buf.data(), len, 0);
        }

        auto read(std::span<uint8_t> buf) -> coro::awaitable_result<int> auto {
            return this->read(buf, buf.size());
        }

        int fd() const noexcept {
            return this->fd_;
        }

    private:
        int fd_;
        io_uring *ring_;
        int state_flag_{};

    private:
        // constexpr static int EXIST_FLAG = 1;

    public:
        static coro::task<file> of(std::string_view path, int flag = O_RDWR) {
            auto *context = co_await coro::this_context;
            co_return file(context->ring(), path, flag);
        }
    };
}
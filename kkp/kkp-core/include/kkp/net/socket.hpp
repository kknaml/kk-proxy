#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <kkp/utils.hpp>

namespace kkp::net {

    class socket {
    public:
        explicit socket(io_uring *ring, int fd) : ring_(ring), fd_(fd) {}

        socket(socket &&other) noexcept :
            ring_(std::exchange(other.ring_, nullptr)),
            fd_(std::exchange(other.fd_, -1)) {}

        socket &operator=(socket &&other) noexcept {
            if (this != &other) {
                if (fd_ > 0) {
                    ::close(fd_);
                }
                ring_ = std::exchange(other.ring_, nullptr);
                fd_ = std::exchange(other.fd_, -1);
            }
            return *this;
        }

        auto send(std::span<uint8_t> buf, size_t len = -1, int flag = 0) -> coro::awaitable_result<int> auto {
            if (len == -1) len = buf.size();
            return uring::send(ring_, fd_, buf.data(), len, 0);
        }

        auto recv(std::span<uint8_t> buf, size_t len = -1, int flag = 0) -> coro::awaitable_result<int> auto {
            if (len == -1) len = buf.size();
            return uring::recv(ring_, fd_, buf.data(), len, 0);
        }

    private:
        io_uring *ring_;
        int fd_;
    };



    struct socket_accept_awaiter : uring::accept {

        socket_accept_awaiter(io_uring *ring, int fd, int flags) noexcept
            : uring::accept(ring, fd, &addr_, &addrlen_, flags) {}

        auto await_resume() const noexcept -> result<socket> {
            int ret = uring::accept::await_resume();
            if (ret > 0) {
                return socket(this->ring_, ret);
            } else {
                return make_sys_error<socket>(ret);
            }
        }

        sockaddr addr_{};
        socklen_t addrlen_ = sizeof(addr_);
    };


    class server_socket : non_copy {
    public:
        explicit server_socket(io_uring *ring) : ring_(ring) {
            fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
            if (fd_ == -1) { // TODO: use result instead of constructor
                throw std::runtime_error("socket creation failed");
            }
        }

        server_socket(server_socket &&other) noexcept :
        ring_(std::exchange(other.ring_, nullptr)),
        fd_(std::exchange(other.fd_, -1)),
        addr_(other.addr_) { }

        server_socket &operator=(server_socket &&other) noexcept {
            if (this != &other) {
                if (fd_ > 0) {
                    ::close(fd_);
                }
                ring_ = std::exchange(other.ring_, nullptr);
                fd_ = std::exchange(other.fd_, -1);
                addr_ = other.addr_;
            }
            return *this;
        }


        auto bind(uint16_t port) -> result<void> {

            int opt = 1;
            int ret = setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            if (ret < 0) {
                perror("setsockopt failed");
                spdlog::error("setsockopt");
                return make_sys_error<void>(ret);
            }

            addr_.sin_family = AF_INET;
            addr_.sin_port = htons(port);
            // addr_.sin_addr.s_addr = htonl(INADDR_ANY);
            addr_.sin_addr.s_addr = inet_addr("0.0.0.0");
            ret = ::bind(fd_, reinterpret_cast<sockaddr*>(&addr_), sizeof(addr_));
            if (ret < 0) {
                return make_sys_error<void>(-errno);
            }
            return {};
        }

        auto listen(int backlog = 5) const -> result<void> {
            auto ret = ::listen(fd_, backlog);
            if (ret < 0) {
                return make_sys_error<void>(ret);
            }
            return {};
        }

        auto accept() -> coro::awaitable_result<socket> auto {
            return socket_accept_awaiter(ring_, fd_, 0);
        }

        auto fd() const noexcept -> int {
            return fd_;
        }

        ~server_socket() noexcept{
            if (fd_ > 0) {
                ::close(fd_);
                fd_ = -1;
            }
        }

    private:
        io_uring *ring_;
        int fd_;
        sockaddr_in addr_{};

    public:
        static auto open() -> task<server_socket> {
            auto *context = co_await coro::this_context;
            co_return server_socket(context->ring());
        }
    };


} // namespace kkp::net

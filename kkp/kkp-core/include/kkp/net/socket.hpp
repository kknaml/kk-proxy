#pragma once

#include <netinet/in.h>
#include <kkp/utils.hpp>

namespace kkp::net {

    class socket {
    public:
        explicit socket(int fd) : fd_(fd) {}

    private:
        int fd_;
    };


    namespace detail {

        struct socket_accept_awaiter : uring::accept {

            socket_accept_awaiter(io_uring *ring, int fd, sockaddr *addr, socklen_t *addrlen, int flags) noexcept
                : uring::accept(ring, fd, addr, addrlen, flags) {}

            auto await_resume() const noexcept -> result<socket> {
                int ret = uring::accept::await_resume();
                if (ret > 0) {
                    return socket(ret);
                } else {
                    return make_sys_error<socket>(ret);
                }
            }

        };


        class server_socket {
        public:
            explicit server_socket(io_uring *ring) {
                fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
                if (fd_ == -1) { // TODO: use result instead of constructor
                    throw std::runtime_error("socket creation failed");
                }
            }

            auto bind(uint16_t port) -> result<void> {


                addr_.sin_family = AF_INET;
                addr_.sin_port = htons(port);
                addr_.sin_addr.s_addr = htonl(INADDR_ANY);
                auto ret = ::bind(fd_, reinterpret_cast<sockaddr*>(&addr_), sizeof(addr_));
                if (ret < 0) {
                    return make_sys_error<void>(ret);
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
                socklen_t len = sizeof(addr_);
                return socket_accept_awaiter(ring_, fd_, reinterpret_cast<sockaddr*>(&addr_), &len, 0);
            }

            auto fd() const noexcept -> int {
                return fd_;
            }

            ~server_socket() noexcept{
                if (fd_ > 0) {
                    ::close(fd_);
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

    } // namespace detail

} // namespace kkp::net

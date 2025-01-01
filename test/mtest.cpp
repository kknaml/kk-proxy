#define SOCK_PATH "/root/echo_socket"

#include <liburing.h>
#include <print>
#include <cstring>
#include <kkp/io_context.hpp>
#include <kkp/net/socket.hpp>

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::debug);
    kkp::io_context context{1};

    context.block_run([] -> kkp::coro::task<> {

        auto socket = co_await kkp::net::detail::server_socket::open();
        spdlog::debug("socket: {}", socket.fd());
        {
            auto res = socket.bind(60045);
            if (!res.has_value()) {
                spdlog::error("bind error: {}", res.error().message());
            }
            res = socket.listen();
            if (!res.has_value()) {
               spdlog::error("listen error: {}", res.error().message());
           }
        }
        auto client = co_await socket.accept();
        spdlog::debug("accept client");
    });
}

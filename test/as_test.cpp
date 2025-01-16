
#include <spdlog/spdlog.h>
#include <kkp/error_code.hpp>
#include <kkp/io_context.hpp>
#include <kkp/coro/task.hpp>
#include <kkp/net/socket.hpp>
#include <kkp/ssl/ssl_engine.hpp>
#include <kkp/ssl/ssl_stream.hpp>

const char *cert_file = "/root/workspace/auto/kkproxy/server.crt";
const char *key_file = "/root/workspace/auto/kkproxy/server.key";

std::string to_hex_string(std::span<unsigned char> str) noexcept {
    std::string result;
    result.reserve(str.size() * 3);
    for (auto c: str) {
        result.append(std::format("{:02X} ", c));
    }
    return result;
}

constexpr char hello[] = "GET / HTTP/1.1\r\n"
                         "\r\n";


int main() {
    spdlog::set_level(spdlog::level::debug);

    kkp::io_context ctx{1};

    ctx.block_run([&ctx] -> kkp::coro::task<> {

        auto endpoint = kkp::net::endpoint::from("baidu.com", 443);
        auto socket = co_await kkp::net::socket::open(endpoint);
        auto res = co_await socket.connect();
        if (!res.has_value()) {
            spdlog::error("connect failed");
            spdlog::error("{}", res.error().message());
            co_return;
        }
        SSL_CTX *ssl_ctx = SSL_CTX_new(TLS_client_method());
        auto ssl_stream = kkp::ssl::ssl_stream(std::move(socket), ssl_ctx);
        auto hand_res = co_await ssl_stream.handshake(kkp::ssl::handshake_type::client);
        if (hand_res.has_value()) {
            spdlog::info("handshake success");
            auto write_res = co_await ssl_stream.send({(uint8_t *)hello, sizeof(hello) - 1});
            spdlog::info("send: {}", write_res);
            std::array<uint8_t, 4096> buffer{};
            auto read_res = co_await ssl_stream.recv(buffer);
            spdlog::info("recv: {}", read_res);
            spdlog::info("recv: {}", std::string_view{(char *)buffer.data(), (size_t)read_res});
;        } else {
            spdlog::error("handshake failed");
        }
        co_return;
    });
}

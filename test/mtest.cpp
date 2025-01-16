#include <liburing.h>
#include <print>
#include <cstring>
#include <kkp/io_context.hpp>
#include <kkp/net/socket.hpp>
#include <kkp/ssl/ssl_engine.hpp>
#include <kkp/ssl/ssl_stream.hpp>
#include <spdlog/spdlog.h>
#include <execinfo.h>




const char *cert_file = "/opt/1panel/apps/openresty/openresty/www/sites/kirapzz.moe/ssl/fullchain.pem";
const char *key_file = "/opt/1panel/apps/openresty/openresty/www/sites/kirapzz.moe/ssl/privkey.pem";

// const char *cert_file = "/root/workspace/auto/kkproxy/server.crt";
// const char *key_file = "/root/workspace/auto/kkproxy/server.key";


bool init_ctx(SSL_CTX *ctx) {
    if (SSL_CTX_use_certificate_file(
            ctx, cert_file,
            SSL_FILETYPE_PEM) <= 0) {
        spdlog::error("use certificate file error");
        ERR_print_errors_fp(stderr);
        return false;
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, key_file,
                                    SSL_FILETYPE_PEM) <= 0) {
        spdlog::error("use private key file error");
        return false;
    }
    if (SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION) != 1) {
        spdlog::error("Failed to set minimum protocol version to TLS 1.2");
        ERR_print_errors_fp(stderr);
        return false;
    }
    if (SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION) != 1) {
        spdlog::error("Failed to set maximum protocol version to TLS 1.3");
        ERR_print_errors_fp(stderr);
        return false;
    }

    // if (SSL_CTX_get_max_proto_version(ctx) < TLS1_3_VERSION) {
    //     spdlog::error("TLS 1.3 is not supported");
    //     return false;
    // }

    // if (SSL_CTX_set_ciphersuites(ctx,
    //                              "TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256") != 1) {
    //     spdlog::error("Failed to set TLS 1.3 ciphersuites");
    //     ERR_print_errors_fp(stderr);
    //     return false;
    // }
    //
    // if (SSL_CTX_set_cipher_list(ctx,
    //                             "ALL") != 1) {
    //     spdlog::error("Failed to set TLS 1.2 and below ciphersuites");
    //     ERR_print_errors_fp(stderr);
    //     return false;
    // }

   // SSL_CTX_set1_sigalgs_list(ctx, sigalgs, sizeof(sigalgs) / sizeof(sigalgs[0]));

    // if (SSL_CTX_set1_sigalgs_list(ctx, "ECDSA+SHA256:RSA+SHA256:RSA+SHA1:ECDSA+SHA1") != 1) {
    //     spdlog::error("Failed to set signature algorithms");
    //     ERR_print_errors_fp(stderr);
    //     // return false;
    // }

    // SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, [](auto, auto) {
    //     return 1;
    // });

    SSL_CTX_set_info_callback(ctx, [](auto *ssl, auto where, auto ret) {
        spdlog::debug("ssl info: type: {}, val: {}", where, ret);
        const char *str;
        int w;
        w = where & ~SSL_ST_MASK;
        if (w & SSL_ST_CONNECT) str = "SSL_connect";
        else if (w & SSL_ST_ACCEPT) str = "SSL_accept";
        else str = "undefined";
        if (where & SSL_CB_LOOP) {
            spdlog::debug("{}:{}\n", str, SSL_state_string_long(ssl));
        } else if (where & SSL_CB_ALERT) {
            str = (where & SSL_CB_READ) ? "read" : "write";
            spdlog::debug("SSL3 alert {}:{}:{}\n", str, SSL_alert_type_string_long(ret),
                          SSL_alert_desc_string_long(ret));
        } else if (where & SSL_CB_EXIT) {
            if (ret == 0)
                spdlog::debug("{}:failed in {}\n", str, SSL_state_string_long(ssl));
            else if (ret < 0) {
                spdlog::debug("{}:error in {}\n", str, SSL_state_string_long(ssl));
            }
        }
    });

    // const unsigned char alpn_protocols[] = "\x02h2\x08http/1.1";
    // SSL_CTX_set_alpn_protos(ctx, alpn_protocols, sizeof(alpn_protocols) - 1);
    // const int curve_list[] = {
    //     NID_X9_62_prime256v1, // secp256r1
    //     NID_secp384r1, // secp384r1
    //     NID_X25519, // X25519
    // };
    // SSL_CTX_set1_curves_list(ctx, "X25519:secp256r1:secp384r1");

    // set server name
    // SSL_CTX_set_tlsext_servername_callback(ctx, [] {
    //                                        spdlog::debug("set server name");
    //                                        });
    // SSL_CTX_set_tlsext_servername_arg(ctx, nullptr);

    return true;
}

static auto print_stack_trace() noexcept -> void {
    void *buffer[100];
    int nptrs = backtrace(buffer, 100);
    char **symbols = backtrace_symbols(buffer, nptrs);
    for (int i = 0; i < nptrs; i++) {
        spdlog::error("{}", symbols[i]);
    }
    free(symbols);
}

const char *xxxx = "HTTP/1.1 200 OK\r\n"
                   "Content-Type: text/html; charset=utf-8\r\n"
                   "Content-Length: 3\r\n"
                   "Server: KK-NAML/0.0.1\r\n"
                   "Connection: close\r\n"
                   "\r\n"
                   "QAQ";

kkp::coro::task<> zz(kkp::net::socket &&s) {
    spdlog::debug("accept client");
    SSL_CTX *ctx = SSL_CTX_new(SSLv23_server_method());

    if (!init_ctx(ctx)) {
        spdlog::error("init ctx error");
        co_return;
    }
    auto ssl_stream = kkp::ssl::ssl_stream(std::move(s), ctx);
    auto hand_res = co_await ssl_stream.handshake(kkp::ssl::handshake_type::server);
    if (!hand_res.has_value()) {

        spdlog::error("handshake error: {}, {}", hand_res.error().value(), hand_res.error().message());
        co_return;
    } else {
        spdlog::debug("handshake success");

        std::array<uint8_t, 4096> buf{};
        auto res = co_await ssl_stream.recv(buf);
        if (res > 0) {
            spdlog::debug("ssl_stream recv bytes: {}", res);
            spdlog::debug("recv: \n{}", (char *) buf.data());
            spdlog::debug("hex: \n{}", to_hex_string(buf));
            res = co_await ssl_stream.send({(uint8_t *)(xxxx), strlen(xxxx)});
            spdlog::debug("ssl_stream send bytes: {}", res);
            if (res < 0) {
                spdlog::error("send error: {}", res);
            }
        } else {
            spdlog::error("recv error: {}", res);
        }


    }
}

int main(int argc, char *argv[]) {
    kkp::ssl::ssl_engine::init_openssl();
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%H:%M:%S %f] [%n] [%^%l%$] [thread %t] %v");
    kkp::io_context context{4};

    context.block_run([&context] -> kkp::coro::task<> {
        auto socket = co_await kkp::net::server_socket::open();
        spdlog::debug("socket: {}", socket.fd()); {
            auto res = socket.bind(8888);
            if (!res.has_value()) {
                spdlog::error("bind error: {}", res.error().message());
                co_return;
            }
            res = socket.listen();
            if (!res.has_value()) {
                spdlog::error("listen error: {}", res.error().message());
                co_return;
            }
        }
        while (true) {
            auto res = co_await socket.accept();
            if (!res.has_value()) {
                spdlog::error("accept error: {}", res.error().message());
                continue;
            }
            auto &s = res.value();
            spdlog::info("peer ip: {}", s.peer().address().ip_address());
            spdlog::info("peer port: {}", s.peer().port());
            // co_await zz(std::move(s));
            context.launch_detached(zz(std::move(s)));
        }
        // std::array<uint8_t, 4096> buffer{};
        // auto read_bytes = co_await ssl_stream.recv(buffer);
        // spdlog::debug("read bytes: {:02X}", read_bytes);
        // spdlog::debug("recv: \n{}", to_hex_string(buffer));
    });
}

auto fff() {
    struct abc {
        int a;
        double b;
        char c;
    };

    abc x{10, 2.0, 's'};
    auto &[a, b, c] = x;
}

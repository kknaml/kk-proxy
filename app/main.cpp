

#include <print>
#include <string_view>
#include <map>
#include <array>
#include <utility>
#include <iostream>
// #include <asio.hpp>
// #include <asio/ssl/stream.hpp>
// #include <asio/awaitable.hpp>
#include <openssl/ssl.h>
#include <openssl/err.h>

constexpr std::string_view OK = "OK";
constexpr std::string_view FORBIDDEN = "FORBIDDEN";
constexpr std::string_view UNKNOWN = "UNKNOWN";

inline constexpr auto mm = std::array {
    std::pair{200, OK},
    std::pair{403, FORBIDDEN},
};

inline constexpr std::string_view get_message(int code) {
    for (const auto& [c, m] : mm) {
        if (c == code) {
            return m;
        }
    }
    return UNKNOWN;
}

void print_hex(const char *buf, size_t len) {
    for (auto i = 0; i < len; i++) {
        std::printf("%02X ", buf[i]);
    }
}

int main() {

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    auto *ctx = SSL_CTX_new(TLS_method());
    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION);
    auto *ssl = SSL_new(ctx);

    BIO *in_bio;
    BIO *out_bio;

    BIO_new_bio_pair(&in_bio, 0, &out_bio, 0);
    BIO_set_nbio(in_bio, 1);
    SSL_set_bio(ssl, in_bio, in_bio);

    SSL_set_connect_state(ssl);

    auto c = SSL_connect(ssl);
    if (c <= 0) {
        ERR_print_errors_fp(stderr);
        auto e = SSL_get_error(ssl, c);

        std::println("c is {}, e is {}", c, e == SSL_ERROR_WANT_READ);
    }

    char buf[1024];
    auto len = BIO_read(out_bio, buf, sizeof(buf));
    std::println("Len: ", len);
    print_hex(buf, len);
}

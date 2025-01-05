#pragma once

#include <openssl/ssl.h>

namespace kkp::ssl {

    enum class handshake_type {
        client, server
    };

    enum class ssl_status {
        ok, error, want_read, want_write
    };

    inline ssl_status get_ssl_status(const SSL *ssl, int n) noexcept {
        switch (auto code = SSL_get_error(ssl, n)) {
            case SSL_ERROR_NONE:
                return ssl_status::ok;
            case SSL_ERROR_WANT_READ:
                return ssl_status::want_read;
            case SSL_ERROR_WANT_WRITE:
                return ssl_status::want_write;
            default:
                return ssl_status::error;
        }
    }

} // namespace kkp::ssl

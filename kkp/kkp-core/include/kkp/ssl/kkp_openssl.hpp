#pragma once

#include <memory>
#include <openssl/ssl.h>
#include <openssl/bio.h>

namespace kkp::ssl {

    class kkp_openssl_free {

    public:

        template<typename T>
        auto operator() (T *ptr) const noexcept -> void {
            if (ptr) {
                if constexpr (std::is_same_v<T, SSL_CTX>) {
                    SSL_CTX_free(ptr);
                } else if constexpr (std::is_same_v<T, SSL>) {
                    SSL_free(ptr);
                } else if constexpr (std::is_same_v<T, BIO>) {
                    BIO_free(ptr);
                } else {
                    static_assert(false, "Unsupported type for kkp_openssl_free");
                }
            }
        }
    };

    using ssl_ctx_ptr = std::unique_ptr<SSL_CTX, kkp_openssl_free>;
    using ssl_ptr = std::unique_ptr<SSL, kkp_openssl_free>;
    using bio_ptr = std::unique_ptr<BIO, kkp_openssl_free>;

} // namespace kkp::ssl

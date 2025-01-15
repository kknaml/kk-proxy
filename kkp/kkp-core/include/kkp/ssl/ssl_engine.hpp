#pragma once

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <kkp/kkp_traits.hpp>
#include <kkp/ssl/ssl_common.hpp>

namespace kkp::ssl {

    class ssl_engine : non_copy {
    public:
        explicit ssl_engine(SSL_CTX *ctx) : ssl_engine(SSL_new(ctx))  {
        }

        explicit ssl_engine(SSL *ssl) : ssl_(ssl) {
            if (ssl_ == nullptr) {
                throw std::runtime_error("SSL_new failed");
            }
            read_bio_ = BIO_new(BIO_s_mem());
            if (read_bio_ == nullptr) {
                throw std::runtime_error("BIO_new failed");
            }
            write_bio_ = BIO_new(BIO_s_mem());
            if (write_bio_ == nullptr) {
                throw std::runtime_error("BIO_new failed");
            }
            SSL_set_bio(ssl_, read_bio_, write_bio_);
        }

        ssl_engine(ssl_engine &&other) noexcept :
                ssl_(std::exchange(other.ssl_, nullptr)),
                read_bio_(std::exchange(other.read_bio_, nullptr)),
                write_bio_(std::exchange(other.write_bio_, nullptr)) {
            spdlog::info("ssl_engine move constructor");
        }

        ssl_engine &operator=(ssl_engine &&other) noexcept {
            spdlog::info("ssl_engine move assignment");
            if (this != &other) {
                if (ssl_ != nullptr) {
                    SSL_free(ssl_);
                }
                if (read_bio_ != nullptr) {
                    BIO_free(read_bio_);
                }
                if (write_bio_ != nullptr) {
                    BIO_free(write_bio_);
                }
                ssl_ = std::exchange(other.ssl_, nullptr);
                read_bio_ = std::exchange(other.read_bio_, nullptr);
                write_bio_ = std::exchange(other.write_bio_, nullptr);
            }
            return *this;
        }

        ~ssl_engine() noexcept {
            if (ssl_ != nullptr) [[likely]] {
                SSL_free(ssl_);
                ssl_ = nullptr;
            }
            if (read_bio_ != nullptr) {
                // BIO_free(read_bio);
            }
            if (write_bio_ != nullptr) {
                // BIO_free(write_bio);
            }
        }

        auto perform(func<int, SSL *> auto &&op) noexcept -> ssl_status {

            auto pending_output_before = write_pending();
            auto result = op(ssl_);
            //  get_ssl_status(ssl_, result);
            auto ssl_error = SSL_get_error(ssl_, result);
            auto sys_error = ERR_get_error();
            auto pending_output_after = write_pending();
            ERR_print_errors_fp(stderr);

            if (sys_error != 0) {
                spdlog::error("SSL error: {}", sys_error);
                char buf[256];
                ERR_error_string_n(sys_error, buf, sizeof(buf));
                spdlog::error("SSL error: {} - {}", sys_error, buf);
            }

            if (pending_output_after > 0) {
                return ssl_status::want_write;
            }

            if (ssl_error == SSL_ERROR_SYSCALL) {
                if (sys_error == 0) {
                    spdlog::error("SSL_ERROR_SYSCALL");
                    if (errno != 0) {
                        perror("System call error");
                    }
                    return ssl_status::error;
                }
                auto code = errno;
                spdlog::error("SSL_ERROR_SYSCALL: {}", code);
                return ssl_status::error;
            }

            return get_ssl_status(ssl_, result);

        }

        auto write(const void *buf, int len) noexcept -> int {
            return BIO_write(read_bio_, buf, len);
        }

        auto read(void *buf, int len) noexcept -> int {
            return BIO_read(write_bio_, buf, len);
        }

        auto ssl_read(void *buf, int len) noexcept -> std::pair<int, ssl_status> {
            auto read =  SSL_read(ssl_, buf, len);
            return  {read, get_ssl_status(ssl_, read)};
        }

        auto ssl_write(const void *buf, int len) noexcept -> std::pair<int, ssl_status> {
            auto write = SSL_write(ssl_, buf, len);
            return {write, get_ssl_status(ssl_, write)};
        }

        auto get_output_pending() const noexcept -> size_t {
            return BIO_ctrl_pending(write_bio_);
        }

        auto ssl() const noexcept -> SSL * {
            return ssl_;
        }

        auto write_pending() const noexcept -> size_t {
            return BIO_ctrl_pending(write_bio_);
        }

        auto read_pending() const noexcept -> size_t {
            return BIO_ctrl_pending(read_bio_);
        }

    private:
        SSL *ssl_;
        BIO *read_bio_;
        BIO *write_bio_;

    public:
        static void init_openssl() noexcept {
            SSL_library_init();
            SSL_load_error_strings();
            OpenSSL_add_all_algorithms();
        }
    };


} // namespace kkp::ssl

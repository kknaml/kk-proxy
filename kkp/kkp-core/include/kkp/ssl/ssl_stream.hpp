#pragma once

#include <kkp/error_code.hpp>
#include <kkp/ssl/ssl_engine.hpp>
#include <kkp/ssl/kkp_openssl.hpp>


namespace kkp::ssl {

    template<kk_stream RawStream>
    class ssl_stream : non_copy {
    public:
        ssl_stream(RawStream &&raw_stream, SSL_CTX *ssl_ctx)
            : raw_stream_(std::move(raw_stream))
            , ssl_engine_{ssl_ctx}
        {
            leftover_.clear();
        }

        ssl_stream(ssl_stream &&other) noexcept
            : raw_stream_(std::move(other.raw_stream_))
            , ssl_engine_{std::move(other.ssl_engine_)}
            , leftover_{std::move(other.leftover_)}
        {
            other.leftover_.clear();
        }

        ssl_stream &operator=(ssl_stream &&other) noexcept {
            if (this != &other) {
                raw_stream_ = std::move(other.raw_stream_);
                ssl_engine_ = std::move(other.ssl_engine_);
                leftover_ = std::move(other.leftover_);
                other.leftover_.clear();
            }
            return *this;
        }

        task<result<void>> handshake(handshake_type type) noexcept {
            spdlog::debug("start handshake");
            auto *ssl = ssl_engine_.ssl();
            int (*op)(SSL *);
            if (type == handshake_type::client) {
                op = SSL_connect;
                SSL_set_connect_state(ssl);
            } else {
                op = SSL_accept;
                SSL_set_accept_state(ssl);
            }

            bool done = false;
            while (!done) {
                std::array<uint8_t, 4096> buf{};
                // auto ret = op(ssl);
                // spdlog::debug("op ret: {}", ret);
                // auto status = get_ssl_status(ssl, ret);
                // spdlog::debug("status: {}", static_cast<int>(status));
                auto status = ssl_engine_.perform(op);
                spdlog::debug("status: {}", static_cast<int>(status));
                switch (status) {
                    case ssl_status::ok: {
                        spdlog::debug("handshake done");
                        done = true;
                        continue;
                    }
                    case ssl_status::error_syscall:
                    case ssl_status::error: {
                        spdlog::warn("handshake error");
                        // auto err = SSL_get_error(ssl, ret);
                        // spdlog::debug("handshake error: {}", err);
                        auto err = (int)ERR_get_error();
                        // co_return make_custom_error<void>(0, "SSL handshake failed");
                        // log ssl error
                        ERR_print_errors_fp(stderr);
                        co_return make_sys_error<void>(-err);
                    }
                    case ssl_status::want_read: {
                        spdlog::debug("wand read");
                        auto r = co_await raw_stream_.recv(buf, buf.size());
                        spdlog::debug("read {} bytes", r);

                        if (r == 0) [[unlikely]] {
                            co_return make_custom_error<void>(E_SSL_HANDSHAKE_SOCKET_READ_EOF);
                        }
                        if (r < 0) [[unlikely]] {
                            // co_return make_custom_error<void>(-r, std::format("SSL handshake failed, raw read error {}", r));
                            co_return make_sys_error<void>(-r);
                        }
                        spdlog::debug("data: {}", to_hex_string({buf.data(), r}));
                        spdlog::debug("start engine write");
                        auto res = ssl_engine_.write(buf.data(), r);
                        spdlog::debug("engine write {} bytes", res);
                        if (res <= 0) {
                            auto err = SSL_get_error(ssl, res);
                            co_return make_custom_error<void>(E_SSL_HANDSHAKE_FAILED);
                        }

                        break;
                    }
                    case ssl_status::want_write: {
                        spdlog::debug("wand write");
                        auto res = ssl_engine_.read(buf.data(), buf.size());
                        spdlog::debug("engine read {} bytes", res);
                        if (res <= 0) [[unlikely]] {
                            auto err = SSL_get_error(ssl, res);
                            co_return make_custom_error<void>(E_SSL_HANDSHAKE_FAILED);
                        }
                        spdlog::debug("start raw write");
                        int bytes_sent = 0;
                        while (bytes_sent < res) {
                            auto sent = co_await raw_stream_.send({buf.data() + bytes_sent, res - bytes_sent});
                            spdlog::debug("raw write {} bytes", sent);
                            if (sent < 0) [[unlikely]] {
                                // co_return make_custom_error<void>(sent, std::format("SSL handshake failed, raw write error: {}", sent));
                                co_return make_sys_error<void>(-sent);
                            }
                            bytes_sent += sent;
                        }

                        break;
                    }
                }
            }
            co_return result<void>{};
        }


        auto send(std::span<uint8_t> buf, int flag = 0) ->  task<int> {
            const auto len = buf.size();
            size_t bytes_consumed = 0;

            std::array<uint8_t, 4096> write_buf{};
            while(bytes_consumed < len) {
                auto res = SSL_write(ssl_engine_.ssl(), buf.data() + bytes_consumed, len - bytes_consumed);
                if (res <= 0) {
                    throw std::runtime_error("ssl write error TODO");
                }
                bytes_consumed += res;

                while (true) {
                    if (const auto pending = ssl_engine_.write_pending(); pending <= 0) {
                        break;
                    }
                    const auto ssl_read = ssl_engine_.read(write_buf.data(), write_buf.size());
                    if (ssl_read <= 0) {
                        const auto status = get_ssl_status(ssl_engine_.ssl(), ssl_read);
                        if (status == ssl_status::want_read || status == ssl_status::want_write) {
                            break;
                        } else {
                            throw std::runtime_error("ssl read error TODO");
                        }
                    }
                    int bytes_sent = 0;
                    while (bytes_sent < ssl_read) {
                        auto raw_send = co_await raw_stream_.send({write_buf.data() + bytes_sent, ssl_read - bytes_sent}, flag);
                        if (raw_send <= 0) {
                            co_return raw_send;
                        }
                        bytes_sent += raw_send;
                    }
                }
            }
            co_return bytes_consumed;
        }

        auto recv(std::span<uint8_t> buf, int flag = 0) -> task<int> {
            const auto len = buf.size();
            if (auto res = read_leftover(buf); res > 0) {
                co_return res;
            }

            if (auto [res, status] = ssl_engine_.ssl_read(buf.data(), len); res > 0) {
                co_return res;
            }

            auto raw_buf = std::vector<uint8_t>(len);

            const auto bytes_received = co_await raw_stream_.recv({raw_buf.data(), len}, flag);
            if (bytes_received <= 0) {
                co_return bytes_received;
            }

            const auto bytes_bio_written = ssl_engine_.write(raw_buf.data(), bytes_received);
            if (bytes_bio_written <= 0) {
                spdlog::error("bytes_bio_written: {}", bytes_bio_written);
                co_return -1;
            }
            if (bytes_bio_written != bytes_received) {
                // TODO
                spdlog::error("bytes_bio_written != bytes_received, TODO");
            }

            int total_decrypted = 0;

            while (true) {
                auto n1 = ssl_engine_.read_pending();
                auto n2 = ssl_engine_.write_pending();
                if (const auto buf_remaining = len - total_decrypted; buf_remaining > 0) [[likely]] {
                    const auto [decrypted, status ] = ssl_engine_.ssl_read(buf.data() + total_decrypted, static_cast<int>(buf_remaining));
                    if (decrypted > 0) {
                        total_decrypted += decrypted;
                        if (status == ssl_status::ok) {
                            break;
                        }
                    } else {
                        if (status == ssl_status::want_read || status == ssl_status::want_write) {
                            break;
                        } else {
                            spdlog::error("decrypted error: {}", decrypted);
                            co_return -1;
                        }
                    }
                } else {
                    const auto leftover_remaining = leftover_.capacity() - leftover_.size();
                    if (leftover_remaining <= 0) [[unlikely]] {
                        leftover_.reserve(leftover_.capacity() * 2);
                    }
                    const auto decrypted = ssl_engine_.read(leftover_.data() + leftover_.size(), leftover_remaining);
                    if (decrypted > 0) {
                        leftover_.resize(leftover_.size() + decrypted);
                    } else {
                        const auto status = get_ssl_status(ssl_engine_.ssl(), decrypted);
                        if (status == ssl_status::want_read || status == ssl_status::want_write) {
                            break;
                        } else {
                            spdlog::error("decrypted error: {}", decrypted);
                            co_return -1;
                        }
                    }
                }
            }
            co_return total_decrypted;
        }

        auto is_alive() const noexcept -> bool {
            return raw_stream_.is_alive();
        }

        RawStream &raw_stream() noexcept {
            return raw_stream_;
        }

        RawStream *operator->() noexcept {
            return &raw_stream_;
        }

    private:

        auto read_leftover(std::span<uint8_t> buf) noexcept -> size_t {
            if (leftover_.empty()) {
                return 0;
            }
            const auto len = std::min(buf.size(), leftover_.size());
            std::copy(leftover_.begin(), leftover_.begin() + len, buf.begin());
            leftover_.erase(leftover_.begin(), leftover_.begin() + len);
            return len;
        }

     private:
        RawStream raw_stream_;
        ssl_engine ssl_engine_;

        std::vector<uint8_t> leftover_ = std::vector<uint8_t>(4096);

    };
} // namespace kkp::ssl

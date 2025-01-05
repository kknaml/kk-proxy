#pragma once

#include <kkp/ssl/ssl_engine.hpp>

std::string to_hex_string(std::span<unsigned char> str) noexcept;
namespace kkp::ssl {

    template<kk_stream RawStream>
    class ssl_stream : non_copy {
    public:
        ssl_stream(RawStream &&raw_stream, SSL_CTX *ssl_ctx)
            : raw_stream_(std::move(raw_stream))
            , ssl_engine_{ssl_ctx}
        {
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
                    case ssl_status::error: {
                        spdlog::debug("handshake error");
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
                        spdlog::debug("data: {}", to_hex_string({buf.data(), r}));
                        // if (!r) {
                        //     co_return r.error();
                        // }
                        spdlog::debug("start engine write");
                        auto res = ssl_engine_.write(buf.data(), r);
                        spdlog::debug("engine write {} bytes", res);
                        if (res <= 0) {
                            auto err = SSL_get_error(ssl, res);
                            co_return make_custom_error<void>(err, "SSL handshake failed");
                        }

                        break;
                    }
                    case ssl_status::want_write: {
                        spdlog::debug("wand write");
                        auto res = ssl_engine_.read(buf.data(), buf.size());
                        spdlog::debug("engine read {} bytes", res);
                        if (res <= 0) {
                            auto err = SSL_get_error(ssl, res);
                            co_return make_custom_error<void>(err, "SSL handshake failed");
                        }
                        spdlog::debug("start raw write");
                        res = co_await raw_stream_.send(buf, res);
                        spdlog::debug("raw write {} bytes", res);

                        break;
                    }
                }
            }
            co_return result<void>{};
        }


        auto send(std::span<uint8_t> buf, size_t len = -1, int flag = 0) ->  task<int> {
            if (len == -1) len = buf.size();
            auto res = SSL_write(ssl_engine_.ssl(), buf.data(), len);
            spdlog::debug("ssl write {} bytes", res);
            auto status = get_ssl_status(ssl_engine_.ssl(), res);
            spdlog::debug("status: {}", static_cast<int>(status));
            std::array<uint8_t, 4096> buf2{};
            res = ssl_engine_.read(buf2.data(), buf2.size());
            spdlog::debug("ssl read {} bytes", res);
            res = co_await raw_stream_.send(buf2, res);
            spdlog::debug("raw send {} bytes", res);
            co_return res;

        }

        auto recv(std::span<uint8_t> buf, size_t len = -1, int flag = 0) -> task<int> {
            if (len == -1) len = buf.size();
            auto res = co_await raw_stream_.recv({buf.data(), len}, len, flag);
            spdlog::debug("raw recv {} bytes", res);
            res = ssl_engine_.write(buf.data(), res);
            spdlog::debug("ssl write {} bytes", res);
            res = SSL_read(ssl_engine_.ssl(), buf.data(), res);
            spdlog::debug("ssl read {} bytes", res);
            co_return res;
        }

    private:


     private:
        RawStream raw_stream_;
        ssl_engine ssl_engine_;

        std::vector<uint8_t> read_buf_ = std::vector<uint8_t>(4096);
        std::vector<uint8_t> write_buf_ = std::vector<uint8_t>(4096);

    };
} // namespace kkp::ssl

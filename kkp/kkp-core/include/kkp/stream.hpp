#pragma once

#include <variant>
#include <kkp/net/socket.hpp>
#include <kkp/ssl/ssl_stream.hpp>

namespace kkp {

    template <kk_stream ...Streams>
    class base_stream : non_copy {
    public:
        using stream_type = std::variant<Streams...>;

        explicit base_stream(kk_stream auto stream) : stream_(std::move(stream)) {}

        base_stream(base_stream &&other) noexcept : stream_(std::move(other.stream_)) {}

        base_stream &operator=(base_stream &&other) noexcept {
            stream_ = std::move(other.stream_);
            return *this;
        }

        [[nodiscard]]
        auto get_varient(this auto &&self) noexcept -> auto && {
            return self.stream_;
        }

        template<kk_stream Stream>
        [[nodiscard]]
        auto get_stream(this auto &&self) noexcept -> auto && {
            return std::get<Stream>(self.stream_);
        }

        auto send(std::span<uint8_t> buf, int flags = 0) {
            return std::visit([&](auto &&stream) {
                return stream.send(buf, flags);
            }, stream_);
        }

        auto recv(std::span<uint8_t> buf, int flags = 0) {
            return std::visit([&](auto &&stream) {
                return stream.recv(buf, flags);
            }, stream_);
        }

        [[nodiscard]]
        auto is_alive() const noexcept -> bool {
            return std::visit([&](auto &&stream) {
                return stream.is_alive();
            }, stream_);
        }

    protected:
        stream_type stream_;
    };

    using stream = base_stream<net::socket, ssl::ssl_stream<net::socket>>;

} // namespace kkp

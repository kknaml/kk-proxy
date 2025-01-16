#pragma once

#include <kkp/net/address.hpp>

namespace kkp::net {

    class endpoint {
    public:

        endpoint() = default;

        explicit endpoint(net::address address) noexcept
            : address_{address}, domain_(address.ip_address()) {

        }

        explicit endpoint(net::address address, std::string_view domain) noexcept
            : address_{address}, domain_{domain} {}

        [[nodiscard]]
        auto port(this auto &&self) noexcept -> auto && {
            if (self.address_.is_v4()) {
                return self.address_.cast_v4().sin_port;
            }
            if (self.address_.is_v6()) {
                return self.address_.cast_v6().sin6_port;
            }
            throw std::runtime_error("Invalid address family");
        }

        [[nodiscard]]
        auto &&address(this auto &&self) noexcept {
            return self.address_;
        }

        operator auto &&(this auto &&self) noexcept {
            return self.address_;
        }
    private:
        net::address address_{};
        std::string domain_{};

    public:
        static auto from(std::string_view host, uint16_t port) -> endpoint {
            return endpoint{address::from(host, port), std::string{host}};
        }
    };
}

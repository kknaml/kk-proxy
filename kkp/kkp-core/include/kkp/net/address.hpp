#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <concepts>
#include <string_view>
#include <kkp/error_code.hpp>

namespace kkp::net {

    class address {
        friend result<address> parse_from_ip(std::string_view addr, uint16_t port) noexcept;
        friend result<address> parse_from_domain(std::string_view addr, uint16_t port) noexcept;
        address() = default;
    public:
        auto &&sockaddr(this auto &&self) noexcept {
            return self.addr_;
        }

        [[nodiscard]]
        bool is_v4() const noexcept {
            return addr_.sa_family == AF_INET;
        }

        [[nodiscard]]
        bool is_v6() const noexcept {
            return addr_.sa_family == AF_INET6;
        }

        auto &&cast_v4(this auto &&self) noexcept {
            using addr_type = std::conditional_t<
                std::is_const_v<std::remove_reference_t<decltype(self)>>,
                const ::sockaddr_in &,
                ::sockaddr_in &
             >;
            return reinterpret_cast<addr_type>(self.addr_);
        }

        auto &&cast_v6(this auto &&self) noexcept {
             using addr_type = std::conditional_t<
                std::is_const_v<std::remove_reference_t<decltype(self)>>,
                const ::sockaddr_in6 &,
                ::sockaddr_in6 &
             >;
            return reinterpret_cast<addr_type>(self.addr_);
        }

        std::string ip_address() const noexcept;

    private:
        ::sockaddr addr_{};

    public:
        static result<address> from(std::string_view addr, uint16_t port) noexcept;
    };
}
#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <concepts>
#include <string_view>
#include <kkp/error_code.hpp>

namespace kkp::net {

    auto init_addr_from_fd(int fd, sockaddr *addr) -> void;

    class address {
    public:
        address() = default;

        explicit address(const sockaddr &addr) noexcept
                : addr_(addr) {}

        auto &&sockaddr(this auto &&self) noexcept {
            return self.addr_;
        }

        [[nodiscard]]
        auto is_v4() const noexcept -> bool {
            return addr_.sa_family == AF_INET;
        }

        [[nodiscard]]
        auto is_v6() const noexcept -> bool {
            return addr_.sa_family == AF_INET6;
        }

        auto &&cast_v4(this auto &&self) noexcept {
            // KKP_DEBUG_ASSERT(self.is_v4());
            using addr_type = std::conditional_t<
                std::is_const_v<std::remove_reference_t<decltype(self)>>,
                const ::sockaddr_in &,
                ::sockaddr_in &
             >;
            return reinterpret_cast<addr_type>(self.addr_);
        }

        auto &&cast_v6(this auto &&self) noexcept {
            // KKP_DEBUG_ASSERT(self.is_v6());
            using addr_type = std::conditional_t<
            std::is_const_v<std::remove_reference_t<decltype(self)>>,
            const ::sockaddr_in6 &,
            ::sockaddr_in6 &
            >;
            return reinterpret_cast<addr_type>(self.addr_);
        }

        [[nodiscard]]
        std::string ip_address() const noexcept;

    private:
        ::sockaddr addr_{};

    public:
        static address from(std::string_view addr, uint16_t port = 0) noexcept(false);
    };
}
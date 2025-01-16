#include <kkp/net/address.hpp>
#include <netdb.h>
#include <stdexcept>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>

namespace  kkp::net {

    auto init_addr_from_fd(int fd, sockaddr *addr) -> void {
        socklen_t len = sizeof(sockaddr_storage);
        sockaddr_storage temp_addr{};
        if (getsockname(fd, reinterpret_cast<sockaddr *>(&temp_addr), &len) == -1) {
            //return std::unexpected(std::error_code(errno, std::generic_category()));
            throw std::runtime_error("getsockname failed: " + std::string(strerror(errno)));
        }
        if (temp_addr.ss_family == AF_INET) {
            memcpy(addr, &temp_addr, sizeof(sockaddr_in));
            return;
        } else if (temp_addr.ss_family == AF_INET6) {
            memcpy(addr, &temp_addr, sizeof(sockaddr_in6));
            return;
        }
        throw std::runtime_error("Unknown address family");
    }


    result<address> parse_from_ip(std::string_view addr, uint16_t port) noexcept {
        address result{};
        if (inet_pton(AF_INET, addr.data(), &result.cast_v4().sin_addr) == 1) {
            auto &v4 = result.cast_v4();
            v4.sin_family = AF_INET;
            v4.sin_port = htons(port);
            return result;
        } else if (inet_pton(AF_INET6, addr.data(), &result.cast_v6().sin6_addr) == 1) {
            auto &v6 = result.cast_v6();
            v6.sin6_family = AF_INET6;
            v6.sin6_port = htons(port);
            return result;
        }
        return std::unexpected(std::error_code(0, std::generic_category()));
    }

    address parse_from_domain(std::string_view addr, uint16_t port) {
        address result{};
        addrinfo hints{};

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM; // TCP
        hints.ai_flags = AI_ADDRCONFIG; // only return IPV4 or IPV6

        addrinfo* res{};
        int ret = getaddrinfo(addr.data(), nullptr, &hints, &res);
        if (ret != 0) {
            // return make_custom_error<address>(ret, "getaddrinfo failed: " + std::string(gai_strerror(ret)) + " for " + std::string(addr));
            throw std::runtime_error("getaddrinfo failed: " + std::string(gai_strerror(ret)) + " for " + std::string(addr));
        }
        KKP_DEFER([res] {
            freeaddrinfo(res);
        });


        for (auto *ptr = res; ptr != nullptr; ptr = ptr->ai_next) {
            if (ptr->ai_family == AF_INET) { // IPV4
                auto &v4 = result.cast_v4();
                memcpy(&v4, ptr->ai_addr, sizeof(v4));
                v4.sin_port = htons(port);
                result.sockaddr().sa_family = AF_INET;
                return result;
            } else if (ptr->ai_family == AF_INET6) { // IPV6
                auto &v6 = result.cast_v6();
                memcpy(&v6, ptr->ai_addr, sizeof(v6));
                v6.sin6_port = htons(port);
                result.sockaddr().sa_family = AF_INET6;
                return result;
            }
        }

        throw std::runtime_error("No valid address found for: " + std::string(addr));
        // return make_custom_error<address>(0, "No valid address found for: " + std::string(addr));
    }


    std::string address::ip_address() const noexcept {
        if (is_v4()) {
            auto &v4 = cast_v4();
            char buf[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &v4.sin_addr, buf, sizeof(buf));
            return buf;
        } else if (is_v6()) {
            auto &v6 = cast_v6();
            char buf[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &v6.sin6_addr, buf, sizeof(buf));
            return buf;
        }
        return "error";
    }

    address address::from(std::string_view addr, uint16_t port) {
        auto res = parse_from_ip(addr, port);
        if (res) {
            return res.value();
        }
        return parse_from_domain(addr, port);
    }

} // namespace kkp::net

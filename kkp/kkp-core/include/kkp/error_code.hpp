#pragma once

#include <expected>
#include <system_error>
#include <kkp/kkp_traits.hpp>
#include <utility>

namespace kkp {

    constexpr inline int TASK_FINISH_MAGIC = 0x123456ab;
    constexpr inline int E_TASK_FINISH = 10086;
    constexpr inline int E_TASK_CANCEL = 10087;
    constexpr inline int E_TASK_TIMEOUT = 10088;
    constexpr inline int E_TASK_ERROR = 10089;

    constexpr inline int E_SSL_HANDSHAKE_FAILED = 10090;
    constexpr inline int E_SSL_HANDSHAKE_SOCKET_READ_EOF = 10091;


    class kkp_error_category final : public std::error_category, non_copy {

    public:
        kkp_error_category() = default;

        [[nodiscard]]
        const char *name() const noexcept override {
            return "kkp_error";
        }

        [[nodiscard]]
        std::string message(int ev) const override;

    };

    extern kkp_error_category kkp_error_category_instance;

    template<typename T>
    constexpr auto make_sys_error(int code) -> result<T> {
        return std::unexpected(std::error_code(code, std::system_category()));
    }

    template<typename T>
    auto make_custom_error(int code) -> result<T> {
        return std::unexpected(std::error_code{code, kkp_error_category_instance});
    }

    template<typename T>
    constexpr auto make_kkp_error(int code) -> result<T>  {
        if (code >= 10086) {
            return make_custom_error<T>(code);
        } else {
            return make_sys_error<T>(code);
        }
    }

} // namespace kkp

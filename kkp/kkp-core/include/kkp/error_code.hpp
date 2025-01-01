#pragma once

#include <expected>
#include <system_error>
#include <kkp/kkp_traits.hpp>

namespace kkp {

    constexpr inline int E_TASK_FINISH = 10086;
    constexpr inline int TASK_FINISH_MAGIC = 0x123456ab;

    constexpr inline int E_TASK_CANCEL = 10087;

    class kkp_error_category final : public std::error_category {

    public:
        explicit kkp_error_category(std::string_view msg) : message_(msg) {}
        [[nodiscard]]
        const char* name() const noexcept override {
            return "kkp_error";
        }

        [[nodiscard]]
        std::string message(int ev) const override {
            return  message_;
        }
    private:
        std::string message_;
    };

    template<typename T>
    constexpr result<T> make_sys_error(int code) {
        return std::unexpected(std::error_code(-code, std::system_category()));
    }

    template<typename T>
    result<T> make_custom_error(int code, std::string_view msg) {
        return std::unexpected(std::error_code(code, kkp_error_category(msg)));
    }

} // namespace kkp

#pragma once

#include <kkp/kkp_macros.hpp>
#include <expected>
#include <system_error>
#include <spdlog/spdlog.h>
#include <concepts>

namespace kkp {


    class non_copy {
    public:
        constexpr non_copy() = default;
        non_copy(const non_copy &) = delete;
        non_copy &operator=(const non_copy &) = delete;
    };

    struct unit_t final {};
    constexpr inline unit_t unit{};

    template<typename T>
    using result = std::expected<T, std::error_code>;

    template<typename F, typename R, typename ...Args>
    concept func = std::is_invocable_r_v<R, F, Args...>;

    template<typename F, typename ...Args>
    concept action = func<F, void, Args...>;

    template<typename T>
    concept kk_stream = requires(T t) {
        { t.send(std::declval<std::span<std::uint8_t>>(), 0) }; // span, flag
        { t.recv(std::declval<std::span<std::uint8_t>>(), 0) }; // span, flag
        { t.is_alive() } -> std::same_as<bool>;
        t.close();
    };

    template<typename ...Ts>
    struct overloads : Ts... {
        using Ts::operator()...;
    };

    // template<typename T = void>
    // struct defer {
    //     constexpr defer() = default;
    // };

    template<action F>
    struct defer {
        explicit defer(F &&fn) : destructor_fn_(fn) {}

        F destructor_fn_;

        ~defer() noexcept(noexcept(destructor_fn_())) {
            destructor_fn_();
        }
    };


#define KKP_DEFER(fn) kkp::defer CONCATENATE(_defer_, __LINE__)(fn)

    template<typename T, typename Visitor>
    requires
    std::invocable<Visitor, T>
    && std::invocable<Visitor>
    && std::is_convertible_v<std::invoke_result_t<Visitor, T>, std::invoke_result_t<Visitor>>
    && std::is_convertible_v<std::invoke_result_t<Visitor>, std::invoke_result_t<Visitor, T>>
    auto match(Visitor &&visitor, std::optional<T> &op)
    noexcept(noexcept(visitor()) && noexcept(visitor(std::declval<T>()))) -> decltype(auto) {
        if (op.has_value()) {
            return std::invoke(visitor, *op);
        } else {
            return std::invoke(visitor);
        }
    }

} // namespace kkp

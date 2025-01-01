#pragma once

#include <kkp/kkp_traits.hpp>
#include <coroutine>
#include <concepts>
#include <cstdint>

namespace kkp::coro {

    class base_kk_awaiter {};

    template<typename T, typename R>
    concept awaitable = requires (T &t) {
        { t.await_ready() } -> std::same_as<bool>;
        { t.await_resume() } -> std::convertible_to<R>;
    };

    template<typename T, typename R>
    concept awaitable_result = awaitable<T, result<R>>;


    namespace flag {

        constexpr inline int daemon = 1 << 0;
        constexpr inline int sub_job = 1 << 1;


        constexpr inline bool is_daemon(uint64_t flag) noexcept {
            return flag & daemon;
        }

        inline void set_daemon(uint64_t &flag, bool value) noexcept {
            if (value) {
                flag |= daemon;
            } else {
                flag &= ~daemon;
            }
        }

        constexpr inline bool is_sub_job(uint64_t flag) noexcept {
            return flag & sub_job;
        }

        inline void set_sub_job(uint64_t &flag, bool value) noexcept {
            if (value) {
                flag |= sub_job;
            } else {
                flag &= sub_job;
            }
        }
    } // namespace flag

} // namespace kkp::coro

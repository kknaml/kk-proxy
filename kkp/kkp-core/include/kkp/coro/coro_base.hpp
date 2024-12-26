#pragma once

#include  <kkp/kkp_traits.hpp>
#include <coroutine>
#include <concepts>

namespace kkp::coro {

    class base_kk_awaiter {};

    template<typename T, typename R>
    concept awaitable = requires (T &t) {
        { t.await_ready() } -> std::same_as<bool>;
        { t.await_resume() } -> std::convertible_to<R>;
    };

    template<typename T, typename R>
    concept awaitable_result = awaitable<T, result<R>>;

}

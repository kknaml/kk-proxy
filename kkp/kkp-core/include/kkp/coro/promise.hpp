#pragma once

#include <kkp/coro/coro_base.hpp>
#include <functional>
#include <vector>
#include <optional>

namespace kkp::coro {

    template<typename T>
    class task;

    template<typename T>
    class promise : public base_kk_awaiter, non_copy {
        using promise_t = typename task<T>::promise_type;

    public:
        explicit promise(std::coroutine_handle<promise_t> task_handle) noexcept
            : task_handle_(task_handle) {}

        bool await_ready() noexcept {
            return false;
        }

        template<typename Promise>
        void await_suspend(std::coroutine_handle<Promise> caller) noexcept {


        }

        T await_resume() noexcept {
            return std::move(task_handle_.promise().get_value());
        }

    private:
        std::coroutine_handle<promise_t> task_handle_;
    };



} // namespace kkp::coro
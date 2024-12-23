#pragma once

#include <coroutine>
#include <kkp/kkp_traits.hpp>
#include <exception>
#include <optional>
#include <utility>
#include <concepts>

#include "coro_base.hpp"

namespace kkp::coro {

    template<typename T = void>
    class task;

    namespace detail {

        struct final_task_awaiter {
            constexpr bool await_ready() const noexcept {
                return false;
            }

            template<typename Promise>
            std::coroutine_handle<> await_suspend(std::coroutine_handle<Promise> callee) const noexcept {
                if (callee.promise().caller_) {
                    return callee.promise().caller_;
                }
                if (callee.promise().ex_) {
                    // TODO handle exception
                }
                callee.destroy();
                return std::noop_coroutine();
            }

            constexpr void await_resume() const noexcept {}
        };

        template<typename Promise>
        struct base_task_awaiter {
            std::coroutine_handle<Promise> callee_;

            bool await_ready() const noexcept {
                return !callee_ || callee_.done();
            }

            std::coroutine_handle<> await_suspend(std::coroutine_handle<Promise> caller) {
                callee_.promise().caller_ = caller;
                return callee_;
            }

        };

        struct base_task_promise {

            constexpr std::suspend_always initial_suspend() const noexcept {
                return {};
            }

            auto final_suspend() const noexcept {
                return final_task_awaiter{};
            }

            void unhandled_exception() noexcept {
                ex_ = std::current_exception();
            }

            void check_error() const {
                if (ex_) {
                    std::rethrow_exception(ex_);
                }
            }

            template<typename Awaiter>
            requires std::is_base_of_v<base_kk_awaiter, Awaiter>
            auto &&await_transform(Awaiter &&awaiter) const noexcept {
                return std::forward<Awaiter>(awaiter);
            }

            std::coroutine_handle<> caller_ {nullptr};
            std::exception_ptr ex_{nullptr};
        };

        template<typename T>
        struct task_promise : base_task_promise {

            task<T> get_return_object();

            auto await_transform(task<T> &&task) {
                struct awaiter : base_task_awaiter<task_promise> {
                    decltype(auto) await_resume() {
                        return std::move(this->callee_.promise()).get_value();
                    }

                };
                return awaiter{task.handle()};
            }

            auto await_transform(task<T> &task) {
                struct awaiter : base_task_awaiter<decltype(*this)> {
                    decltype(auto) await_resume() {
                        return this->callee_.promise().get_value();
                    }
                };
                return awaiter{task.handle()};
            }

            void return_value(auto &&value) noexcept {
                value_ = std::forward<decltype(value)>(value);
            }

            T &get_value() & {
                check_error();
                return value_.value();
            }

            T get_value() && {
                check_error();
                return std::move(value_.value());
            }

            std::optional<T> value_{};
        };

        template<>
        struct task_promise<void> : base_task_promise {

            task<> get_return_object();

            template<typename Void>
            requires std::is_same_v<Void, void>
            auto await_transform(const task<Void> &task) {
                struct awaiter : base_task_awaiter<task_promise> {
                    decltype(auto) await_resume() {
                        return this->callee_.promise().get_value();
                    }

                };
                return awaiter{task.handle()};
            }

            void return_void() const noexcept {

            }

            void get_value() const {
               check_error();
            }
        };

    } // namespace detail


    template<typename T>
    class task : public non_copy {
    public:
        using promise_type = detail::task_promise<T>;

        explicit task(std::coroutine_handle<promise_type> handle) : handle_(handle) {}

        task(task &&other) noexcept : handle_(std::exchange(other.handle_, nullptr)) {}

        task &operator=(task &&other) noexcept {
            if (this != &other) {
                if (handle_) {
                    handle_.destroy();
                }
                handle_ = std::exchange(other.handle_, nullptr);
            }
            return *this;
        }

        ~task() {
            if (handle_) {
                handle_.destroy();
            }
        }

        [[nodiscard]]
        auto handle() const noexcept {
            return handle_;
        }

        [[nodiscard]]
        auto take_handle() noexcept {
            return std::exchange(handle_, nullptr);
        }


    private:
        std::coroutine_handle<promise_type> handle_;
    };

    template<typename T>
    task<T> detail::task_promise<T>::get_return_object() {
        return task<T>{std::coroutine_handle<task_promise>::from_promise(*this)};
    }

    inline task<> detail::task_promise<void>::get_return_object() {
        return task{std::coroutine_handle<task_promise>::from_promise(*this)};
    }

} // namespace kkp::coro
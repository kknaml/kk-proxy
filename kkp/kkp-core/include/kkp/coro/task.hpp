#pragma once

#include <coroutine>
#include <kkp/kkp_traits.hpp>
#include <kkp/coro/promise.hpp>
#include <exception>
#include <optional>
#include <ostream>
#include <utility>
#include <concepts>
#include <print>

#include "coro_base.hpp"

namespace kkp {
    class io_context;
}

namespace kkp::coro {

    template<typename T = void>
    class task;

    void task_finish(io_context *context) noexcept;

    struct context_awaiter : base_kk_awaiter {
        constexpr bool await_ready() const noexcept {
            return false;
        }

        template<typename Promise>
        bool await_suspend(std::coroutine_handle<Promise> callee) noexcept {
            this->context_ = static_cast<io_context *>(callee.promise().context_);
            return false;
        }

        io_context *await_resume() const noexcept {
            return this->context_;
        }

        io_context *context_{};
    };

    inline context_awaiter this_context{};

    namespace detail {

        struct final_task_awaiter {
            constexpr bool await_ready() const noexcept {
                return false;
            }

            template<typename Promise>
            std::coroutine_handle<> await_suspend(std::coroutine_handle<Promise> callee) const noexcept {
                spdlog::debug("task final auspend");
                if (callee.promise().caller_) {
                    spdlog::debug("return caller");
                    return callee.promise().caller_;
                }
                if (callee.promise().ex_) {
                    // TODO handle exception
                    spdlog::error("callee error ");
                    try {
                        std::rethrow_exception(callee.promise().ex_);
                    } catch (const std::exception &e) {
                        spdlog::error("task exception: {}", e.what());
                    } catch (...) {
                        spdlog::error("task exception: unknown");
                    }
                }
                if (flag::is_daemon(callee.promise().flags_)) [[unlikely]] {
                    spdlog::debug("is deamon xxxxxxxxxxxxx");
                    return std::noop_coroutine();
                } else {
                    task_finish(static_cast<io_context *>(callee.promise().context_));
                    spdlog::debug("destroy");
                    callee.destroy();
                    return std::noop_coroutine();
                }
            }

            constexpr void await_resume() const noexcept {}
        };

        template<typename Promise>
        struct base_task_awaiter {
            std::coroutine_handle<Promise> callee_;
            bool await_ready() const noexcept {
                spdlog::debug("await ready");
                return !callee_ || callee_.done();
            }

            template<typename Promise2>
            std::coroutine_handle<> await_suspend(std::coroutine_handle<Promise2> caller) {
                auto &callee_promise = callee_.promise();
                if (flag::is_daemon(callee_promise.flags_)) {
                    spdlog::debug("is deamon: {}", caller.done());

                    callee_promise.caller_ = caller;
                    callee_promise.context_ = caller.promise().context_;
                    return std::noop_coroutine();
                } else {
                    callee_promise.caller_ = caller;
                    callee_promise.context_ = caller.promise().context_;
                    return callee_;
                }
            }

        };

        struct base_task_promise {

            void *operator new(std::size_t size) {
                spdlog::debug("task allocate size: {}", size);
                try {
                    return ::operator new(size);
                } catch (const std::bad_alloc &e) {
                    spdlog::error("task allocate error: {}", e.what());
                    throw;
                }
            }

            void operator delete(void *ptr, std::size_t size) {
                spdlog::debug("task free  size: {}", size);
                ::operator delete(ptr);
            }

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
            requires std::is_base_of_v<base_kk_awaiter, std::remove_reference_t<Awaiter>>
            auto &&await_transform(Awaiter &&awaiter) const noexcept {
                return std::forward<Awaiter>(awaiter);
            }

            template<typename T>
            auto await_transform(this auto &&self, task<T> &&task) {
                struct awaiter : base_task_awaiter<typename kkp::coro::task<T>::promise_type> {
                    decltype(auto) await_resume() {
                        return std::move(this->callee_.promise()).get_value();
                    }
                };
                return awaiter{task.take_handle()};
            }

            template<typename T>
            auto await_transform(this auto &&self, task<T> &task) {
                struct awaiter : base_task_awaiter<typename kkp::coro::task<T>::promise_type> {
                    decltype(auto) await_resume() {
                        return this->callee_.promise().get_value();
                    }
                };
                return awaiter{task.handle()};
            }

            std::coroutine_handle<> caller_ {nullptr};
            std::exception_ptr ex_{nullptr};
            void *context_{nullptr};
            uint64_t flags_ {0};
        };

        template<typename T>
        struct task_promise : base_task_promise {

            task<T> get_return_object();

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

            bool has_value() const noexcept {
                return value_.has_value();
            }

            std::optional<T> value_{};
        };

        template<>
        struct task_promise<void> : base_task_promise {

            task<> get_return_object();

            // template<typename Void>
            // requires std::is_same_v<Void, void>
            // auto await_transform(const task<Void> &task) {
            //     struct awaiter : base_task_awaiter<task_promise> {
            //         decltype(auto) await_resume() {
            //             return this->callee_.promise().get_value();
            //         }
            //
            //     };
            //     return awaiter{task.handle()};
            // }

            void return_void() noexcept {
                has_value_ = true;
            }

            void get_value() const {
               check_error();
            }

            bool has_value() const noexcept {
                return has_value_;
            }

            bool has_value_{false};
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

        void set_context(void *context) {
            handle_.promise().context_ = context;
        }

        void set_daemon(bool is_daemon) {
            flag::set_daemon(handle_.promise().flags_, is_daemon);
        }

        void set_sub_job(bool value) {
            flag::set_sub_job(handle().promise().flags_, value);
        }

    private:
        std::coroutine_handle<promise_type> handle_;

    public:
        // template<typename ...Args>
        // static task<std::tuple<Args...>> all(task<Args>... tasks) {
        //
        // }
    };

    template<typename T>
    task<T> detail::task_promise<T>::get_return_object() {
        return task<T>{std::coroutine_handle<task_promise>::from_promise(*this)};
    }

    inline task<> detail::task_promise<void>::get_return_object() {
        return task{std::coroutine_handle<task_promise>::from_promise(*this)};
    }

} // namespace kkp::coro
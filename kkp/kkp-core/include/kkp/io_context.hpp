#pragma once

#include <thread_pool/thread_pool.h>
#include <kkp/kkp_traits.hpp>
#include <kkp/uring/uring_loop.hpp>
#include <kkp/coro/task.hpp>

namespace kkp {

    using kkp::coro::task;

    class io_context : public non_copy {
    public:
        explicit io_context(int concurrency_n = 1) : ring_(), loop_(concurrency_n, &ring_) {
            io_uring_queue_init(32, &ring_, 0);
        }

        // void spawn(task<> &&task) {
        //
        // }

        void block_run(task<> &&task) {
            task.set_context(this);
            task.set_daemon(false);
            task.take_handle()();
            loop_.run();
        }

        template<typename F>
        requires std::is_invocable_r_v<task<>, F>
        void block_run(F &&f) {
            block_run(f());
        }

        template<typename T>
        task<T> launch(task<T> &&task) {
            task.set_context(this);
            task.set_daemon(true);
            auto handle = task.take_handle();
            handle();
            return coro::task<T>{handle};
        }

        void finish() {
            spdlog::debug("finish task1");
            auto *sqe = io_uring_get_sqe(&ring_);
            auto *data = new uring::io_data{
                nullptr, 0x123456ab
            };
            io_uring_sqe_set_data(sqe, data);
            io_uring_prep_nop(sqe);
            io_uring_submit(&ring_);
            spdlog::debug("finish task2");
        }

        io_uring *ring() noexcept {
            return &ring_;
        }

    private:
        io_uring ring_;
        uring::uring_loop loop_;
    };

    template<typename T>
    T run_blocking(task<T> &&task) {
        // TODOs
        return std::declval<T>();
    }

};ll

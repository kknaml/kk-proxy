#pragma once

#include <kkp/uring/io_data.hpp>
#include <liburing.h>
#include <print>

namespace kkp::uring {


    class uring_loop {

    public:

         uring_loop(int concurrency_n, io_uring *ring) noexcept
            : ring_(ring), pool_(concurrency_n) {
        }

        void run() noexcept {
            running_ = true;
            // __kernel_timespec ts{};
            // // 200 ms
            // ts.tv_nsec = 200 * 1000 * 1000;
            io_uring_cqe *cqe{};
            while (running_) {
                // int res = io_uring_wait_cqes(ring_, &cqe, 1, &ts, nullptr);
                // std::println("enter loop");
                int res = io_uring_wait_cqe(ring_, &cqe);
                // std::println("wait res: {}", res);
                if (res != 0) [[unlikely]] {
                    if (res == -ETIME && cqe != nullptr) { // prep_timeout
                        resume(cqe);
                        continue;
                    } else {
                        // TODO
                    }
                } else {
                    resume(cqe);
                }
                if (cqe != nullptr) [[likely]] {
                    io_uring_cqe_seen(ring_, cqe);
                }
            }
        }

    private:
        void resume(io_uring_cqe *cqe) noexcept {
            auto *data = reinterpret_cast<io_data *>(cqe->user_data);
            if (data == nullptr) [[unlikely]] {
                // TODO
                return;
            }
            // std::println("CQE res: {}", cqe->res);
            if (data->handle_ == nullptr && data->result_ == 0x123456ab) {
                running_ = false;
                delete data;
                return;
            }
            data->result_ = cqe->res;
            pool_.enqueue_detach(data->handle_);
        }

    private:
        bool running_{false};
        io_uring *ring_;
        dp::thread_pool<> pool_;
    };
} // namespace kkp::uring

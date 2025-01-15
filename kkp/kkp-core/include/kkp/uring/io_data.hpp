#pragma once

#include <coroutine>

namespace kkp::uring {

    struct io_data {
        std::coroutine_handle<> handle_{nullptr};
        int result_{};
    };

    inline auto operator==(const io_data &lhs, const io_data &rhs) noexcept -> bool {
        return lhs.handle_ == rhs.handle_ && lhs.result_ == rhs.result_;
    }

    extern const io_data io_data_completed;

}

#pragma once

#include <coroutine>

namespace kkp::uring {

    struct io_data {
        std::coroutine_handle<> handle_{nullptr};
        int result_{};
    };

}

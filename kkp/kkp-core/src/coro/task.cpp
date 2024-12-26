#include <kkp/io_context.hpp>
#include <kkp/coro/task.hpp>

namespace kkp::coro {

    void task_finish(io_context *context) noexcept {
        if (context == nullptr) {
            return;
        }
        context->finish();
    }

}
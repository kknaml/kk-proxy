#include <liburing.h>
#include <print>
#include <cstring>


int main(int argc, char *argv[]) {
    io_uring ring{};
    io_uring_queue_init(32, &ring, 0);

    __kernel_timespec ts{};
    // 200ms
    ts.tv_nsec = 200 * 1000 * 1000;
    io_uring_cqe *cqe = (io_uring_cqe *)123456789;
    auto ret = io_uring_wait_cqes(&ring, &cqe, 1, &ts, nullptr);
    std::println("ret: {}", ret);
    if (ret != 0) {
        // print error message
        std::println("error: {}", strerror(-ret));
        std::println("cqe: {}", (void *)cqe);
        if (ret == -ETIME) {
            std::println("timeout");
        }

    }
}

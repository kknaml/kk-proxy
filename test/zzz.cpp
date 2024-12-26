#include <kkp/kkp_traits.hpp>
#include <kkp/coro/task.hpp>
#include <print>
#include <thread_pool/thread_pool.h>
#include <kkp/uring/base_uring_awaiter.hpp>
#include <kkp/io_context.hpp>
#include <kkp/uring/uring_loop.hpp>
#include <kkp/uring/awaiter/all.hpp>
#include <kkp/fs/file.hpp>
#include <kkp/utils.hpp>
#include <pthread.h>

using kkp::coro::task;
using kkp::io_context;

task<int> bar() {
    // co_await std::suspend_always{};
    co_return 20;
}

task<> foo() {
    auto r = co_await bar();
    std::println("r = {}", r);
    co_return;
}

void print_thread_name() {
    char name[16];
    if (pthread_getname_np(pthread_self(), name, sizeof(name)) == 0) {
        std::println("thread name: {}, {}", name, pthread_self());
    } else {
        std::println("get thread name failed");
    }
}

int main(int argc, char *argv[]) {

    io_context context{1};
    context.block_run([&] -> task<> {
        auto file = co_await kkp::fs::file::of("CMakeLists.txt");
        print_thread_name();
        std::array<uint8_t, 1024> buf{};
        auto result = co_await file.read(buf);
        print_thread_name();
        if (result.has_value()) {
            std::println("read {} bytes", result.value());
            //std::println("buf: {}", reinterpret_cast<char *>(buf.data()));
        } else {
            std::println("error: {}", result.error().message());
        }
        co_await kkp::delay(context.ring(), 5000);
        co_return;
    });
}

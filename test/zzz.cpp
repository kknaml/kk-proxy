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
#include <kkp/net/address.hpp>
#include <pthread.h>
#include <arpa/inet.h>
#include <kkp/net/socket.hpp>


using kkp::coro::task;
using kkp::io_context;

task<int> bar() {
    // co_await std::suspend_always{};
    co_return 22;
}

task<> foo() {
    auto r = co_await bar();
    spdlog::debug("r = {}", r);
    co_return;
}

void print_thread_name() {
    char name[16];
    if (pthread_getname_np(pthread_self(), name, sizeof(name)) == 0) {
        spdlog::debug("thread name: {}, {}", name, pthread_self());
    } else {
        spdlog::debug("get thread name failed");
    }
}

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::debug);

    spdlog::set_pattern("[%H:%M:%S %f] [%n] [%^%l%$] [thread %t] %v");
    auto address = kkp::net::address::from("inory.me", 80);
    if (address.has_value()) {
        auto &value = address.value();
        spdlog::debug("address: {}", value.ip_address());

    } else {
        spdlog::debug("error: {}", address.error().message());
    }

    io_context context{1};
    context.block_run([&context] -> task<> {

        auto p = context.launch([&context] -> task<int> {
            spdlog::debug("hello hello1");
            auto r = co_await kkp::delay(context.ring(), 5000);
            if (r.has_value()) {
                spdlog::debug("delay success");
            } else {
                spdlog::error("delay failed: {}", r.error().message());
            }
            spdlog::debug("hello hello2");
            co_return co_await bar();
        }());
        auto file = co_await kkp::fs::file::of("CMakeLists.txt");
        std::array<uint8_t, 1024> buf{};
        auto result = co_await file.read(buf);
        if (result.has_value()) {
            spdlog::debug("read {} bytes", result.value());
            //spdlog::debug("buf: {}", reinterpret_cast<char *>(buf.data()));
        } else {
            spdlog::error("error: {}", result.error().message());
        }
        co_await kkp::delay(context.ring(), 500);
        spdlog::debug("QAQ");
        spdlog::debug("p is {}", co_await std::move(p));
        co_return;
    });
}

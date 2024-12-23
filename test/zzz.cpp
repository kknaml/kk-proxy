#include <kkp/kkp_traits.hpp>
#include <kkp/coro/task.hpp>
#include <print>

kkp::coro::task<int> bar() {
    // co_await std::suspend_always{};
    co_return 20;
}

kkp::coro::task<int> foo() {
    auto r = co_await bar();
    std::println("r = {}", r);
    co_return 20;
}

int main(int argc, char *argv[]) {
    auto task = foo();
    task.take_handle()();

}

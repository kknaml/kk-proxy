#include <map>
#include <print>
#include <kkp/server//kkp_server.hpp>

int main() {
    std::multimap<int, int> m;
    m.insert({10, 20});
    m.insert({20, 50});
    m.insert({10, 30});

    auto f = m.find(10);
    if (f != m.end()) {
        std::println("found first: {}, second: {}", f->first, f->second);
    } else {
        std::println("not found");
    }
}
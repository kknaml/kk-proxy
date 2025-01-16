#include <kkp/io_context.hpp>

namespace kkp {
    std::string to_hex_string(std::span<unsigned char> str) noexcept {
        std::string result;
        result.reserve(str.size() * 3);
        for (auto c: str) {
            result.append(std::format("{:02X} ", c));
        }
        return result;
    }

}
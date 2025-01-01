#pragma once

#include <expected>
#include <system_error>
#include <spdlog/spdlog.h>

namespace kkp {

    class non_copy {
    public:
        constexpr non_copy() = default;
        non_copy(const non_copy &) = delete;
        non_copy &operator=(const non_copy &) = delete;
    };

    struct unit_t final {};
    constexpr inline unit_t unit{};

    template<typename T>
    using result = std::expected<T, std::error_code>;


} // namespace kkp

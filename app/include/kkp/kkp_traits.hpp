#pragma once

namespace kkp {

    class non_copy {
    public:
        constexpr non_copy() = default;
        non_copy(const non_copy &) = delete;
        non_copy &operator=(const non_copy &) = delete;
    };

} // namespace kkp

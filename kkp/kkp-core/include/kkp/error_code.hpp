#pragma once

#include <expected>
#include <system_error>
#include <kkp/kkp_traits.hpp>

namespace kkp {

    constexpr inline int E_TASK_FINISH = 10086;
    constexpr inline int TASK_FINISH_MAGIC = 0x123456ab;

    constexpr inline int E_TASK_CANCEL = 10087;


    template<typename T>
    constexpr result<T> make_sys_error(int code) {
        return std::unexpected(std::error_code(-code, std::system_category()));
    }
} // namespace kkp

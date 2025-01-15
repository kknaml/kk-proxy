#include <kkp/error_code.hpp>

namespace kkp {

    kkp_error_category kkp_error_category_instance{};

#define make_msg_pair(code, msg) \
    std::pair<int, std::string>{(code), msg}

    static constexpr auto error_msg_map = std::array {
        make_msg_pair(E_TASK_FINISH, "Task finish"),
        make_msg_pair(E_TASK_CANCEL, "Task cancel"),
    };

#undef make_msg_pair

    std::string kkp_error_category::message(int ev) const {
        for (const auto &[code, msg] : error_msg_map) {
            if (code == ev) {
                return msg;
            }
        }
        return std::format("Unknown error code: {}", ev);
    }

}
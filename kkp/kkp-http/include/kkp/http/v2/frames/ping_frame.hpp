#pragma once

#include <kkp/http/v2/frames/frame.hpp>

namespace kkp::http::v2 {

    class ping_frame : public frame {
    public:
        ping_frame() = default;
        ping_frame(bool ack, std::span<const uint8_t, 8> payload) : ack_(ack) {
            memcpy(payload_, payload.data(), 8);
        }

        auto is_ack() const noexcept -> bool {
            return ack_;
        }

        auto payload(this auto &&self) noexcept -> auto && {
            return self.payload_;
        }

    private:
        bool ack_{};
        uint8_t payload_ [8]{};

    public:
        static constexpr uint8_t type = 0x6;
        static constexpr int length = 8;
    };
}
#pragma once

#include <kkp/http/http_common.hpp>
#include <kkp/kkp_traits.hpp>
#include <kkp/stream.hpp>

namespace kkp::http {

    enum class http_version {
        http_1_0,
        http_1_1,
        http_2_0,
        quic
    };

    class base_session : non_copy {
    public:

        explicit base_session(stream stream) : underlying_stream_(std::move(stream)) {

        }

        [[nodiscard]]
        virtual auto is_alive() const -> bool {
            return underlying_stream_.is_alive();
        }

        [[nodiscard]]
        virtual auto http_version() const -> http_version = 0;

        [[nodiscard]]
        auto underlying_stream(this auto &&self) -> auto && {
            return self.underlying_stream_;
        }

        virtual ~base_session() = default;

    protected:
        stream underlying_stream_;
        bool keep_alive_{true};
    };

    using http_session = std::unique_ptr<base_session>;

};
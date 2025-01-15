#pragma once

#include <kkp/http/http_common.hpp>
#include <kkp/kkp_traits.hpp>

namespace kkp::http {

    template<kk_stream Stream>
    class base_session {

    private:
        Stream underlying_stream_;
    };

};
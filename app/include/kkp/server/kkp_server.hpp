#pragma once


#include "kkp/kkp_common.hpp"

namespace kkp {

    class server {
    public:
        explicit server(int port = 0) : m_port(port) {};

    private:
        int m_port;
    };
} // namespace kkp

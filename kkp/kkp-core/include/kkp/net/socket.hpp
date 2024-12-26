#pragma once

namespace kkp::net {

    namespace detail {

        template<typename SocketType>
        class base_socket {

        };

        class server_socket {

            void bind() {}
            void listen() {}


        };

    } // namespace detail

} // namespace kkp::net

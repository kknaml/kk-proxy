#pragma once

#include <sys/socket.h>

namespace kkp::net {

     class tcp {

     public:
        tcp() : protocol_family_(AF_INET) {}
        tcp(int protocol_family) : protocol_family_(protocol_family) {}
        int protocol_family() const { return protocol_family_; }

     private:
         int protocol_family_;

     public:
         static tcp v4() { return tcp(AF_INET); }
         static tcp v6() { return tcp(AF_INET6); }
     };

     inline bool operator==(const tcp& lhs, const tcp& rhs) {
         return lhs.protocol_family() == rhs.protocol_family();
     }
}

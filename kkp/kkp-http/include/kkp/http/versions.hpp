#pragma once

#include <string_view>

namespace kkp::http {

    constexpr std::string_view tls_1_0 = "TLSv1.0";
    constexpr std::string_view tls_1_1 = "TLSv1.1";
    constexpr std::string_view tls_1_2 = "TLSv1.2";
    constexpr std::string_view tls_1_3 = "TLSv1.3";


    constexpr std::string_view http_1_0 = "HTTP/1.0";
    constexpr std::string_view http_1_1 = "HTTP/1.1";
    constexpr std::string_view http_2_0 = "HTTP/2.0";
    constexpr std::string_view http_3_0 = "HTTP/3.0";


} // namespace kkp::http

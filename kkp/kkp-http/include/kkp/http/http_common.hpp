#pragma once

#include <string>
#include <map>
#include <kkp/http/http_def.hpp>

namespace kkp::http {

    struct header_value {
        std::string value;
        bool sensitive;
    };

    using header_map = std::multimap<std::string, header_value>;

    struct uri_ref {
        std::string scheme;
        std::string host;
        std::string path;
        std::string raw_path;
        std::string raw_query;
        std::string fragment;
    };





} // namespace kkp::http

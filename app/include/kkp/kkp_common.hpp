#pragma once

#include "kkp_traits.hpp"
#include <string>
#include <map>

namespace kkp {

    using headers = std::multimap<std::string, std::string>;

    struct uri {
        std::string scheme;
        std::string host;
        std::string path;
        std::string raw_path;
        std::string raw_query;
        std::string fragment;
    };
} // namespace kkp

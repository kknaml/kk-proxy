#pragma once

#include <string>
#include <map>
#include <kkp/http/http_def.hpp>
#include <kkp/kkp_traits.hpp>

namespace kkp::http {

    struct header_value {
        std::string value;
        bool sensitive;
    };

    using header_map = std::multimap<std::string, header_value>;

    class headers : public non_copy {

    public:

        headers() : headers_{} {}

        explicit headers(header_map headers) : headers_{std::move(headers)} {}

        [[nodiscard]]
        auto copy() const -> headers {
            return headers{headers_};
        }

        auto add(std::string_view name, std::string_view value, bool sensitive = false) -> void {
            headers_.emplace(std::string{name}, header_value{std::string{value}, sensitive});
        }

        auto set(std::string_view name, std::string_view value, bool sensitive = false) -> void {
            auto range = headers_.equal_range(std::string{name});
            if (range.first == range.second) {
                headers_.emplace(std::string{name}, header_value{std::string{value}, sensitive});
                return;
            }
            for (auto it = range.first; it != range.second; ++it) {
                it->second = header_value{std::string{value}, sensitive};
            }
        }

        [[nodiscard]]
        auto get(std::string_view name) const -> std::string_view {
            auto range = headers_.equal_range(std::string{name});
            if (range.first == range.second) {
                return {};
            }
            return range.first->second.value;
        }


    private:
        header_map headers_;

    public:

    };

    struct uri_ref {
        std::string scheme;
        std::string host;
        std::string path;
        std::string raw_path;
        std::string raw_query;
        std::string fragment;
    };





} // namespace kkp::http

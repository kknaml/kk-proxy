#pragma once

#include <string_view>
#include <array>
#include <format>
#include <kkp/kkp_common.hpp>

namespace kkp::http {


    inline std::string kkp_server_name = std::format("kankan/{}", kkp_version);

    // method
    constexpr inline std::string_view method_get = "GET";
    constexpr inline std::string_view method_post = "POST";
    constexpr inline std::string_view method_put = "PUT";
    constexpr inline std::string_view method_delete = "DELETE";
    constexpr inline std::string_view method_head = "HEAD";
    constexpr inline std::string_view method_options = "OPTIONS";
    constexpr inline std::string_view method_trace = "TRACE";
    constexpr inline std::string_view method_connect = "CONNECT";
    constexpr inline std::string_view method_patch = "PATCH";


    // status
    constexpr inline int status_continue = 100;
    constexpr inline int status_switching_protocols = 101;
    constexpr inline int status_processing = 102;

    constexpr inline int status_ok = 200;
    constexpr inline int status_created = 201;
    constexpr inline int status_accepted = 202;
    constexpr inline int status_non_authoritative_information = 203;
    constexpr inline int status_no_content = 204;
    constexpr inline int status_reset_content = 205;
    constexpr inline int status_partial_content = 206;
    constexpr inline int status_multi_status = 207;
    constexpr inline int status_already_reported = 208;
    constexpr inline int status_im_used = 226;

    constexpr inline int status_multiple_choices = 300;
    constexpr inline int status_moved_permanently = 301;
    constexpr inline int status_found = 302;
    constexpr inline int status_see_other = 303;
    constexpr inline int status_not_modified = 304;
    constexpr inline int status_use_proxy = 305;
    constexpr inline int status_switch_proxy = 306;
    constexpr inline int status_temporary_redirect = 307;
    constexpr inline int status_permanent_redirect = 308;

    constexpr inline int status_bad_request = 400;
    constexpr inline int status_unauthorized = 401;
    constexpr inline int status_payment_required = 402;
    constexpr inline int status_forbidden = 403;
    constexpr inline int status_not_found = 404;
    constexpr inline int status_method_not_allowed = 405;
    constexpr inline int status_not_acceptable = 406;
    constexpr inline int status_proxy_authentication_required = 407;
    constexpr inline int status_request_timeout = 408;
    constexpr inline int status_conflict = 409;
    constexpr inline int status_gone = 410;
    constexpr inline int status_length_required = 411;
    constexpr inline int status_precondition_failed = 412;
    constexpr inline int status_payload_too_large = 413;
    constexpr inline int status_uri_too_long = 414;
    constexpr inline int status_unsupported_media_type = 415;
    constexpr inline int status_range_not_satisfiable = 416;
    constexpr inline int status_expectation_failed = 417;
    constexpr inline int status_im_a_teapot = 418;
    constexpr inline int status_misdirected_request = 421;
    constexpr inline int status_unprocessable_entity = 422;
    constexpr inline int status_locked = 423;
    constexpr inline int status_failed_dependency = 424;
    constexpr inline int status_upgrade_required = 426;
    constexpr inline int status_precondition_required = 428;
    constexpr inline int status_too_many_requests = 429;
    constexpr inline int status_request_header_fields_too_large = 431;
    constexpr inline int status_unavailable_for_legal_reasons = 451;

    constexpr inline int status_internal_server_error = 500;
    constexpr inline int status_not_implemented = 501;
    constexpr inline int status_bad_gateway = 502;
    constexpr inline int status_service_unavailable = 503;
    constexpr inline int status_gateway_timeout = 504;
    constexpr inline int status_http_version_not_supported = 505;
    constexpr inline int status_variant_also_negotiates = 506;
    constexpr inline int status_insufficient_storage = 507;
    constexpr inline int status_loop_detected = 508;
    constexpr inline int status_not_extended = 510;
    constexpr inline int status_network_authentication_required = 511;

    constexpr inline auto status_map = std::array{
        std::pair{status_continue, "Continue"},
        std::pair{status_switching_protocols, "Switching Protocols"},
        std::pair{status_processing, "Processing"},
        std::pair{status_ok, "OK"},
        std::pair{status_created, "Created"},
        std::pair{status_accepted, "Accepted"},
        std::pair{status_non_authoritative_information, "Non-Authoritative Information"},
        std::pair{status_no_content, "No Content"},
        std::pair{status_reset_content, "Reset Content"},
        std::pair{status_partial_content, "Partial Content"},
        std::pair{status_multi_status, "Multi-Status"},
        std::pair{status_already_reported, "Already Reported"},
        std::pair{status_im_used, "IM Used"},
        std::pair{status_multiple_choices, "Multiple Choices"},
        std::pair{status_moved_permanently, "Moved Permanently"},
        std::pair{status_found, "Found"},
        std::pair{status_see_other, "See Other"},
        std::pair{status_not_modified, "Not Modified"},
        std::pair{status_use_proxy, "Use Proxy"},
        std::pair{status_switch_proxy, "Switch Proxy"},
        std::pair{status_temporary_redirect, "Temporary Redirect"},
        std::pair{status_permanent_redirect, "Permanent Redirect"},
        std::pair{status_bad_request, "Bad Request"},
        std::pair{status_unauthorized, "Unauthorized"},
        std::pair{status_payment_required, "Payment Required"},
        std::pair{status_forbidden, "Forbidden"},
        std::pair{status_not_found, "Not Found"},
        std::pair{status_method_not_allowed, "Method Not Allowed"},
        std::pair{status_not_acceptable, "Not Acceptable"},
        std::pair{status_proxy_authentication_required, "Proxy Authentication Required"},
        std::pair{status_request_timeout, "Request Timeout"},
        std::pair{status_conflict, "Conflict"},
        std::pair{status_gone, "Gone"},
        std::pair{status_length_required, "Length Required"},
        std::pair{status_precondition_failed, "Precondition Failed"},
        std::pair{status_payload_too_large, "Payload Too Large"},
        std::pair{status_uri_too_long, "URI TooLong"},
        std::pair{status_unsupported_media_type, "Unsupported Media Type"},
        std::pair{status_range_not_satisfiable, "Range Not Satisfiable"},
        std::pair{status_expectation_failed, "Expectation Failed"},
        std::pair{status_im_a_teapot, "I'm a teapot"},
        std::pair{status_misdirected_request, "Misdirected Request"},
        std::pair{status_unprocessable_entity, "Unprocessable Entity"},
        std::pair{status_locked, "Locked"},
        std::pair{status_failed_dependency, "Failed Dependency"},
        std::pair{status_upgrade_required, "Upgrade Required"},
        std::pair{status_precondition_required, "Precondition Required"},
        std::pair{status_too_many_requests, "Too Many Requests"},
        std::pair{status_request_header_fields_too_large, "Request Header Fields Too Large"},
        std::pair{status_unavailable_for_legal_reasons, "Unavailable For Legal Reasons"},
        std::pair{status_internal_server_error, "Internal Server Error"},
        std::pair{status_not_implemented, "Not Implemented"},
        std::pair{status_bad_gateway, "Bad Gateway"},
        std::pair{status_service_unavailable, "Service Unavailable"},
        std::pair{status_gateway_timeout, "Gateway Timeout"},
        std::pair{status_http_version_not_supported, "HTTP Version Not Supported"},
        std::pair{status_variant_also_negotiates, "Variant Also Negotiates"},
        std::pair{status_insufficient_storage, "Insufficient Storage"},
        std::pair{status_loop_detected, "Loop Detected"},
        std::pair{status_not_extended, "Not Extended"},
        std::pair{status_network_authentication_required, "Network Authentication Required"},
    };

    constexpr inline std::string_view header_unknown = "Unknown Status";

    constexpr inline std::string_view get_status_message(int status) {
        for (const auto &[s, msg] : status_map) {
            if (s == status) {
                return msg;
            }
        }
        return header_unknown;
    }

    // common headers
    constexpr inline std::string_view header_accept = "Accept";
    constexpr inline std::string_view header_accept_charset = "Accept-Charset";
    constexpr inline std::string_view header_accept_encoding = "Accept-Encoding";
    constexpr inline std::string_view header_accept_language = "Accept-Language";
    constexpr inline std::string_view header_accept_ranges = "Accept-Ranges";
    constexpr inline std::string_view header_access_control_allow_credentials = "Access-Control-Allow-Credentials";
    constexpr inline std::string_view header_access_control_allow_headers = "Access-Control-Allow-Headers";
    constexpr inline std::string_view header_access_control_allow_methods = "Access-Control-Allow-Methods";
    constexpr inline std::string_view header_access_control_allow_origin = "Access-Control-Allow-Origin";
    constexpr inline std::string_view header_access_control_expose_headers = "Access-Control-Expose-Headers";
    constexpr inline std::string_view header_access_control_max_age = "Access-Control-Max-Age";
    constexpr inline std::string_view header_access_control_request_headers = "Access-Control-Request-Headers";
    constexpr inline std::string_view header_access_control_request_method = "Access-Control-Request-Method";
    constexpr inline std::string_view header_age = "Age";
    constexpr inline std::string_view header_allow = "Allow";
    constexpr inline std::string_view header_authorization = "Authorization";
    constexpr inline std::string_view header_cache_control = "Cache-Control";
    constexpr inline std::string_view header_connection = "Connection";
    constexpr inline std::string_view header_content_disposition = "Content-Disposition";
    constexpr inline std::string_view header_content_encoding = "Content-Encoding";
    constexpr inline std::string_view header_content_language = "Content-Language";
    constexpr inline std::string_view header_content_length = "Content-Length";
    constexpr inline std::string_view header_content_location = "Content-Location";
    constexpr inline std::string_view header_content_range = "Content-Range";
    constexpr inline std::string_view header_content_type = "Content-Type";
    constexpr inline std::string_view header_cookie = "Cookie";
    constexpr inline std::string_view header_date = "Date";
    constexpr inline std::string_view header_etag = "ETag";
    constexpr inline std::string_view header_expect = "Expect";
    constexpr inline std::string_view header_expires = "Expires";
    constexpr inline std::string_view header_from = "From";
    constexpr inline std::string_view header_host = "Host";
    constexpr inline std::string_view header_if_match = "If-Match";
    constexpr inline std::string_view header_if_modified_since = "If-Modified-Since";
    constexpr inline std::string_view header_if_none_match = "If-None-Match";
    constexpr inline std::string_view header_if_range = "If-Range";
    constexpr inline std::string_view header_if_unmodified_since = "If-Unmodified-Since";
    constexpr inline std::string_view header_last_modified = "Last-Modified";
    constexpr inline std::string_view header_link = "Link";
    constexpr inline std::string_view header_location = "Location";
    constexpr inline std::string_view header_max_forwards = "Max-Forwards";
    constexpr inline std::string_view header_origin = "Origin";
    constexpr inline std::string_view header_pragma = "Pragma";
    constexpr inline std::string_view header_proxy_authenticate = "Proxy-Authenticate";
    constexpr inline std::string_view header_proxy_authorization = "Proxy-Authorization";
    constexpr inline std::string_view header_range = "Range";
    constexpr inline std::string_view header_referer = "Referer";
    constexpr inline std::string_view header_retry_after = "Retry-After";
    constexpr inline std::string_view header_server = "Server";
    constexpr inline std::string_view header_set_cookie = "Set-Cookie";
    constexpr inline std::string_view header_strict_transport_security = "Strict-Transport-Security";
    constexpr inline std::string_view header_transfer_encoding = "Transfer-Encoding";
    constexpr inline std::string_view header_user_agent = "User-Agent";
    constexpr inline std::string_view header_vary = "Vary";
    constexpr inline std::string_view header_via = "Via";
    constexpr inline std::string_view header_warning = "Warning";
    constexpr inline std::string_view header_www_authenticate = "WWW-Authenticate";
    constexpr inline std::string_view header_x_frame_options = "X-Frame-Options";
    constexpr inline std::string_view header_x_xss_protection = "X-XSS-Protection";
    constexpr inline std::string_view header_x_content_type_options = "X-Content-Type-Options";
    constexpr inline std::string_view header_x_forwarded_for = "X-Forwarded-For";
    constexpr inline std::string_view header_x_forwarded_host = "X-Forwarded-Host";
    constexpr inline std::string_view header_x_forwarded_proto = "X-Forwarded-Proto";
    constexpr inline std::string_view header_x_forwarded_server = "X-Forwarded-Server";
    constexpr inline std::string_view header_x_real_ip = "X-Real-IP";
    constexpr inline std::string_view header_x_requested_with = "X-Requested-With";
    constexpr inline std::string_view header_x_ua_compatible = "X-UA-Compatible";
    constexpr inline std::string_view header_x_forwarded = "X-Forwarded";
    constexpr inline std::string_view header_x_forwarded_port = "X-Forwarded-Port";
    constexpr inline std::string_view header_x_forwarded_path = "X-Forwarded-Path";
    constexpr inline std::string_view header_x_csrf_token = "X-CSRF-Token";
    constexpr inline std::string_view header_x_content_security_policy = "X-Content-Security-Policy";
    constexpr inline std::string_view header_x_content_security_policy_report_only = "X-Content-Security-Policy-Report-Only";
    constexpr inline std::string_view header_x_webkit_csp = "X-WebKit-CSP";
    constexpr inline std::string_view header_x_content_duration = "X-Content-Duration";
    constexpr inline std::string_view header_x_powered_by = "X-Powered-By";
    constexpr inline std::string_view header_x_request_id = "X-Request-ID";
    constexpr inline std::string_view header_x_correlation_id = "X-Correlation-ID";
    constexpr inline std::string_view header_x_robots_tag = "X-Robots-Tag";


    // content type
    constexpr inline std::string_view content_type_application_json = "application/json";
    constexpr inline std::string_view content_type_application_xml = "application/xml";
    constexpr inline std::string_view content_type_application_x_www_form_urlencoded = "application/x-www-form-urlencoded";
    constexpr inline std::string_view content_type_application_octet_stream = "application/octet-stream";
    constexpr inline std::string_view content_type_text_plain = "text/plain";
    constexpr inline std::string_view content_type_text_html = "text/html";
    constexpr inline std::string_view content_type_text_xml = "text/xml";

    // content encoding
    constexpr inline std::string_view content_encoding_gzip = "gzip";
    constexpr inline std::string_view content_encoding_deflate = "deflate";
    constexpr inline std::string_view content_encoding_br = "br";


} // namespace kkp::http

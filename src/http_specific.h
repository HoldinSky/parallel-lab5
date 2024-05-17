#ifndef LAB5_HTTP_SPECIFIC_H
#define LAB5_HTTP_SPECIFIC_H

#include <string>
#include <unordered_map>

#define GOOD_START_LINE "HTTP/1.1 200 OK\r\n"
#define BAD_START_LINE "HTTP/1.1 404 NOT FOUND\r\n"

enum Method {
    GET = 1,
    UNKNOWN = 0,
};

struct RequestData {
    std::string request_line;
    std::unordered_map<std::string, std::string> headers{};
    std::string body;

    std::string error;
};

struct RequestLine {
    Method method = UNKNOWN;
    std::string request_target;
    std::string protocol;

    std::string error;
};

struct RequestTarget {
    static constexpr const char* const index = "/";
    static constexpr const char* const page2 = "/page2";
    static constexpr const char* const favicon = "/favicon.ico";
};

ssize_t parse_request(const char* request, RequestData* out_data);

ssize_t parse_request_line(const std::string& input, RequestLine* out_line);

std::string get_response_status_line(bool page_exists);

std::string get_resource(const std::string& filename);

std::string generate_response_headers(const std::string& body, bool is_favicon);

#endif //LAB5_HTTP_SPECIFIC_H

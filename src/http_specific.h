#ifndef LAB5_HTTP_SPECIFIC_H
#define LAB5_HTTP_SPECIFIC_H

#include <string>
#include <unordered_map>

#define CRLF "\r\n"
#define GOOD_START_LINE ("HTTP/1.1 200 OK" + CRLF)
#define BAD_START_LINE ("HTTP/1.1 404 NOT FOUND" + CRLF)

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

ssize_t parse_request(const char* request, RequestData* out_data);

ssize_t parse_request_line(const std::string& input, RequestLine* out_line);

char* find_resource(const std::string& target);

#endif //LAB5_HTTP_SPECIFIC_H

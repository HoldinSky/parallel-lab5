#include "http_specific.h"
#include "common.h"
#include <sstream>

std::string trim(const std::string &s) {
    auto start = s.find_first_not_of(" \t\r\n");
    auto end = s.find_last_not_of(" \t\r\n");
    return start == std::string::npos ? "" : s.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> output;
    std::string word;
    std::istringstream stream(str);

    while (std::getline(stream, word, delimiter)) {
        output.push_back(word);
    }

    return output;
}

ssize_t parse_request(const char *request, RequestData* out_data) {
    std::vector<std::string> headers;
    std::string body;

    std::istringstream stream(request);
    std::string line;

    if (std::getline(stream, line)) {
        out_data->request_line = trim(line);
    }

    if (strncmp(request, Strings::method_get, Strings::method_get_len) != 0) {
        out_data->error = "Failed to recognize request method";
        return -1;
    }

    while (std::getline(stream, line) && !line.empty() && line != "\r") {
        auto header_line = trim(line);

        auto colon_ind = header_line.find_first_of(':');
        if (colon_ind == std::string::npos) continue;

        auto header = trim(header_line.substr(0, colon_ind));
        auto value = trim(header_line.substr(colon_ind + 1));

        out_data->headers[header] = value;
    }

    std::stringstream bodyStream;
    while (std::getline(stream, line)) {
        bodyStream << line << "\n";
    }
    out_data->body = bodyStream.str();

    if (!out_data->body.empty() && out_data->body.back() == '\n') {
        out_data->body.pop_back();
    }

    return 0;
}

ssize_t parse_request_line(const std::string& input, RequestLine* out_line) {
    auto words = split(input, ' ');

    for (const auto& w : words) {
        printf("%s\n", w.c_str());
    }

    if (words.size() != 3) {
        out_line->error = "Bad request line format";
        return -1;
    }

    if (strcmp(words[0].c_str(), Strings::method_get) != 0) {
        out_line->error = "Failed to recognize request method";
        return -1;
    }

    out_line->method = Method::GET;
    out_line->request_target = words[1];
    out_line->protocol = words[2];

    return 0;
}

char* find_resource(const std::string& target) {

    return "";
}
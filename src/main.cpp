#include "common.h"
#include "pool/thread_pool.h"
#include "http_specific.h"

#include <iostream>
#include <poll.h>

void handle_request(int32_t socket_fd) {
    set_timeout(socket_fd, SO_RCVTIMEO, 10);

    char buf[BUFFER_SIZE]{};

    ssize_t bytesRead = recv(socket_fd, buf, BUFFER_SIZE, 0);
    if (bytesRead == 0) {
        print_error("Connection closed by peer\n");
        close(socket_fd);
        return;
    } else if (bytesRead == -1) {
        print_error("Failed to receive data from client or timeout was reached\n");
        close(socket_fd);
        return;
    }

    RequestData data{};
    if (parse_request(buf, &data)) {
        print_error(data.error.c_str());
        close(socket_fd);
        return;
    }

    RequestLine request_line{};
    if (parse_request_line(data.request_line, &request_line)) {
        print_error(request_line.error.c_str());
        close(socket_fd);
        return;
    }

    auto path_start = request_line.request_target.find_first_not_of('/');
    std::string path;
    if (path_start == std::string::npos) {
        path = "/";
    } else {
        path = "/" + request_line.request_target.substr(path_start);
    }

    std::string filename;
    bool target_exists = true;
    bool is_favicon = false;

    if (strcmp(path.c_str(), RequestTarget::index) == 0) {
        filename = "index.html";
    } else if (strcmp(path.c_str(), RequestTarget::page2) == 0) {
        filename = "page2.html";
    } else if (strcmp(path.c_str(), RequestTarget::favicon) == 0) {
        filename = "favicon.png";
        is_favicon = true;
    } else {
        filename = "not_found.html";
        target_exists = false;
    }

    auto status_line = get_response_status_line(target_exists);
    auto response_body = get_resource(filename);
    auto headers = generate_response_headers(response_body, is_favicon);

    std::string response = status_line + headers + "\r\n" + response_body;
    send(socket_fd, response.c_str(), response.length(), 0);

    close(socket_fd);
}

int main_app() {
    int32_t main_socket_d = create_and_open_socket(DEFAULT_PORT);

    bool stop_flag = false;

    std::thread terminal_thread([&]() {
        while (!stop_flag) {
            std::string terminal_input;
            std::getline(std::cin, terminal_input);

            if (strcmp(terminal_input.c_str(), Strings::stop_server) == 0) {
                stop_flag = true;
                printf("Finishing the work");
                return;
            }

            terminal_input.clear();
        }
    });

    struct pollfd fds[1];
    fds[0].fd = main_socket_d;
    fds[0].events = POLLIN;

    ThreadPool pool(5);

    int rcode;
    while (!stop_flag) {
        rcode = poll(fds, 1, 1000); // 1000 means wait 1 second

        if (rcode == -1) {
            std::cerr << "Error in poll\n";
            stop_flag = true;
            break;
        }

        if (fds[0].revents & POLLIN) {
            auto client_sfd = accept_request(main_socket_d);

            ThreadTask task{};
            task.executable = [=]() {
                handle_request(client_sfd);
            };

            pool.add_task(task);
        }
    }

    terminal_thread.join();
    pool.terminate(true);
    close(main_socket_d);

    return 0;
}

//#define TEST
void test() {
//    const char* buf = "GET /path/file.html HTTP/1.1\r\n"
//                 "Host: www.example.com\r\n"
//                 "Content-Type: application/x-www-form-urlencoded\r\n"
//                 "Content-Length: 27\r\n"
//                 "\r\n"
//                 "field1=value1&field2=value2";
//
//    RequestData data{};
//    parse_request(buf, &data);
//
//    printf("Request line: %s\n", data.request_line.c_str());
//    printf("Headers:\n");
//    for (const auto& pair : data.headers) {
//        printf("%s : %s\n", pair.first.c_str(), pair.second.c_str());
//    }
//    printf("\n%s\n\n\n", data.body.c_str());
//
//    RequestLine request_line{};
//    parse_request_line(data.request_line, &request_line);

    std::string body;

    body = get_resource("/");
    std::cout << body << "\n\n";

    body = get_resource("/page2");
    std::cout << body << "\n\n";

    body = get_resource("/apsdf/fd/2302cb4");
    std::cout << body << "\n\n";
}

int main() {
#ifdef TEST
    test();
    return 0;
#else
    return main_app();
#endif
}

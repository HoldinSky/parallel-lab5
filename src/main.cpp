#include "common.h"

#include "pool/thread_pool.h"

#include <iostream>
#include <poll.h>

void print_error(const char *const msg) {
    fprintf(stderr, "%s", msg);
}

void print_error_and_halt(const char *const msg) {
    print_error(msg);
    exit(-1);
}

int32_t create_and_open_socket(uint16_t port) {
    sockaddr_in server{};

    int32_t socket_descriptor;
    int64_t return_code;

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_descriptor < 0) {
        close(socket_descriptor);
        print_error_and_halt("Failed to create socket");
    }

    return_code = bind(socket_descriptor, reinterpret_cast<sockaddr *>(&server), sizeof(server));
    if (return_code == -1) {
        close(socket_descriptor);
        print_error_and_halt("Failed to bind socket");
    }

    listen(socket_descriptor, 5);

    return socket_descriptor;
}

int32_t accept_request(const int32_t &socket_handler) {
    sockaddr_in client{};

    uint32_t client_len = sizeof(sockaddr_in);
    int32_t socket_fd = accept(socket_handler, reinterpret_cast<sockaddr *>(&client), &client_len);

    if (socket_fd == -1) {
        close(socket_handler);
        print_error_and_halt("Failed to accept socket connection\n");
    }

    printf("Successfully accepted connection from %s:%hu\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

    return socket_fd;
}

int main() {
//    int32_t main_socket_d = create_and_open_socket(DEFAULT_PORT);

//    bool stop_flag = false;

//    std::thread terminal_thread([&]() {
//        while (!stop_flag) {
//            std::string terminal_input;
//            std::getline(std::cin, terminal_input);
//
//            if (strcmp(terminal_input.c_str(), Commands::stop_server) == 0) {
//                stop_flag = true;
//                printf("Finishing the work");
//                return;
//            }
//
//            terminal_input.clear();
//        }
//    });
//
//    struct pollfd fds[1];
//    fds[0].fd = main_socket_d;
//    fds[0].events = POLLIN;
//
//    int rcode;
//    while (!stop_flag) {
//        rcode = poll(fds, 1, 1000); // 1000 means wait 1 second
//
//        if (rcode == -1) {
//            std::cerr << "server :: Error in poll\n";
//            break;
//        }
//
//        if (fds[0].revents & POLLIN) {
//            auto connection = accept_request(main_socket_d);
//
//            // todo: handle request in thread pool
//        }
//    }
//
//    terminal_thread.join();
//    close(main_socket_d);

    ThreadPool pool(4);

    for (int i = 0; i < 10; i++) {
        auto task = ThreadTask{};
        task.executable = [=]() {
            std::this_thread::sleep_for(std::chrono::seconds(4));
            printf("Task \"%d\" is finished\n", i + 1);
        };

        pool.add_task(task);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    pool.terminate();

    return 0;
}

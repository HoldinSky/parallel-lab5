#include "common.h"
#include "pool/thread_pool.h"

void test_pool(ThreadPool *pool) {
    for (int i = 0; i < 10; i++) {
        auto task = ThreadTask{};
        task.executable = [=]() {
            std::this_thread::sleep_for(std::chrono::seconds(4));
            printf("Task \"%d\" is finished\n", i + 1);
        };

        pool->add_task(task);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

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

void set_timeout(int32_t s_fd, int32_t timeout_type, uint32_t secs, uint32_t usec) {
    struct timeval tv{secs, usec};

    setsockopt(s_fd, SOL_SOCKET, timeout_type, (const char *) &tv, sizeof tv);
}

void remove_timeout(int32_t s_fd, int32_t timeout_type) {
    struct timeval tv{0, 0};

    setsockopt(s_fd, SOL_SOCKET, timeout_type, (const char *) &tv, sizeof tv);
}
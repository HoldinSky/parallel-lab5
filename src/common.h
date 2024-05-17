#ifndef LAB5_RESOURCES_H
#define LAB5_RESOURCES_H

#include <sys/socket.h>
#include <arpa/inet.h>

#include <csignal>
#include <cstring>
#include <thread>

#include <mutex>
#include <shared_mutex>

#include <functional>
#include <cstdint>
#include <chrono>

#define DEFAULT_PORT 8080
#define BUFFER_SIZE 32768

using rw_lock = std::shared_mutex;
using read_lock = std::shared_lock<rw_lock>;
using write_lock = std::unique_lock<rw_lock>;

// returns exact length of passed string without '\0'
constexpr uint32_t str_length(const char *const str) {
    uint32_t len = 0;

    for (; str[len] != '\0'; len++);

    return len;
}

struct Strings {
    static constexpr const char *const stop_server = "stop";    // stop the server (from terminal)
    static constexpr uint32_t stop_server_len = str_length(stop_server);

    static constexpr const char* const method_get = "GET";
    static constexpr uint32_t method_get_len = str_length(method_get);
};

struct ThreadTask {
    std::function<void()> executable;

    void operator()() const {
        executable();
    }

    ThreadTask() = default;
};

void print_error(const char* msg);

void print_error_and_halt(const char *msg);

int32_t create_and_open_socket(uint16_t port);

int32_t accept_request(const int32_t &socket_handler);

void set_timeout(int32_t s_fd, int32_t timeout_type, uint32_t secs, uint32_t usec = 0);

void remove_timeout(int32_t s_fd, int32_t timeout_type);

#endif //LAB5_RESOURCES_H

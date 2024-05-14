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

static rw_lock stdout_lock;
static rw_lock telemetry_lock;


constexpr uint32_t str_length(const char *const str) {
    uint32_t len = 0;

    for (; str[len] != '\0'; len++);

    return len + 1;
}

struct Commands {
    static constexpr const char *const stop_server = "stop";    // stop the server (from terminal)
    static constexpr uint32_t stop_server_len = str_length(stop_server);
};

struct ThreadTask {
    std::function<void()> executable;
    uint32_t id{};
    std::chrono::high_resolution_clock::time_point creation_point;
    std::chrono::milliseconds wait_time{};
    bool is_moved = false;
    bool is_in_progress = false;

    void operator()() const {
        executable();
    }

    friend bool operator<(const ThreadTask &a, const ThreadTask &b) {
        return a.wait_time < b.wait_time;
    }

    ThreadTask() = default;
};

void print_error(const char* msg);

void print_error_and_halt(const char *msg);

#endif //LAB5_RESOURCES_H

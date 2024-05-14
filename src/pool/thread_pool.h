#ifndef LAB5_THREAD_POOL_H
#define LAB5_THREAD_POOL_H

#include "../common.h"
#include "concurrent_queue.h"

#include <condition_variable>

typedef PriorityQueue<ThreadTask> PoolQueue;

class ThreadPool {
public:
    inline explicit ThreadPool(uint32_t main_threads_num, bool start_immediately = true) {
        this->threads_num = main_threads_num;
        this->workers = std::make_unique<std::thread[]>(this->threads_num);

        this->initialize(start_immediately);
    }

    inline ~ThreadPool() { terminate(); }

public:
    bool working() const;

    bool working_unsafe() const;

    bool alive() const;

    bool alive_unsafe() const;

public:
    void add_task(const ThreadTask &task);

    void start();

    void pause();

    void resume();

    void terminate(bool finish_tasks_in_queue = false);

public:
    ThreadPool(ThreadPool const &other) = delete;

    ThreadPool &operator=(ThreadPool const &rhs) = delete;

private:
    bool initialized = false;
    bool terminated = false;
    bool stopped = false;

    bool is_last_wish = false;

    uint32_t threads_num;

    PoolQueue tasks{};

private:
    std::unique_ptr<std::thread[]> workers;

    mutable rw_lock common_lock;
    mutable rw_lock pause_lock;

    std::condition_variable_any task_waiter{};
    std::condition_variable_any pause_waiter{};
    std::condition_variable_any last_wish_waiter{};

private:

    void initialize(bool start_immediately);

    void thread_routine();

    bool get_task_from_queue(std::shared_ptr<ThreadTask> &out_task);

    void check_pause() {
        write_lock _(this->pause_lock);
        this->pause_waiter.wait(_, [this]() { return !this->stopped; });
    }
};

#endif //LAB5_THREAD_POOL_H

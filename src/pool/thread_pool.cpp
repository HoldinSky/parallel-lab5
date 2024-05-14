#include "thread_pool.h"

bool ThreadPool::working() const {
    read_lock _(this->common_lock);
    return this->working_unsafe();
}

bool ThreadPool::alive() const {
    read_lock _(this->common_lock);
    return this->alive_unsafe();
}

bool ThreadPool::working_unsafe() const {
    return this->alive_unsafe() && !this->stopped;
}

bool ThreadPool::alive_unsafe() const {
    return this->initialized && !this->terminated;
}

void ThreadPool::initialize(bool start_immediately) {
    write_lock _(this->common_lock);
    if (this->initialized || this->terminated)
        return;

    this->stopped = !start_immediately;

    for (size_t i = 0; i < this->threads_num; i++)
        this->workers[i] = std::thread(&ThreadPool::thread_routine, this);


    this->initialized = true;
    this->terminated = false;
    this->is_last_wish = false;
}

void ThreadPool::terminate(bool finish_tasks_in_queue) {
    {
        write_lock _(this->common_lock);
        if (!alive_unsafe())
            return;

        this->is_last_wish = finish_tasks_in_queue;

        if (this->is_last_wish) {
            while (!this->tasks.empty() && !this->tasks.empty()) {
                this->last_wish_waiter.wait(_);
            }
        }

        this->initialized = false;
        this->terminated = true;
        this->task_waiter.notify_all();
    }
    {
        write_lock _p(this->pause_lock);
        this->stopped = false;

        this->pause_waiter.notify_all();
    }

    for (size_t i = 0; i < this->threads_num; i++)
        this->workers[i].join();


    this->stopped = true;
}

bool ThreadPool::get_task_from_queue(std::shared_ptr<ThreadTask> &out_task) {
    write_lock _(this->common_lock);
    bool task_obtained = false;

    auto wait_condition = [&]() {
        do {
            task_obtained = this->tasks.pop(out_task);

            // if this task is in the queue but marked as IN_PROGRESS
        } while (task_obtained && out_task->is_in_progress);

        return this->terminated || task_obtained || this->is_last_wish;
    };

    this->task_waiter.wait(_, wait_condition);

    if (this->terminated || !task_obtained) {
        if (this->is_last_wish) {
            this->last_wish_waiter.notify_one();
        }
        return false;
    }

    if (out_task)
        out_task->is_in_progress = true;

    return out_task != nullptr;
}

void ThreadPool::thread_routine() {
    while (true) {
        std::shared_ptr<ThreadTask> task{};

        this->check_pause();

        if (!this->get_task_from_queue(task))
            return;

        this->check_pause();

        task->operator()();
    }
}

void ThreadPool::add_task(const ThreadTask &task) {
    write_lock _(this->common_lock);

    if (!alive_unsafe()) {
        return;
    }

    this->tasks.push(const_cast<ThreadTask &>(task));

    this->task_waiter.notify_one();
}

void ThreadPool::pause() {
    write_lock _(this->pause_lock);
    this->stopped = true;
}

void ThreadPool::start() {
    if (this->working()) return;

    write_lock _(this->pause_lock);
    this->stopped = false;
    this->pause_waiter.notify_all();
}

void ThreadPool::resume() {
    write_lock _(this->pause_lock);
    this->stopped = false;
    this->pause_waiter.notify_all();
}
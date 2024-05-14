#ifndef LAB2_CONCURRENT_QUEUE_H
#define LAB2_CONCURRENT_QUEUE_H

#include <vector>
#include <memory>
#include <queue>

template <typename T>
class PriorityQueue {
    using queue_implementation = std::queue<std::shared_ptr<T>>;

public:

    PriorityQueue() = default;

    inline ~PriorityQueue() { clear(); }

    bool empty() const;

    size_t size() const;

public:
    void clear();

    bool pop(std::shared_ptr<T> &out_value);

    void push(T &value);
    void push(T &&value);
    void push(std::shared_ptr<T> &value);

public:
    PriorityQueue(PriorityQueue const &other) = delete;

    PriorityQueue &operator=(PriorityQueue const &rhs) = delete;

private:
    mutable rw_lock read_write_lock;

    queue_implementation queue_base{};
};

template <typename T>
bool PriorityQueue<T>::empty() const {
    read_lock _(this->read_write_lock);
    return this->queue_base.empty();
}

template <typename T>
size_t PriorityQueue<T>::size() const {
    read_lock _(this->read_write_lock);
    return this->queue_base.size();
}

template <typename T>
void PriorityQueue<T>::clear() {
    write_lock _(this->read_write_lock);
    while (!this->queue_base.empty()) {
        this->queue_base.pop();
    }
}

template <typename T>
bool PriorityQueue<T>::pop(std::shared_ptr<T> &out_value) {
    write_lock _(this->read_write_lock);

    if (this->queue_base.empty()) return false;

    out_value = std::move(this->queue_base.front());
    this->queue_base.pop();

    return true;
}

template <typename T>
void PriorityQueue<T>::push(T &value) {
    write_lock _(this->read_write_lock);

    this->queue_base.push(std::make_shared<T>(value));
}

template <typename T>
void PriorityQueue<T>::push(T &&value) {
    write_lock _(this->read_write_lock);

    this->queue_base.push(std::make_shared<T>(value));
}

template <typename T>
void PriorityQueue<T>::push(std::shared_ptr<T> &value) {
    write_lock _(this->read_write_lock);

    this->queue_base.push(value);
}

#endif //LAB2_CONCURRENT_QUEUE_H

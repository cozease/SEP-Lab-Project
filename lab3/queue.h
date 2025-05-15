#ifndef QUEUE_H
#define QUEUE_H

// Read-only definition
// NOTE: DON'T modify this file

#include "node.h"
#include <cstddef>

template <typename T>
class Queue {
    int sz;
    UniquePtr<Node<T>> head;

    // Question: why we use raw pointer for tail?
    Node<T> *tail;

public:
    Queue() : sz(0), head(nullptr), tail(nullptr) { }

    void push(T t);
    void pop();
    T &front();
    [[nodiscard]] bool empty() const;
    [[nodiscard]] size_t size() const;
};

#include "queue_impl.h"
#endif

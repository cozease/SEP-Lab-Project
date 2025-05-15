#ifndef STACK_H
#define STACK_H

// Read-only definition
// NOTE: DON'T modify this file

#include "node.h"

template <typename T>
class Stack {
    size_t sz;
    UniquePtr<Node<T>> head;

public:
    Stack() : sz(0), head(nullptr) { }

    void push(T t);
    void pop();
    T &top();
    [[nodiscard]] bool empty() const;
    [[nodiscard]] size_t size() const;
};

#include "stack_impl.h"
#endif

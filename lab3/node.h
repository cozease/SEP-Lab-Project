#ifndef NODE_H_
#define NODE_H_

#include "unique_ptr.h"

template <typename T>
struct Node {
    T val;
    UniquePtr<Node> next;

    Node() : next(nullptr) { }

    explicit Node(T t) : val(t), next(nullptr) { }
};

#endif
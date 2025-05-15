#ifndef STACK_IMPL_H
#define STACK_IMPL_H
#include <cassert>

template <typename T>
void Stack<T>::push(T t) {
    // TODO
    UniquePtr<Node<T>> newNode = MakeUnique<Node<T>>(t);
    if (head) newNode->next = std::move(head);
    head = std::move(newNode);
    sz++;
}

template <typename T>
void Stack<T>::pop() {
    // TODO
    head = std::move(head->next);
    sz--;
}

template <typename T>
T &Stack<T>::top() {
    // TODO
    return head->val;
}

template <typename T>
bool Stack<T>::empty() const {
    // TODO
    return sz == 0;
}

template <typename T>
size_t Stack<T>::size() const {
    // TODO
    return sz;
}

#endif  // STACK_IMPL_H

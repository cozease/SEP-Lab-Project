#ifndef QUEUE_IMPL_H
#define QUEUE_IMPL_H

template <typename T>
void Queue<T>::push(T t) {
    // TODO
    if (sz == 0) {
        head = MakeUnique<Node<T>>(t);
        tail = head.get();
    }
    else{
        tail -> next = MakeUnique<Node<T>>(t);
        tail = tail->next.get();
    }
    sz++;
}

template <typename T>
void Queue<T>::pop() {
    // TODO
    if (sz == 1) {
        head = nullptr;
        tail = nullptr;
    } else {
        head = std::move(head->next);
    }
    sz--;
}

template <typename T>
T &Queue<T>::front() {
    // TODO
    return head->val;
}

template <typename T>
bool Queue<T>::empty() const {
    // TODO
    return sz == 0;
}

template <typename T>
size_t Queue<T>::size() const {
    // TODO
    return sz;
}
#endif  // QUEUE_IMPL_H

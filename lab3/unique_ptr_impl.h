#ifndef UNIQUE_PTR_IMPL_H
#define UNIQUE_PTR_IMPL_H

// You need to implement follow functions, signatures are provided.
// NOTE: DON'T change the function definition

template <typename T>
UniquePtr<T>::UniquePtr(UniquePtr &&other) noexcept : pointer (other.release()) { /* TODO */ }

template <typename T>
UniquePtr<T>::~UniquePtr() {
    // TODO
    delete pointer;
}

template <typename T>
UniquePtr<T> &UniquePtr<T>::operator=(UniquePtr &&other) noexcept {
    // TODO
    if (this != &other) {
        reset(other.release());
    }
    return *this;
}

template <typename T>
UniquePtr<T> &UniquePtr<T>::operator=(std::nullptr_t) noexcept {
    // TODO
    reset(nullptr);
    return *this;
}

template <typename T>
void UniquePtr<T>::reset(T *ptr) noexcept {
    // TODO
    if (pointer != ptr) {
        delete pointer;
        pointer = ptr;
    }
}

template <typename T>
T *UniquePtr<T>::release() noexcept {
    // TODO
    T *tmp = pointer;
    pointer = nullptr;
    return tmp;
}

#endif  // UNIQUE_PTR_IMPL_H

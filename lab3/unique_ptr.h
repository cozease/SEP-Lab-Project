#ifndef UNIQUE_PTR_H
#define UNIQUE_PTR_H

// Read-only definition
// NOTE: DON'T modify this file

#include <utility>

template <typename T>
class UniquePtr {
public:
    UniquePtr() noexcept : pointer(nullptr) { }
    explicit UniquePtr(T *raw) noexcept : pointer(raw) { }

    // move constructor: Build a new UniquePtr and move other into this
    UniquePtr(UniquePtr &&other) noexcept;

    ~UniquePtr();

    // move assign: move other UniquePtr into this
    UniquePtr &operator=(UniquePtr &&other) noexcept;
    // move assign: move nullptr into this
    UniquePtr &operator=(std::nullptr_t) noexcept;

    T *get() const {
        return pointer;
    }

    T &operator*() const {
        return *pointer;
    }

    explicit operator bool() const noexcept {
        return static_cast<bool>(pointer);
    }

    T *operator->() const noexcept {
        return pointer;
    }

    friend bool operator==(const UniquePtr &lhs, std::nullptr_t) {
        return !lhs.pointer;
    }

    friend bool operator==(std::nullptr_t, const UniquePtr &rhs) {
        return !rhs.pointer;
    }

    friend bool operator!=(const UniquePtr &lhs, std::nullptr_t rhs) {
        return !(lhs == rhs);
    }

    friend bool operator!=(std::nullptr_t lhs, const UniquePtr &rhs) {
        return !(lhs == rhs);
    }

    // Make this give up the memory it holds, and hold a new memory pointed by `ptr`.
    // Default value of `ptr` is nullptr, which means `reset()` will give up the memory
    // and hold nothing
    //
    // Hint: `reset()` will make memory no owner
    void reset(T *ptr = nullptr) noexcept;

    // Give up the memory it holds, and hold nothing.
    // You need to return the pointer holds to the memory.
    //
    // Hint: `release()` will just release the ownership of memory, which means
    // someone else will hold the memory in the future.
    T *release() noexcept;

private:
    T *pointer;
};

template <typename T, typename... Types>
UniquePtr<T> MakeUnique(Types &&...args) {
    return UniquePtr<T> { new T(std::forward<Types>(args)...) };
}

#include "unique_ptr_impl.h"
#endif  // UNIQUE_PTR_H

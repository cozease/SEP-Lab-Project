#include "rod.h"
#include <cstddef>

Rod::Rod(const int capacity, const int id) : /* TODO */
    capacity(capacity),
    id(id) { }

Rod::~Rod() { }

bool Rod::push(const Disk d) {
    // TODO
    if (stack.empty() || d.id < stack.top().id) {
        stack.push(d);
        return true;
    }
    return false;
}

const Disk &Rod::top() {
    // TODO
    return stack.top();
}

void Rod::pop() {
    // TODO
    stack.pop();
}

size_t Rod::size() const {
    // TODO
    return stack.size();
}

bool Rod::empty() const {
    // TODO
    return stack.empty();
}
bool Rod::full() const {
    // TODO
    return stack.size() == capacity;
}
void Rod::draw(Canvas &canvas) {
    // TODO
    Stack<const Disk> tmp;
    while (!stack.empty()) {
        stack.top().draw(canvas, stack.size() - 1, id - 1);
        tmp.push(stack.top());
        stack.pop();
    }
    while (!tmp.empty()) {
        stack.push(tmp.top());
        tmp.pop();
    }
}

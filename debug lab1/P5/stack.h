#ifndef SEP_DEBUG1_P5_STACK_H
#define SEP_DEBUG1_P5_STACK_H

class Stack {
    int *ptr;
    int size;
    int index;

public:
    Stack(int size = 10);
    Stack(const Stack &s);
    Stack(Stack &&s);
    ~Stack();
    Stack &operator=(const Stack &s);
    Stack &operator=(Stack &&s);
    void push(int val);
    int pop();
    int top();
    bool empty();
};

#endif //SEP_DEBUG1_P5_STACK_H

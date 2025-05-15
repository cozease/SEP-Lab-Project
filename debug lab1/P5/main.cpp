#include <iostream>

#include "stack.h"

int main() {
    int n;
    int stack_index, num;
    Stack stack_arr[3];

    for (int i = 0; i < 3; i++) {
        stack_arr[i] = Stack(20);
    }

    std::cin >> n;

    for (int i = 0; i < n; i++) {
        std::cin >> stack_index >> num;

        if (stack_index < 0 || stack_index >= 3) {
            std::cout << "invalid stack index" << std::endl;
            continue;
        }

        stack_arr[stack_index].push(num);
    }

    for (int i = 0; i < 3; i++) {
        std::cout << "stack " << i << ": ";
        while (!stack_arr[i].empty()) {
            std::cout << stack_arr[i].pop() << " ";
        }
        std::cout << std::endl;
    }
}

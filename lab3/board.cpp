#include <iostream>

#include "board.h"
#include "queue.h"

Board::Board(const int num_disk) : /* TODO */
    num_disk(num_disk),
    rods{
        Rod(num_disk, 1),
        Rod(num_disk, 2),
        Rod(num_disk, 3)
    }{
    // TODO
    for (int i = num_disk; i > 0; i --) {
        rods[0].push(Disk(i, 2 * i + 1));
    }
}

Board::~Board() {
    // TODO
}

void Board::draw() {
    Canvas canvas {};
    canvas.reset();
    // TODO
    for (int i = 0; i < 41; i ++) {
        canvas.buffer[10][i] = '-';
    }
    for (int i = 0; i < 11; i ++) {
        canvas.buffer[i][5] = '|';
        canvas.buffer[i][20] = '|';
        canvas.buffer[i][35] = '|';
    }
    for (auto & rod : rods) rod.draw(canvas);
    canvas.draw();
}

void Board::move(int from, int to, const bool log) {
    // TODO
    if(from < 1 || from > 3 || to < 1 || to > 3) return;
    if(rods[from - 1].empty()) return;
    if(!rods[to - 1].empty() && rods[from - 1].top().id > rods[to - 1].top().id) return;

    if (log) {
        history.push(std::pair<int, int> (from, to));
    } else {
        std::cout << "Auto moving:" << from << "->" << to << std::endl;
    }
    rods[to - 1].push(rods[from - 1].top());
    rods[from - 1].pop();
}

bool Board::win() const {
    // TODO
    return rods[1].full();
}

void solve(
    const int n,
    const int src,
    const int buf,
    const int dest,
    Queue<std::pair<int, int>> &solution
) {
    // TODO
    if (n == 1) {
        solution.push(std::pair<int, int> (src, dest));
        return;
    }
    solve(n-1, src, dest, buf, solution);
    solution.push(std::pair<int, int> (src, dest));
    solve(n-1, buf, src, dest, solution);
}

void Board::autoplay() {
    // TODO
    while (!history.empty()) {
        move(history.top().second, history.top().first, false);
        history.pop();
        draw();
    }
    Queue<std::pair<int, int>> solution;
    solve(num_disk, 1, 3, 2, solution);
    while (!solution.empty()) {
        move(solution.front().first, solution.front().second, false);
        solution.pop();
        draw();
    }
    std::cout<<"Congratulations! You win!"<<std::endl;
}

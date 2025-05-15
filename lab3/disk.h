#ifndef DISK_H
#define DISK_H

#include "canvas.h"

class Disk {
public:
    int id;
    int val;

    Disk() : id(-1), val(-1) {}
    Disk(const int id, const int val) : id(id), val(val) { }

    void draw(Canvas &canvas, const int level, const int rod_loc) const {
        const int s_x = 5 + (rod_loc * 15) - (val / 2);
        const int y = Canvas::HEIGHT - (level + 1) * 2;
        for (int i = 0; i < val; i++) {
            canvas.buffer[y][s_x + i] = '*';
        }
    }
};

#endif

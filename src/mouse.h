#ifndef MOUSE_H
#define MOUSE_H

#include "ceslete.h"

struct mouse_state {
    i32 x;
    i32 y;
    u8 buttons;
    i8 dx;
    i8 dy;
};

int mouse_init(void);
void mouse_poll(struct mouse_state *state);

#endif

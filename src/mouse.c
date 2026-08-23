#include "mouse.h"

static struct mouse_state cached;
static int initialized;

static u8 port_read(u16 port) {
    u8 val;
    __asm__ volatile("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static void port_write(u16 port, u8 val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static void wait_input(void) {
    while (!(port_read(0x64) & 0x01)) {}
}

static void wait_output(void) {
    while (port_read(0x64) & 0x02) {}
}

static void ps2_send(u8 cmd) {
    wait_output();
    port_write(0x64, 0xD4);
    wait_output();
    port_write(0x60, cmd);
}

int mouse_init(void) {
    wait_output();
    port_write(0x64, 0xA8);
    ps2_send(0xF6);
    wait_input();
    port_read(0x60);
    ps2_send(0xF4);
    wait_input();
    u8 ack = port_read(0x60);
    if (ack != 0xFA) return -1;
    cached.x = 0;
    cached.y = 0;
    cached.buttons = 0;
    cached.dx = 0;
    cached.dy = 0;
    initialized = 1;
    return 0;
}

void mouse_poll(struct mouse_state *state) {
    if (!initialized) return;
    u8 status = port_read(0x64);
    if (!(status & 0x01)) {
        *state = cached;
        return;
    }
    u8 flags = port_read(0x60);
    if (!(flags & 0x08)) {
        *state = cached;
        return;
    }
    wait_input();
    i8 dx = (i8)port_read(0x60);
    wait_input();
    i8 dy = (i8)port_read(0x60);
    cached.dx = dx;
    cached.dy = dy;
    cached.buttons = flags & 0x07;
    cached.x += dx;
    cached.y -= dy;
    if (cached.x < 0) cached.x = 0;
    if (cached.y < 0) cached.y = 0;
    *state = cached;
}

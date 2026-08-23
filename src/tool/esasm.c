#include "ceslete.h"
#include "eru/eru.h"

static struct eru_ctx display;
static u32 frame_count;

static int validate_handoff(const struct ToastHandoff *h) {
    if (!h) return -1;
    if (h->toast_version != TOAST_HANDOFF_VERSION) return -2;
    if (!(h->flags & 0x1)) return -3;
    if (h->fb.base_address == 0) return -4;
    return 0;
}

static void delay(volatile u64 cycles) {
    while (cycles--) {}
}

void dotshirp_main(struct ToastHandoff *handoff) {
    int status = validate_handoff(handoff);
    if (status != 0) {
        while (1) __asm__ volatile("hlt");
    }

    if (eru_init(&display, &handoff->fb) != 0) {
        while (1) __asm__ volatile("hlt");
    }

    frame_count = 0;

    while (1) {
        eru_clear(&display, 0x001A1A1A);
        eru_3d_cube(&display, frame_count & 0xFF, 0x00FF69B4, 256);
        eru_str(&display, 10, 10, "Ceslete OS", 0x00FFFFFF, 0x001A1A1A);
        frame_count++;
        delay(5000000);
    }
}

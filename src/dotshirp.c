#include "ceslete.h"

static int validate_handoff(const struct ToastHandoff *h) {
    if (!h) return -1;
    if (h->toast_version != TOAST_HANDOFF_VERSION) return -2;
    if (!(h->flags & 0x1)) return -3;
    if (h->fb.base_address == 0) return -4;
    return 0;
}

void dotshirp_main(struct ToastHandoff *handoff) {
    int status = validate_handoff(handoff);
    if (status != 0) {
        while(1) __asm__ volatile("hlt");
    }
    
    while(1) {
        __asm__ volatile("hlt");
    }
}

#ifndef ERU_H
#define ERU_H

#include "../ceslete.h"

struct eru_ctx {
    u64 base;
    u32 w;
    u32 h;
    u32 pitch;
};

int eru_init(struct eru_ctx *ctx, const struct FramebufferInfo *fb);
void eru_clear(struct eru_ctx *ctx, u32 color);
void eru_pixel(struct eru_ctx *ctx, u32 x, u32 y, u32 color);
void eru_str(struct eru_ctx *ctx, u32 x, u32 y, const char *s, u32 fg, u32 bg);

#endif

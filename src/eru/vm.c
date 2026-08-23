#include "eru.h"

extern const unsigned char render_program[];
extern const unsigned int render_program_len;

static unsigned char tape[65536];

static void bf_malbolge_exec(const unsigned char *prog, unsigned int len,
                             u64 fb_base, u32 w, u32 h, u32 pitch,
                             u32 arg0, u32 arg1, u32 arg2) {
    unsigned int ip = 0;
    unsigned int dp = 0;
    u32 px = arg0;
    u32 py = arg1;
    u32 color = arg2;

    while (ip < len) {
        unsigned char c = prog[ip];
        switch (c) {
            case '>': dp++; break;
            case '<': dp--; break;
            case '+': tape[dp]++; break;
            case '-': tape[dp]--; break;
            case '.':
                if (px < w && py < h) {
                    u32 *ptr = (u32 *)(fb_base + (py * pitch) + (px * 4));
                    *ptr = color;
                }
                px++;
                if (px >= w) { px = 0; py++; }
                break;
            case ',': tape[dp] = (unsigned char)(color & 0xFF); break;
            case '[':
                if (tape[dp] == 0) {
                    int depth = 1;
                    while (depth > 0 && ip < len) {
                        ip++;
                        if (prog[ip] == '[') depth++;
                        if (prog[ip] == ']') depth--;
                    }
                }
                break;
            case ']':
                if (tape[dp] != 0) {
                    int depth = 1;
                    while (depth > 0 && ip > 0) {
                        ip--;
                        if (prog[ip] == ']') depth++;
                        if (prog[ip] == '[') depth--;
                    }
                }
                break;
            default:
                tape[dp] = (tape[dp] / 94 + tape[dp] % 33) & 0xFF;
                break;
        }
        ip++;
    }
}

int eru_init(struct eru_ctx *ctx, const struct FramebufferInfo *fb) {
    if (!ctx || !fb || fb->base_address == 0) return -1;
    ctx->base = fb->base_address;
    ctx->w = fb->width;
    ctx->h = fb->height;
    ctx->pitch = fb->pitch;
    return 0;
}

void eru_clear(struct eru_ctx *ctx, u32 color) {
    for (u32 y = 0; y < ctx->h; y++) {
        for (u32 x = 0; x < ctx->w; x++) {
            bf_malbolge_exec(render_program, render_program_len,
                             ctx->base, ctx->w, ctx->h, ctx->pitch,
                             x, y, color);
        }
    }
}

void eru_pixel(struct eru_ctx *ctx, u32 x, u32 y, u32 color) {
    bf_malbolge_exec(render_program, render_program_len,
                     ctx->base, ctx->w, ctx->h, ctx->pitch,
                     x, y, color);
}

void eru_str(struct eru_ctx *ctx, u32 x, u32 y, const char *s, u32 fg, u32 bg) {
    while (*s) {
        for (u32 row = 0; row < 8; row++) {
            for (u32 col = 0; col < 8; col++) {
                u32 c = ((*s >> col) & 1) ? fg : bg;
                bf_malbolge_exec(render_program, render_program_len,
                                 ctx->base, ctx->w, ctx->h, ctx->pitch,
                                 x + col, y + row, c);
            }
        }
        x += 8;
        s++;
    }
}

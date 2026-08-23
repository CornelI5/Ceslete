#include "eru.h"

extern const unsigned char render_program[];
extern const unsigned int render_program_len;

static unsigned char tape[65536];

enum eru_cmd {
    ERU_CMD_CLEAR   = 0,
    ERU_CMD_PIXEL   = 1,
    ERU_CMD_CHAR    = 2,
    ERU_CMD_CUBE    = 3
};

static void eru_exec(u8 cmd, struct eru_ctx *ctx, u32 a0, u32 a1, u32 a2) {
    tape[0] = cmd;
    tape[1] = (u8)(a0 & 0xFF);
    tape[2] = (u8)((a0 >> 8) & 0xFF);
    tape[3] = (u8)(a1 & 0xFF);
    tape[4] = (u8)((a1 >> 8) & 0xFF);
    tape[5] = (u8)(a2 & 0xFF);
    tape[6] = (u8)((a2 >> 8) & 0xFF);
    tape[7] = (u8)(ctx->w & 0xFF);
    tape[8] = (u8)((ctx->w >> 8) & 0xFF);
    tape[9] = (u8)(ctx->h & 0xFF);
    tape[10] = (u8)((ctx->h >> 8) & 0xFF);
    tape[11] = (u8)((a0 >> 16) & 0xFF);
    tape[12] = (u8)((a1 >> 16) & 0xFF);

    unsigned int ip = 0;
    unsigned int dp = 13;

    while (ip < render_program_len) {
        unsigned char c = render_program[ip];
        switch (c) {
            case '>': dp++; break;
            case '<': dp--; break;
            case '+': tape[dp]++; break;
            case '-': tape[dp]--; break;
            case '.':
                {
                    u32 px = tape[dp] | (tape[dp+1] << 8);
                    u32 py = tape[dp+2] | (tape[dp+3] << 8);
                    u32 col = tape[dp+4] | (tape[dp+5] << 16) | (tape[dp+6] << 24);
                    if (px < ctx->w && py < ctx->h) {
                        u32 *ptr = (u32 *)(ctx->base + (py * ctx->pitch) + (px * 4));
                        *ptr = col;
                    }
                }
                break;
            case ',': tape[dp] = 0; break;
            case '[':
                if (tape[dp] == 0) {
                    int d = 1;
                    while (d > 0 && ip < render_program_len) {
                        ip++;
                        if (render_program[ip] == '[') d++;
                        if (render_program[ip] == ']') d--;
                    }
                }
                break;
            case ']':
                if (tape[dp] != 0) {
                    int d = 1;
                    while (d > 0 && ip > 0) {
                        ip--;
                        if (render_program[ip] == ']') d++;
                        if (render_program[ip] == '[') d--;
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
    eru_exec(ERU_CMD_CLEAR, ctx, 0, 0, color);
}

void eru_pixel(struct eru_ctx *ctx, u32 x, u32 y, u32 color) {
    eru_exec(ERU_CMD_PIXEL, ctx, x, y, color);
}

void eru_str(struct eru_ctx *ctx, u32 x, u32 y, const char *s, u32 fg, u32 bg) {
    while (*s) {
        eru_exec(ERU_CMD_CHAR, ctx, x, y, (u32)(*s));
        x += 8;
        s++;
    }
}

void eru_3d_cube(struct eru_ctx *ctx, u32 angle, u32 color, u32 scale) {
    eru_exec(ERU_CMD_CUBE, ctx, angle, color, scale);
}

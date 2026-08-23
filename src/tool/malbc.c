#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_OUT 65536
#define MAX_LINE 2048

static uint8_t out[MAX_OUT];
static int out_pos;
static int dp;

static const char malb_table[] =
    "JIHGFEDCBAzyxwvutsrqponmlkjihgfedcba_^]\\[ZYXWVUTSRQPONMLKJ"
    "IHGFEDCBA@?>=<;:9876543210/.-,+*)('&%$#\"!~}|{zyxwvutsrqponm"
    "lkjihgfedcba`_^]\\[ZYXWVUTSRQPONMLKJIHGFEDCBA@?>=<;:9876543"
    "210/.-,+*)('&%$#\"!~}|{zyxwvutsrqponmlkjihgfedcba`_^]\\[ZYXW";

static void emit(uint8_t b) {
    if (out_pos >= MAX_OUT) { fprintf(stderr, "overflow\n"); exit(1); }
    out[out_pos++] = b;
}

static uint8_t malb_op(int target_val) {
    int pos = out_pos % 94;
    for (int c = 33; c <= 126; c++) {
        int idx = (c + pos) % 94;
        if ((malb_table[idx] - 33) == target_val) return (uint8_t)c;
    }
    return '?';
}

static void compile_move(int delta) {
    while (delta > 0) { emit(malb_op(6)); delta--; dp++; }
    while (delta < 0) { emit(malb_op(7)); delta++; dp--; }
}

static void compile_set(uint8_t val) {
    emit(malb_op(4));
    int cur = 0;
    int diff = (val - cur + 94) % 94;
    for (int i = 0; i < diff; i++) emit(malb_op(2));
}

static void compile_add(uint8_t val) {
    for (int i = 0; i < val; i++) emit(malb_op(2));
}

static void compile_out(void) { emit(malb_op(5)); }

static void compile_in(void) { emit(malb_op(3)); }

static void compile_nop(void) { emit(malb_op(0)); }

static void process_line(char *line) {
    char *p = line;
    while (*p == ' ' || *p == '\t') p++;
    if (*p == '#' || *p == '\n' || *p == '\0') return;

    if (strncmp(p, "SET", 3) == 0) {
        int val = atoi(p + 3);
        compile_set((uint8_t)(val & 0xFF));
    } else if (strncmp(p, "ADD", 3) == 0) {
        int val = atoi(p + 3);
        compile_add((uint8_t)(val & 0xFF));
    } else if (strncmp(p, "MOVE", 4) == 0) {
        int val = atoi(p + 4);
        compile_move(val);
    } else if (strncmp(p, "OUT", 3) == 0) {
        compile_out();
    } else if (strncmp(p, "IN", 2) == 0) {
        compile_in();
    } else if (strncmp(p, "NOP", 3) == 0) {
        compile_nop();
    } else if (strncmp(p, "RAW", 3) == 0) {
        int val = atoi(p + 3);
        emit((uint8_t)(val & 0xFF));
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: malbc <input.mbc> <output.bin>\n");
        return 1;
    }
    FILE *fin = fopen(argv[1], "r");
    if (!fin) { fprintf(stderr, "cannot open %s\n", argv[1]); return 1; }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fin)) process_line(line);
    fclose(fin);

    FILE *fout = fopen(argv[2], "wb");
    if (!fout) { fprintf(stderr, "cannot open %s\n", argv[2]); return 1; }
    fwrite(out, 1, out_pos, fout);
    fclose(fout);

    printf("malbc: %d bytes, dp=%d\n", out_pos, dp);
    return 0;
}

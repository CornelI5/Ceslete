#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_PROGRAM 65536
#define MAX_LINE 1024

static uint8_t output[MAX_PROGRAM];
static int out_pos;

static void emit(uint8_t byte) {
    if (out_pos >= MAX_PROGRAM) {
        fprintf(stderr, "error: program too large\n");
        exit(1);
    }
    output[out_pos++] = byte;
}

static void emit_data_byte(const char *arg) {
    unsigned long val = strtoul(arg, NULL, 0);
    emit((uint8_t)(val & 0xFF));
}

static void emit_string(const char *s) {
    s++;
    while (*s && *s != '"') {
        if (*s == '\\' && *(s+1)) {
            s++;
            switch (*s) {
                case 'n': emit('\n'); break;
                case '0': emit(0); break;
                case '\\': emit('\\'); break;
                case '"': emit('"'); break;
                default: emit(*s); break;
            }
        } else {
            emit((uint8_t)*s);
        }
        s++;
    }
}

static void process_line(char *line) {
    char *p = line;
    while (*p == ' ' || *p == '\t') p++;
    if (*p == '#' || *p == '\n' || *p == '\0') return;

    if (*p == '.') {
        p++;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '"') {
            emit_string(p);
        } else {
            while (*p && *p != ' ' && *p != '\t' && *p != '\n') {
                char buf[16];
                int i = 0;
                while (*p && *p != ',' && *p != ' ' && *p != '\t' && *p != '\n' && i < 15) {
                    buf[i++] = *p++;
                }
                buf[i] = '\0';
                emit_data_byte(buf);
                if (*p == ',') p++;
                while (*p == ' ' || *p == '\t') p++;
            }
        }
        return;
    }

    while (*p && *p != '\n') {
        switch (*p) {
            case '>': case '<': case '+': case '-':
            case '.': case ',': case '[': case ']':
                emit((uint8_t)*p);
                break;
            default:
                emit((uint8_t)*p);
                break;
        }
        p++;
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: esasm <input.es.txt> <output.bin>\n");
        return 1;
    }

    FILE *fin = fopen(argv[1], "r");
    if (!fin) {
        fprintf(stderr, "error: cannot open %s\n", argv[1]);
        return 1;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fin)) {
        process_line(line);
    }
    fclose(fin);

    FILE *fout = fopen(argv[2], "wb");
    if (!fout) {
        fprintf(stderr, "error: cannot open %s\n", argv[2]);
        return 1;
    }
    fwrite(output, 1, out_pos, fout);
    fclose(fout);

    printf("esasm: %d bytes written to %s\n", out_pos, argv[2]);
    return 0;
}

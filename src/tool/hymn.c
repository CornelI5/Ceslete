#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define SAMPLE_RATE 22050
#define MAX_SAMPLES (SAMPLE_RATE * 60)

static int16_t buffer[MAX_SAMPLES];
static int sample_pos;

static void write_wav_header(FILE *f, int num_samples) {
    uint32_t data_size = num_samples * sizeof(int16_t);
    uint32_t file_size = 36 + data_size;
    uint8_t header[44] = {
        'R','I','F','F',
        file_size&0xFF,(file_size>>8)&0xFF,(file_size>>16)&0xFF,(file_size>>24)&0xFF,
        'W','A','V','E','f','m','t',' ',
        16,0,0,0,
        1,0,
        1,0,
        SAMPLE_RATE&0xFF,(SAMPLE_RATE>>8)&0xFF,0,0,
        (SAMPLE_RATE*2)&0xFF,((SAMPLE_RATE*2)>>8)&0xFF,0,0,
        2,0,16,0,
        'd','a','t','a',
        data_size&0xFF,(data_size>>8)&0xFF,(data_size>>16)&0xFF,(data_size>>24)&0xFF
    };
    fwrite(header, 1, 44, f);
}

static int16_t generate_sample(uint8_t waveform, double phase) {
    switch (waveform) {
        case 0: return (int16_t)(sin(phase * 2.0 * M_PI) * 16000);
        case 1: return (phase < 0.5) ? 16000 : -16000;
        case 2: return (int16_t)((phase * 2.0 - 1.0) * 16000);
        case 3: return (int16_t)((rand() % 32000) - 16000);
        default: return 0;
    }
}

static void emit_note(uint8_t pitch, uint8_t duration, uint8_t velocity, uint8_t waveform) {
    double freq = 440.0 * pow(2.0, (pitch - 69) / 12.0);
    int samples = (duration * SAMPLE_RATE) / 8;
    double phase = 0.0;
    double amp = velocity / 127.0;

    for (int i = 0; i < samples && sample_pos < MAX_SAMPLES; i++) {
        int16_t s = generate_sample(waveform, phase);
        buffer[sample_pos++] = (int16_t)(s * amp);
        phase += freq / SAMPLE_RATE;
        if (phase >= 1.0) phase -= 1.0;
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: hymn <malbolge_output.bin> <output.wav>\n");
        return 1;
    }

    FILE *fin = fopen(argv[1], "rb");
    if (!fin) { fprintf(stderr, "cannot open %s\n", argv[1]); return 1; }

    uint8_t bytes[4];
    while (fread(bytes, 1, 4, fin) == 4 && sample_pos < MAX_SAMPLES) {
        uint8_t pitch = (bytes[0] % 24) + 48;
        uint8_t duration = (bytes[1] % 8) + 1;
        uint8_t velocity = (bytes[2] % 64) + 32;
        uint8_t waveform = bytes[3] % 4;
        emit_note(pitch, duration, velocity, waveform);
    }
    fclose(fin);

    FILE *fout = fopen(argv[2], "wb");
    if (!fout) { fprintf(stderr, "cannot open %s\n", argv[2]); return 1; }
    write_wav_header(fout, sample_pos);
    fwrite(buffer, sizeof(int16_t), sample_pos, fout);
    fclose(fout);

    printf("hymn: %d samples, %.2f sec, 16-bit mono 22050Hz\n",
           sample_pos, (double)sample_pos / SAMPLE_RATE);
    return 0;
}

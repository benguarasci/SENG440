#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include <time.h>



FILE *input;
unsigned char buffer[4];

struct WAVE_FILE sample;
struct COMPRESSED_WAVE_FILE compressedSample;

unsigned long numSamples;
unsigned int sampleSize;

time_t start, stop;
double compressionTime;
double decompressionTime;


int main (int argc, char **argv) {
    if (argc < 2) {
        perror("\nPlease input a valid .wav file\n");
        return printf("\nPlease input a valid .wav file\n");
    }

    printf("\nInput Wave Filename:\t\t%s\n", argv[1]);

    input = fopen(argv[1], "rb");  //read in binary mode
    if (input == NULL) {
        printf("Error opening file %s", argv[1]);
    }

    return 0;

}


void deconstructWave(){
    printf("HEADERS:");
    printf("start reading chunk_id");
    fread(sample.header.chunk_id, sizeof(sample.header.chunk_id), 1, input);
    // printf("read remaining bits");
    fread(buffer, sizeof(buffer), 1, input);
    sample.header.chunk_size = (buffer[0]) | (buffer[1] << 8) | (buffer[2])



}
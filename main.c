#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include <time.h>



FILE *input;
unsigned char bigBuffer[4];
unsigned char littleBuffer[2];


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




void readSample(){
    printf("HEADERS:\n");
    printf("read chunk_id -- 4bytes\n");
    fread(sample.header.chunk_id, sizeof(sample.header.chunk_id), 1, input);

    printf("read chunk_size -- 4bytes\n");
    fread(bigBuffer, sizeof(bigBuffer), 1, input);
    sample.header.chunk_size = ((bigBuffer[0]) | (bigBuffer[1] << 8) | (bigBuffer[2] << 16) | (bigBuffer[3] << 24)); // converting to big endian

    printf("read chunk_type -- 4bytes\n");
    fread(sample.header.chunk_type, sizeof(sample.header.chunk_type), 1, input); //"WAV"

    printf("read format -- 4bytes\n");
    fread(sample.header.format, sizeof(sample.header.format), 1, input); //"fmt"

    printf("read subchunk1_size -- 4bytes\n");
    fread(bigBuffer, sizeof(bigBuffer), 1, input);
    sample.header.subchunk1_size = ((bigBuffer[0]) | (bigBuffer[1] << 8) | (bigBuffer[2] << 16) | (bigBuffer[3] << 24)); // converting to big endian

    printf("read audio_format -- 2bytes\n");
    fread(littleBuffer, sizeof(littleBuffer), 1, input);
    sample.header.audio_format = ((littleBuffer[0]) | (littleBuffer[1] << 8)); // converting to big endian

    printf("read num_channels -- 2bytes\n");
    fread(littleBuffer, sizeof(littleBuffer), 1, input);
    sample.header.num_channels = ((littleBuffer[0]) | (littleBuffer[1] << 8)); // converting to big endian

    printf("read sample_rate -- 4bytes\n");
    fread(bigBuffer, sizeof(bigBuffer), 1, input);
    sample.header.sample_rate = ((bigBuffer[0]) | (bigBuffer[1] << 8) | (bigBuffer[2] << 16) | (bigBuffer[3] << 24)); // converting to big endian

    printf("read byte_rate -- 4bytes\n");
    fread(bigBuffer, sizeof(bigBuffer), 1, input);
    sample.header.byte_rate = ((bigBuffer[0]) | (bigBuffer[1] << 8) | (bigBuffer[2] << 16) | (bigBuffer[3] << 24)); // converting to big endian

    printf("read block_align -- 2bytes\n");
    fread(littleBuffer, sizeof(littleBuffer), 1, input);
    sample.header.block_align = ((littleBuffer[0]) | (littleBuffer[1] << 8)); // converting to big endian

    printf("read bits_per_sample -- 2bytes\n");
    fread(littleBuffer, sizeof(littleBuffer), 1, input);
    sample.header.bits_per_sample = ((littleBuffer[0]) | (littleBuffer[1] << 8)); //converting to big endian

    printf("DATA:\n");
    printf("read subchunk2_id -- 4bytes\n");
    fread(sample.rawData.subchunk2_id, sizeof(sample.rawData.subchunk2_id), 1, input);

    printf("read subchunk2_size -- 4bytes\n");
    fread(bigBuffer, sizeof(bigBuffer), 1, input);
    sample.rawData.subchunk2_size = ((bigBuffer[0]) | (bigBuffer[1] << 8) | (bigBuffer[2] << 16) | (bigBuffer[3] << 24)); // converting to big endian






 











}
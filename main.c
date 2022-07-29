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

    readSample();
    printHeader();

    return 0;

}


void readSample(){
    printf("HEADERS:\n");
    printf("read chunk_id -- 4bytes\n");
    fread(sample.header.chunk_id, sizeof(sample.header.chunk_id), 1, input);
    //printf("\n(01-04): chunk_id\t\t%u\n", sample.header.chunk_id);

    printf("read chunk_size -- 4bytes\n");
    fread(bigBuffer, sizeof(bigBuffer), 1, input);
    sample.header.chunk_size = ((bigBuffer[0]) | (bigBuffer[1] << 8) | (bigBuffer[2] << 16) | (bigBuffer[3] << 24)); // converting to big endian
    //printf("\n(05-08): chunk_size\t\t%u\n", sample.header.chunk_size);

    printf("read chunk_type -- 4bytes\n");
    fread(sample.header.chunk_type, sizeof(sample.header.chunk_type), 1, input); //"WAV"
    //printf("\n(09-12): chunk_type\t\t%u\n", sample.header.chunk_type);

    printf("read format -- 4bytes\n");
    fread(sample.header.format, sizeof(sample.header.format), 1, input); //"fmt"
    //printf("\n(13-16): format\t\t%u\n", sample.header.format);

    printf("read subchunk1_size -- 4bytes\n");
    fread(bigBuffer, sizeof(bigBuffer), 1, input);
    sample.header.subchunk1_size = ((bigBuffer[0]) | (bigBuffer[1] << 8) | (bigBuffer[2] << 16) | (bigBuffer[3] << 24)); // converting to big endian
    //printf("\n(17-20): subchunk1_size\t\t%u\n", sample.header.subchunk1_size);

    printf("read audio_format -- 2bytes\n");
    fread(littleBuffer, sizeof(littleBuffer), 1, input);
    sample.header.audio_format = ((littleBuffer[0]) | (littleBuffer[1] << 8)); // converting to big endian
    //printf("\n(21-22): audio_format\t\t%u\n", sample.header.audio_format);

    printf("read num_channels -- 2bytes\n");
    fread(littleBuffer, sizeof(littleBuffer), 1, input);
    sample.header.num_channels = ((littleBuffer[0]) | (littleBuffer[1] << 8)); // converting to big endian
    //printf("\n(23-24): num_channels\t\t%u\n", sample.header.num_channels);

    printf("read sample_rate -- 4bytes\n");
    fread(bigBuffer, sizeof(bigBuffer), 1, input);
    sample.header.sample_rate = ((bigBuffer[0]) | (bigBuffer[1] << 8) | (bigBuffer[2] << 16) | (bigBuffer[3] << 24)); // converting to big endian
    //printf("\n(25-28): sample_rate\t%u\n", sample.header.sample_rate);

    printf("read byte_rate -- 4bytes\n");
    fread(bigBuffer, sizeof(bigBuffer), 1, input);
    sample.header.byte_rate = ((bigBuffer[0]) | (bigBuffer[1] << 8) | (bigBuffer[2] << 16) | (bigBuffer[3] << 24)); // converting to big endian
    //printf("\n(29-32): byte_rate\t%u\n", sample.header.byte_rate);

    printf("read block_align -- 2bytes\n");
    fread(littleBuffer, sizeof(littleBuffer), 1, input);
    sample.header.block_align = ((littleBuffer[0]) | (littleBuffer[1] << 8)); // converting to big endian
    //printf("\n(33-34): block_align\t\t%u\n", sample.header.block_align);

    printf("read bits_per_sample -- 2bytes\n");
    fread(littleBuffer, sizeof(littleBuffer), 1, input);
    sample.header.bits_per_sample = ((littleBuffer[0]) | (littleBuffer[1] << 8)); //converting to big endian
    //printf("\n(35-36): bits_per_sample\t%u\n", sample.header.bits_per_sample);




    printf("DATA INFO:\n");

    printf("Parsing to 'data' marker");
    fread(bigBuffer, sizeof(bigBuffer), 1, input);
    int dataMarker = strcmp(bigBuffer, "data");
    int endInput = sample.header.chunk_size - 40;
    if(dataMarker){
        while (endInput >= 0){
            fread(bigBuffer, sizeof(bigBuffer), 1, input);
            dataMarker = strcmp(bigBuffer, "data");
            if(!dataMarker){
                break;
            }
            fseek(input, -3, SEEK_CUR); //SEEK_CUR – It moves file pointer position to given location.
        }
    }
    
    printf("read subchunk2_id -- 4bytes\n");
    strcpy(sample.rawData. subchunk2_id, bigBuffer);
    //printf("\n(37-40): subchunk2_id\t\t%u\n", sample.rawData.subchunk2_id);

    printf("read subchunk2_size -- 4bytes\n");
    fread(bigBuffer, sizeof(bigBuffer), 1, input);
    sample.rawData.subchunk2_size = ((bigBuffer[0]) | (bigBuffer[1] << 8) | (bigBuffer[2] << 16) | (bigBuffer[3] << 24)); // converting to big endian
    //printf("\n(37-40): subchunk2_size\t\t%u\n", sample.rawData.subchunk2_size);


    printf("DONE\n");

    printf("BEGIN READING SAMPLE DATA\n");

    printf("Ensure audio format is PCM\n");

    if(sample.header.audio_format == 1){

        printf("Calculate number of samples and sample size for calloc\n");

        //NumSamples = chunk_size / (num_channels * bits_per_sample / 8) (converte to bytes)
        numSamples = (sample.header.chunk_size / (sample.header.num_channels*(sample.header.bits_per_sample/8)));
        printf("numSamples: %lu \n", numSamples);

        sampleSize = (sample.header.num_channels * sample.header.bits_per_sample)/8; //should be under 2 bytes

        printf("sampleSize: %lu \n", sampleSize);

        printf("allocate memory in data chunk to store numsamples*samplesize");

        sample.rawData.sampleData = calloc(numSamples, sampleSize);

        printf("begin reading samples");

        int n = 0;
        while(n < numSamples){
            fread(littleBuffer, sampleSize, 1, input);
            sample.rawData.sampleData[n] = ((littleBuffer[0]) | (littleBuffer[1] << 8));
            n++;
        }

        printf("DONE!");
    } else{
        printf("Invalid Input");
    }

}

void printHeader() {
    printf("Display Wave Headers:\t\t...\n");

    fwrite("(01-04): chunk_id\t\t", 1, 19, stdout);
    fwrite(sample.header.chunk_id, sizeof(sample.header.chunk_id), 1, stdout);

    printf("\n(05-08): chunk_size\t\t%u", sample.header.chunk_size);

    fwrite("\n(09-12): chunk_type\t\t", 1, 21, stdout);
    fwrite(sample.header.chunk_type, sizeof(sample.header.chunk_type), 1, stdout);

    fwrite("\n(13-16): format\t\t", 1, 20, stdout);
    fwrite(sample.header.format, sizeof(sample.header.format), 1, stdout);
    
    printf("\n(17-20): subchunk1_size\t\t%u", sample.header.subchunk1_size);
    printf("\n(21-22): audio_format\t\t%u", sample.header.audio_format);
    printf("\n(23-24): num_channels\t\t%u", sample.header.num_channels);
    printf("\n(25-28): sample_rate\t%u", sample.header.sample_rate);
    printf("\n(29-32): byte_rate\t%u", sample.header.byte_rate);
    printf("\n(33-34): block_align\t\t%u", sample.header.block_align);
    printf("\n(35-36): bits_per_sample\t%u", sample.header.bits_per_sample);

    fwrite("\n(37-40): subchunk2_id\t\t", 1, 20, stdout);
    fwrite(sample.rawData.subchunk2_id, sizeof(sample.rawData.subchunk2_id), 1, stdout);

    printf("\n(41-44): subchunk2_size\t\t%u", sample.rawData.subchunk2_size);

    printf("\n...\nDisplaying Wave Headers:\tCOMPLETE\n\n");
}

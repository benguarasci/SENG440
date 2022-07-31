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

    printf("testing data compression methods\n\n\n\n\n");

    compress_data();
    decompress_data();

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

    printf("Parsing to 'data' marker\n");
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
    strcpy(sample.rawData.subchunk2_id, bigBuffer);
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

        printf("allocate memory in data chunk to store numsamples*samplesize\n");

        sample.rawData.sampleData = calloc(numSamples, sampleSize);

        printf("begin reading samples\n");

        int n = 0;
        while(n < numSamples){
            fread(littleBuffer, sampleSize, 1, input);
            sample.rawData.sampleData[n] = ((littleBuffer[0]) | (littleBuffer[1] << 8));
            n++;
        }

        printf("DONE!\n");
    } else{
        printf("Invalid Input\n");
    }

}


void compress_data(){
	printf("Allocate data for compressed samples\n");
	compressedSample.compressedData.sampleData = calloc(numSamples, sizeof(char)); //will only be 2 bytes after mu compression
    if (compressedSample.compressedData.sampleData == NULL) {
        printf("Could not allocate enough memory to store compressed data samples\n");
        return;
    }
	printf("before init\n");
    int sample_magnitude;
    short a_sample;
    int sample_sign;
    printf("after init\n");

    int n = 0;
    while(n < numSamples){
        // printf("start while\n");
        a_sample = (sample.rawData.sampleData[n] >> 2);
        // printf("init a_sample\n");
        if (a_sample >= 0){
            //positive
            sample_sign = 1;
            sample_magnitude = a_sample + 33;
        }else{
            //negative
            sample_sign = 0;
            sample_magnitude = -a_sample + 33;
        }
        // printf("linear to mu\n\n\n");
        // LinearToMuLawSample(a_sample);
        // printf("Mulaw\n\n\n");
        compressedSample.compressedData.sampleData[n]=mu_law(sample_sign, sample_magnitude);
        n++;
    }
}


//invert above logic

decompress_data(){
    int n = 0;
    __uint8_t a_sample;
    unsigned short sample_magnitude;
    short sample_sign;

    while(n < numSamples){
        a_sample = ~compressedSample.compressedData.sampleData[n];
        sample_sign = decode_sign(a_sample);
        sample_magnitude = decode_magnitude(a_sample) - 33;
        short rebuilt_sample = rebuild_sample(sample_sign, sample_magnitude) << 2;
        sample.rawData.sampleData[n] = rebuilt_sample;
        n++;
    }

    printf("Done Decompression");

}


const int cBias = 0x84;

const int cClip = 32635;

static char MuLawCompressTable[256] =

{

     0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,

     4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,

     5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,

     5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,

     6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,

     6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,

     6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,

     6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,

     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,

     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,

     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,

     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,

     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,

     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,

     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,

     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7

};

unsigned char LinearToMuLawSample(short sample)

{

     int sign = (sample >> 8) & 0x80;

     if (sign)

          sample = (short)-sample;

     if (sample > cClip)

          sample = cClip;

     sample = (short)(sample + cBias);

     int exponent = (int)MuLawCompressTable[(sample>>7) & 0xFF];

     int mantissa = (sample >> (exponent+3)) & 0x0F;

     int compressedByte = ~ (sign | (exponent << 4) | mantissa);

     printf(compressedByte);

     return (unsigned char)compressedByte;

}




__uint8_t mu_law(int sign, int magnitude){
    // printf("start mulaw\n");
int chord, step, codeword;
    if (magnitude & (1 << 12)) {
        chord = 0x7;
        step = (magnitude >> 8) & 0xF;
        // printf("if 1\n");
    } 
    else if (magnitude & (1 << 11)) {
        chord = 0x6;
        step = (magnitude >> 7) & 0xF;
        // printf("if 2\n");
    } 
    else if (magnitude & (1 << 10)) {
        chord = 0x5;
        step = (magnitude >> 6) & 0xF;
        // printf("if 3\n");
    } 
    else if (magnitude & (1 << 9)) {
        chord = 0x4;
        step = (magnitude >> 5) & 0xF;
        // printf("if 4\n");
    } 
    else if (magnitude & (1 << 8)) {
        chord = 0x3;
        step = (magnitude >> 4) & 0xF;
        // printf("if 5\n");
    } 
    else if (magnitude & (1 << 7)) {
        chord = 0x2;
        step = (magnitude >> 3) & 0xF;
        // printf("if 6\n");
    } 
    else if (magnitude & (1 << 6)) {
        chord = 0x1;
        step = (magnitude >> 2) & 0xF;
        // printf("if 7\n");
    } 
    else if (magnitude & (1 << 5)) {
        chord = 0x0;
        step = (magnitude >> 1) & 0xF;
        // printf("if 8\n");
    } 
    else {
        chord = 0x0;
        step = magnitude;
        // printf("else\n");
    }
    codeword = (sign << 7) | (chord << 4) | step;
    // printf("final print\n");

    // printf(codeword);
    return (__uint8_t) ~codeword;
}

// decode magnitude from the sign, chord and step bits in the codeword
unsigned short decode_magnitude(char codeword) {
    int chord = (codeword & 0x70) >> 4;
    int step = codeword & 0x0F;
    int msb = 1, lsb = 1;
    int magnitude;
    
    if (chord == 0x7) {
        magnitude = (lsb << 7) | (step << 8) | (msb << 12);
    }
    else if (chord == 0x6) {
        magnitude = (lsb << 6) | (step << 7) | (msb << 11);
    }
    else if (chord == 0x5) {
        magnitude = (lsb << 5) | (step << 6) | (msb << 10);
    }
    else if (chord == 0x4) {
        magnitude = (lsb << 4) | (step << 5) | (msb << 9);
    }
    else if (chord == 0x3) {
        magnitude = (lsb << 3) | (step << 4) | (msb << 8);
    }
    else if (chord == 0x2) {
        magnitude = (lsb << 2) | (step << 3) | (msb << 7);
    }
    else if (chord == 0x1) {
        magnitude = (lsb << 1) | (step << 2) | (msb << 6);
    }
    else if (chord == 0x0) {
        magnitude = lsb | (step << 1) | (msb << 5);
    }

    return (unsigned short) magnitude;
}

short decode_sign(__uint8_t sample){
    return sample & (1 << 7) ? 0 : 1;
}

short rebuild_sample(short sample_sign, unsigned short sample_magnitude){
    return (short) (sample_sign ? sample_magnitude : -sample_magnitude);
}



void printHeader() {
    printf("Display Wave Headers:\t\t...\n");

    fwrite("(01-04): chunk_id\t\t", 1, 19, stdout);
    fwrite(sample.header.chunk_id, sizeof(sample.header.chunk_id), 1, stdout);

    printf("\n(05-08): chunk_size\t\t%u", sample.header.chunk_size);

    fwrite("\n(09-12): chunk_type\t\t", 1, 21, stdout);
    fwrite(sample.header.chunk_type, sizeof(sample.header.chunk_type), 1, stdout);

    fwrite("\n(13-16): format\t\t", 1, 25, stdout);
    fwrite(sample.header.format, sizeof(sample.header.format), 1, stdout);
    
    printf("\n(17-20): subchunk1_size\t\t%u", sample.header.subchunk1_size);
    printf("\n(21-22): audio_format\t\t%u", sample.header.audio_format);
    printf("\n(23-24): num_channels\t\t%u", sample.header.num_channels);
    printf("\n(25-28): sample_rate\t%u", sample.header.sample_rate);
    printf("\n(29-32): byte_rate\t%u", sample.header.byte_rate);
    printf("\n(33-34): block_align\t\t%u", sample.header.block_align);
    printf("\n(35-36): bits_per_sample\t%u", sample.header.bits_per_sample);

    fwrite("\n(37-40): subchunk2_id\t\t", 1, 25, stdout);
    fwrite(sample.rawData.subchunk2_id, sizeof(sample.rawData.subchunk2_id), 1, stdout);

    printf("\n(41-44): subchunk2_size\t\t%u", sample.rawData.subchunk2_size);

    printf("\n...\nDisplaying Wave Headers:\tCOMPLETE\n\n");
}

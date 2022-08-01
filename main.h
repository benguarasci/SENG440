
struct HEADER_CHUNK {
  unsigned char chunk_id[4];  //"RIFF" 4byte    
  __uint32_t chunk_size;       //4byte
  unsigned char chunk_type[4];   //"WAV"  4byte
  unsigned char format[4];      //"fmt" 4byte
  __uint32_t subchunk1_size; // 4byte
  __uint16_t audio_format; //2byte
  __uint16_t num_channels; //2byte
  __uint32_t sample_rate; //4byte
  __uint32_t byte_rate; //4byte
  __uint16_t block_align; //2byte
  __uint16_t bits_per_sample; //2byte
};


struct DATA_CHUNK {
    unsigned subchunk2_id[4];  //"data" 4byte
    __uint32_t subchunk2_size; //4byte
    short *sampleData;
};



struct COMPRESSED_DATA_CHUNK {
    unsigned char   subchunk2_id[4];        //"data"
    __uint32_t      subchunk2_size;        
    __uint8_t       *sampleData;        // sampleData = dwSamplesPerSec * wChannels 
};

struct WAVE_FILE{
    struct HEADER_CHUNK         header;
    struct DATA_CHUNK           rawData;
};

struct COMPRESSED_WAVE_FILE {
    struct HEADER_CHUNK                 header;
    struct COMPRESSED_DATA_CHUNK   compressedData;
};



void printHeader();
void readSample();


unsigned char LinearToMuLawSample(short sample);
__uint8_t mu_law(int sign, int magnitude);
__uint16_t decode_magnitude (__uint8_t codeword);
short decode_sign(__uint8_t sample);
short rebuild_sample(short sample_sign, unsigned short sample_magnitude);

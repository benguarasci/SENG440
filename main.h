
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


// Read Wave File
void readWaveFileHeaders();
void readWaveFileDataSamples();
void readWaveFile();

// Mu Law Compression and Decompression
void compressDataSamples();
void decompressDataSamples();

// Helper Functions
short getSignFromSample(short sample);
unsigned short getMagnitudeFromSample(short sample);
unsigned short getMagnitudeFromCodeword(char codeword);
__uint8_t generateCodeword(short sign, unsigned short magnitude);
void convertIntToLittleEndian(__uint32_t chunk);
void convertShortToLittleEndian(__uint16_t chunk);

// Terminal Display and File Writing Functions
void displayWaveHeaders();
void displayWaveDataSamples();
void saveWaveDataSamples();
void displayWaveHeadersAndSaveDataSamples();
void saveCompressedDataSamples();
void saveMuLawWaveFile();
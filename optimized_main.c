int codeword_compression(register int sample) {
    //hardware instruction set
    register const int mode_compression = 1;
    register int codeword_tmp;
    __asm__ ("mulaw %0, %1, %2"
        : "=r" (codeword_tmp)
        : "r" (sample), "r" (mode_compression)); 
        return ( (int)codeword_tmp);
}
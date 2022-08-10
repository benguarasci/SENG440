mu:
	clear
	gcc main.c -o main.o -std=c99
	./main.o bensvoice.wav
test:
	clear
	gcc test.c -o test.o
	./test.o sample.wav
testv:
	clear
	gcc test.c -o test.o
	./test.o bensvoice.wav

clean:
	rm main.o
	rm test.o
opt:
	clear
	gcc main_opt.c -o main_opt.o
	./main_opt.o bensvoice.wav
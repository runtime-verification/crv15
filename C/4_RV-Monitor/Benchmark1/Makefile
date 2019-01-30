all: memtest

memtest: main.c __RVC_Instrument.c __RVC_Instrument.h
	gcc *.c -o memtest
	
clean:
	rm -f memtest *.o


CC = gcc
LIBS = -lm

main: main.o
	$(CC) main.o -o main $(LIBS)

main.o: main.c
	$(CC) -c main.c -o main.o

clean:
	rm main main.o

CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic

all: encode decode error entropy

encode: encode.o bm.o hamming.o
	$(CC) -o encode encode.o bm.o hamming.o bv.o

decode: decode.o bm.o hamming.o
	$(CC) -o decode decode.o bm.o hamming.o bv.o

error: error.o
	$(CC) -o error error.o

entropy: entropy.o
	$(CC) -o entropy entropy.o -lm

encode.o: encode.c
	$(CC) $(CFLAGS) -c encode.c

decode.o: decode.c
	$(CC) $(CFLAGS) -c decode.c

bv.o: bv.c
	$(CC) $(CFLAGS) -c bv.c

bm.o: bm.c bv.o
	$(CC) $(CFLAGS) -c bm.c

hamming.o: hamming.c
	$(CC) $(CFLAGS) -c hamming.c

format:
	clang-format -i -style=file *.[ch]

clean:
	rm -f encode encode.o decode decode.o error error.o entropy entropy.o bv.o bm.o hamming.o

scan-build: clean
	scan-build --use-cc=clang make

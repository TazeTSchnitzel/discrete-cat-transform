CC = cc

CCFLAGS = -std=c99 -Wall -pedantic -O3 -march=haswell

FILES = \
	main.c \
	netpbm.c \
	gamma.c


discrete-cat-transform: $(FILES)
	$(CC) $(CCFLAGS) -o discrete-cat-transform $(FILES)

asm: $(FILES)
	$(CC) -S $(CCFLAGS) $(FILES)

clean:
	rm discrete-cat-transform

clean-asm:
	rm *.s

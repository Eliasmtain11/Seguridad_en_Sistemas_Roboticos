CC = gcc
CFLAGS = -Wall -Wshadow -Wvla

all: caesar breakcaesar

caesar: caesar.c
	$(CC) $(CFLAGS) -o caesar caesar.c $(LDFLAGS)

breakcaesar: breakcaesar.c
	$(CC) $(CFLAGS) -o breakcaesar breakcaesar.c -lm $(LDFLAGS)

clean:
	rm -f caesar breakcaesar key-*.txt

.PHONY: all clean

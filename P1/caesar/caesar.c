#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum {
	FIRST_CHAR = 'A',
	LAST_CHAR = 'Z',
	ALPHABET_LENGHT = LAST_CHAR - FIRST_CHAR + 1
};

int usage(int argc, char *argv[]);
int normalize(char c, int key);

int
usage(int argc, char *argv[])
{
	if (argc != 1) {
		printf("Usage: ./caesar key\n");
		exit(EXIT_FAILURE);
	}

	char *endptr;

	errno = 0;
	long long key = strtoll(argv[0], &endptr, 10);

	if (errno != 0 || endptr == argv[0] || *endptr != '\0') {
		printf("Usage: ./caesar key\n");
		exit(EXIT_FAILURE);
	}

	return (int)key;
}

int
normalize(char c, int key)
{
	c = toupper((unsigned char)c);
	if (c < FIRST_CHAR || c > LAST_CHAR) {
		return -1;
	}
	return (c - FIRST_CHAR) % ALPHABET_LENGHT + key;
}

int
readEncryptStdin(int key)
{
	char buffer[4096];
	ssize_t n;
	int i, result;
	char c;

	while ((n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1)) > 0) {

		for (i = 0; i < n; i++) {

			result = normalize(buffer[i], key);

			if (result < 0) {
				c = buffer[i];

			} else {
				c = (result % ALPHABET_LENGHT) + FIRST_CHAR;

			}
			printf("%c", c);	//Imprimimos los char codificados

			// if((write(STDOUT_FILENO, &c, sizeof(buffer[i]))) == -1) {
			//     fprintf(stderr,"Error while writing STDOUT");
			//     return -1;
			// }
		}
	}

	if (n == -1) {
		fprintf(stderr, "Error while reading STDIN");
		return -1;
	}

	return 0;
}

int
main(int argc, char *argv[])
{
	argv++;
	argc--;

	int key = usage(argc, argv);

	if (readEncryptStdin(key) < 0) {
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

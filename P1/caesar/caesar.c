#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>

enum{
    ALPHABET_LENGTH = 'Z' - 'A' + 1
};

int
strToint(const char *text)
{
	char *end;

	const int key = strtol(text, &end, 10);

	if (*end != '\0') {
		fprintf(stderr,
			    "ERROR: Non-numeric character in the key\n");
		return 0;
	} else {
		return (int)key;
	}
}

int
usage (int argc,  char *argv[]){ // Comprobamos que sean correctas las caracteristicas de los argumentos y la clave.
    int key;
    if (argc != 1) {
        fprintf(stderr, 
                "usage: ./caesar <clave>\n");
        exit(EXIT_FAILURE);
    }

    key = strToint(argv[0]);

    if (key < 0 || key > 25) {
        fprintf(stderr, 
                "The key is not between [0-25]\n");
        exit(EXIT_FAILURE);
    }
    return key;
}

int 
normalize(char c){
    c = toupper((unsigned char)c);
    if(c >= 'A' && c <= 'Z'){
        return c - 'A';
    }
    return -1; // No es una letra de A-Z
}

char
caesar(char c, int key){
    char norm_c = normalize(c);

    if(norm_c < 0){
        return c;
    }

    return (norm_c % ALPHABET_LENGTH) + key;
}

char*
readStdin(char *text, const int key){
    char buffer[4096];
    ssize_t size;

    while((size = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0){
        
    }

}

int
main (int argc, char *argv[])
{
    int key;
    char a;
    argc--;
    argv++;

    key = usage(argc,argv);
    a = caesar('A',key);
    printf("%d",a);
}
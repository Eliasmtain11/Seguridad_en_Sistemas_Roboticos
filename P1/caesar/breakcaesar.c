#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

enum {
	FIRST_CHAR = 'A',
	LAST_CHAR = 'Z',
	ALPHABET_LENGHT = LAST_CHAR - FIRST_CHAR + 1,
	NUM_DIGRAMS = 28,
	NUM_TRIGRAMS = 16,
	MAX_TEXT = 32 * 1024 * 1024,
	BUFFER_SIZE = 8192
};

/* Resultados de descifrar con una clave concreta. */
struct Count {
	long long len;
	long long freq[ALPHABET_LENGHT];
	long long dig;
	long long tri;
	double dist;
};

typedef struct Count Count;

/* Un texto descifrado por clave (26 x 32 MB). */
static char text[ALPHABET_LENGHT][MAX_TEXT];
static size_t textLen;
static Count count[ALPHABET_LENGHT];

static const double english[ALPHABET_LENGHT] = {
	.08167, .01492, .02782, .04253, .12702, .02228, .02015, .06094, .06966,
	.00153, .00772, .04025, .02406, .06749, .07507, .01929, .00095, .05987,
	.06327, .09056, .02758, .00978, .02360, .00150, .01974, .00074
};

static const char digrams[NUM_DIGRAMS][3] = {
	"TH", "HE", "IN", "EN", "NT", "RE", "ER", "AN", "TI", "ES",
	"ON", "AT", "SE", "ND", "OR", "AR", "AL", "TE", "CO", "DE",
	"TO", "RA", "ET", "ED", "IT", "SA", "EM", "RO"
};

static const char trigrams[NUM_TRIGRAMS][4] = {
	"THE", "AND", "THA", "ENT", "ING", "ION", "TIO", "FOR",
	"NDE", "HAS", "NCE", "EDT", "TIS", "OFT", "STH", "MEN"
};

static char isDigram[ALPHABET_LENGHT][ALPHABET_LENGHT];
static char isTrigram[ALPHABET_LENGHT][ALPHABET_LENGHT][ALPHABET_LENGHT];

int normalize(char c);
void buildTables(void);
void decryptStdin(void);
void finish(void);
void writeDecrypted(int key);
void printCandidate(int key);

int
normalize(char c)
{
	c = toupper((unsigned char)c);
	if (c < FIRST_CHAR || c > LAST_CHAR) {
		return -1;
	}
	return c - FIRST_CHAR;
}

void
buildTables(void)
{
	int i;

	for (i = 0; i < NUM_DIGRAMS; i++) {
		isDigram[digrams[i][0] - FIRST_CHAR][digrams[i][1] -
						     FIRST_CHAR] = 1;
	}
	for (i = 0; i < NUM_TRIGRAMS; i++) {
		isTrigram[trigrams[i][0] - FIRST_CHAR][trigrams[i][1] -
						       FIRST_CHAR][trigrams[i][2]
								   - FIRST_CHAR]
		    = 1;
	}
}

/* Lee stdin y, a la vez, descifra con las 26 claves y cuenta. */
void
decryptStdin(void)
{
	char buffer[BUFFER_SIZE];
	ssize_t n;
	size_t i;
	int r, k, d, d1, d2, prev1 = -1, prev2 = -1;

	while ((n = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
		for (i = 0; i < (size_t)n; i++) {
			if (textLen == MAX_TEXT) {
				fprintf(stderr, "Input too large\n");
				exit(EXIT_FAILURE);
			}
			r = normalize(buffer[i]);

			if (r < 0) {
				for (k = 0; k < ALPHABET_LENGHT; k++) {
					text[k][textLen] = buffer[i];
				}
				textLen++;
				if (isspace((unsigned char)buffer[i])) {
					prev1 = prev2 = -1;
				}
				continue;
			}

			for (k = 0; k < ALPHABET_LENGHT; k++) {
				d = (r - k + ALPHABET_LENGHT) % ALPHABET_LENGHT;
				text[k][textLen] = d + FIRST_CHAR;

				count[k].len++;
				count[k].freq[d]++;
				if (prev1 >= 0) {
					d1 = (prev1 - k + ALPHABET_LENGHT) %
					    ALPHABET_LENGHT;
					count[k].dig += isDigram[d1][d];
					if (prev2 >= 0) {
						d2 = (prev2 - k +
						      ALPHABET_LENGHT) %
						    ALPHABET_LENGHT;
						count[k].tri +=
						    isTrigram[d2][d1][d];
					}
				}
			}
			textLen++;
			prev2 = prev1;
			prev1 = r;
		}
	}

	if (n == -1) {
		fprintf(stderr, "Error while reading STDIN\n");
		exit(EXIT_FAILURE);
	}
}

/* Distancia euclídea de las frecuencias de cada clave al inglés. */
void
finish(void)
{
	int k, i;
	double sum, freq, diff;

	for (k = 0; k < ALPHABET_LENGHT; k++) {
		sum = 0;
		for (i = 0; i < ALPHABET_LENGHT; i++) {
			freq = 0;
			if (count[k].len > 0) {
				freq = (double)count[k].freq[i] / count[k].len;
			}
			diff = freq - english[i];
			sum += diff * diff;
		}
		count[k].dist = sqrt(sum);
	}
}

void
writeDecrypted(int key)
{
	char name[32];
	int fd;

	snprintf(name, sizeof(name), "key-%d.txt", key);
	fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1) {
		fprintf(stderr, "Error while creating %s\n", name);
		exit(EXIT_FAILURE);
	}
	

	if (write(fd, text[key], textLen) != (ssize_t)textLen) {
		fprintf(stderr, "Error while writing %s\n", name);
		close(fd);
		exit(EXIT_FAILURE);
	}

	if (close(fd) == -1) {
		fprintf(stderr, "Error while writing %s\n", name);
		exit(EXIT_FAILURE);
	}
}

void
printCandidate(int key)
{
	printf("%d: %.6f, %lld, %lld\n", key, count[key].dist, count[key].dig,
	       count[key].tri);
}

int
main(int argc, char *argv[])
{
	double bestDist = 1e9;
	long long bestDig = -1, bestTri = -1;
	int kDist = 0, kDig = 0, kTri = 0;
	int cand[3], ncand = 0, i, k;

	buildTables();
	decryptStdin();
	finish();

	for (k = 1; k < ALPHABET_LENGHT; k++) {
		if (count[k].dist < bestDist) {
			bestDist = count[k].dist;
			kDist = k;
		}
		if (count[k].dig > bestDig) {
			bestDig = count[k].dig;
			kDig = k;
		}
		if (count[k].tri > bestTri) {
			bestTri = count[k].tri;
			kTri = k;
		}
	}

	if (kDist == kDig && kDig == kTri) {
		cand[ncand++] = kDist;
	} else if (kDig == kTri) {
		cand[ncand++] = kDig;
		cand[ncand++] = kDist;
	} else if (kDist == kDig) {
		cand[ncand++] = kDist;
		cand[ncand++] = kTri;
	} else if (kDist == kTri) {
		cand[ncand++] = kDist;
		cand[ncand++] = kDig;
	} else {
		cand[ncand++] = kDist;
		cand[ncand++] = kDig;
		cand[ncand++] = kTri;
	}

	for (i = 0; i < ncand; i++) {
		printCandidate(cand[i]);
		writeDecrypted(cand[i]);
	}

	exit(EXIT_SUCCESS);
}

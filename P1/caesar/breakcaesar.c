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
	BLOCK_SIZE = 8192,
	BUFFER_SIZE = 8192
};

struct Counts {
	long long len;
	long long c1[ALPHABET_LENGHT];
	long long c2[ALPHABET_LENGHT][ALPHABET_LENGHT];
	long long c3[ALPHABET_LENGHT][ALPHABET_LENGHT][ALPHABET_LENGHT];
};

struct Score {
	double dist;
	long long dig;
	long long tri;
};

typedef struct Score Score;
typedef struct Counts Counts;

static Counts counts;

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

int normalize(char c);
void readLettersStdin(Counts *cnt, int temp_file);
Score score(const Counts *cnt, int key);
void writeDecrypted(int temp_file, int key);
void printCandidate(int key, const Score *s);

int
normalize(char c)
{
	c = toupper((unsigned char)c);
	if (c < FIRST_CHAR || c > LAST_CHAR) {
		return -1;
	}
	return (c - FIRST_CHAR) % ALPHABET_LENGHT;
}

void
readLettersStdin(Counts *cnt, int temp_file)
{
	char buffer[BUFFER_SIZE];
	ssize_t n;
	size_t i;
	int r, prev1 = -1, prev2 = -1;
	char out;
	char block[BLOCK_SIZE];
	size_t blockLen = 0;

	while ((n = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
		for (i = 0; i < (size_t)n; i++) {
			r = normalize(buffer[i]);

			out = buffer[i];
			if (r >= 0) {
				out = r + FIRST_CHAR;
			}
			block[blockLen++] = out;
			if (blockLen == BLOCK_SIZE) {
				if (write(temp_file, block, blockLen) == -1) {
					fprintf(stderr,
						"Error while writing temporary file\n");
					exit(EXIT_FAILURE);
				}
				blockLen = 0;
			}

			if (r < 0) {
				if (isspace((unsigned char)buffer[i])) {
					prev1 = prev2 = -1;
				}
				continue;
			}

			cnt->len++;
			cnt->c1[r]++;	// Sumamos 1 a esa letra
			if (prev1 >= 0) {
				cnt->c2[prev1][r]++;	// Sumamos 1 a ese binomio
			}
			if (prev2 >= 0 && prev1 >= 0) {
				cnt->c3[prev2][prev1][r]++;	// Sumamos 1 a ese trinomio
			}
			prev2 = prev1;
			prev1 = r;
		}
	}

	if (n == -1) {
		fprintf(stderr, "Error while reading STDIN\n");
		exit(EXIT_FAILURE);
	}

	if (blockLen > 0) {
		if (write(temp_file, block, blockLen) == -1) {
			fprintf(stderr, "Error while writing temporary file\n");
			exit(EXIT_FAILURE);
		}
	}
}

Score
score(const Counts *cnt, int key)
{
	Score s;
	double sum = 0, freq, diff;
	int i, pos1, pos2, pos3;

	for (i = 0; i < ALPHABET_LENGHT; i++) {
		freq = 0;
		if (cnt->len > 0) {
			freq =
			    (double)cnt->c1[(i + key) % ALPHABET_LENGHT] /
			    cnt->len;
		}
		diff = freq - english[i];
		sum += pow(diff, 2);
	}
	s.dist = sqrt(sum);

	s.dig = 0;
	for (i = 0; i < NUM_DIGRAMS; i++) {
		pos1 = digrams[i][0] - FIRST_CHAR;
		pos2 = digrams[i][1] - FIRST_CHAR;
		s.dig +=
		    cnt->c2[(pos1 + key) % ALPHABET_LENGHT][(pos2 + key) %
							    ALPHABET_LENGHT];
	}

	s.tri = 0;
	for (i = 0; i < NUM_TRIGRAMS; i++) {
		pos1 = trigrams[i][0] - FIRST_CHAR;
		pos2 = trigrams[i][1] - FIRST_CHAR;
		pos3 = trigrams[i][2] - FIRST_CHAR;
		s.tri += cnt->c3[(pos1 + key) % ALPHABET_LENGHT]
		    [(pos2 + key) % ALPHABET_LENGHT]
		    [(pos3 + key) % ALPHABET_LENGHT];
	}

	return s;
}

void
writeDecrypted(int temp_file, int key)
{
	char name[32], block[BLOCK_SIZE];
	int fd, c;
	ssize_t n, i;

	snprintf(name, sizeof(name), "key-%d.txt", key);	// Crea el nombre del archivo
	fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0644);	// Creamos el archivo
	if (fd == -1) {
		fprintf(stderr, "Error while creating %s\n", name);
		exit(EXIT_FAILURE);
	}

	lseek(temp_file, 0, SEEK_SET);
	while ((n = read(temp_file, block, sizeof(block))) > 0) {
		for (i = 0; i < n; i++) {
			c = block[i];
			if (c >= FIRST_CHAR && c <= LAST_CHAR) {
				c = (c - FIRST_CHAR - key +
				     ALPHABET_LENGHT) % ALPHABET_LENGHT +
				    FIRST_CHAR;
			}
			block[i] = c;
		}
		if (write(fd, block, n) == -1) {
			fprintf(stderr, "Error while writing %s\n", name);
			close(fd);
			exit(EXIT_FAILURE);
		}
	}

	if (n == -1) {
		fprintf(stderr, "Error while reading temporary file\n");
		close(fd);
		exit(EXIT_FAILURE);
	}

	if (close(fd) == -1) {
		fprintf(stderr, "Error while writing %s\n", name);
		exit(EXIT_FAILURE);
	}
}

void
printCandidate(int key, const Score *s)
{
	printf("%d: %.6f, %lld, %lld\n", key, s[key].dist, s[key].dig,
	       s[key].tri);
}

int
main(int argc, char *argv[])
{
	int temp_file = open("temp_file", O_RDWR | O_CREAT | O_TRUNC, 0644);
	Score s[ALPHABET_LENGHT];
	double bestDist = 1e9;
	long long bestDig = -1, bestTri = -1;
	int kDist = 0, kDig = 0, kTri = 0;
	int cand[3], ncand = 0, i, k;

	if (temp_file == -1) {
		fprintf(stderr, "Error while creating temporary file\n");
		exit(EXIT_FAILURE);
	}

	readLettersStdin(&counts, temp_file);

	for (k = 1; k < ALPHABET_LENGHT; k++) {
		s[k] = score(&counts, k);
		if (s[k].dist < bestDist) {
			bestDist = s[k].dist;
			kDist = k;
		}
		if (s[k].dig > bestDig) {
			bestDig = s[k].dig;
			kDig = k;
		}
		if (s[k].tri > bestTri) {
			bestTri = s[k].tri;
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
		printCandidate(cand[i], s);
		writeDecrypted(temp_file, cand[i]);
	}

	close(temp_file);

	exit(EXIT_SUCCESS);
}

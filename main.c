#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "functii.h"
#include "structuri.h"

/* MANESCU Emilian-Claudiu - 313 CB */

int main(int argc, char *argv[]) {
	char buf[CMAX], fisier[LMAX][CMAX];
	int linii_fisier = 0, cuanta;
	void *c;
	if(argc != 3) {
		fprintf(stderr, "Numar gresit de parametri!\n");
		return -1;
	}

	FILE *input = fopen(argv[1], "r");
	if(input == NULL) {
		return -1;
	}

	if(fscanf(input, "%d\n", &cuanta) != 1) {
		return 0;
	}
	while (fgets (buf, CMAX, input)) {
		memcpy(fisier[linii_fisier], buf, CMAX);
		linii_fisier++;
	}

	FILE *output = fopen(argv[2], "w");
	if(output == NULL) {
		return -1;
	}

	c = InitQ(sizeof(TProces));
	executare_comenzi(fisier, linii_fisier, c, output, cuanta);
	DistrQ(&c, EliberareProc);
	fclose(input);
	fclose(output);
	return 0;
}
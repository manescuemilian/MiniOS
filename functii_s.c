#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structuri.h"
#include "functii.h"

/* MANESCU Emilian-Claudiu - 313 CB */

TLG AlocCelula(void *ax, size_t d) {
	TLG aux;
	aux = (TLG)malloc(sizeof(TCelulaG));
	if(!aux) return NULL;
	aux -> urm = NULL;
	aux -> info = malloc(d);
	if(!aux -> info) {
		free(aux);
		return NULL;
	}
	memcpy(aux -> info, ax, d);
	return aux;
}

void AfiLista(TLG L, TF AfiElem, FILE *o) {
	for(; L != NULL; L = L -> urm) {
		if (L -> info != NULL) AfiElem(L -> info, o);
	}
}

int EliminaLista(aLG aL, void *ae, TFCmp fcmp, size_t d) {
	TLG p = *aL;
	TLG aux = AlocCelula(ae, d);
	if(!aux) return 0;
	for(; p != NULL; p = p -> urm) {
		if(p -> info && fcmp(p -> info, aux -> info) == 0) {
			break;
		}
	}
	free(p -> info);
	p -> info = NULL;
	free(aux -> info); free(aux);
	return 1;
}

void DistrugeL(aLG aL) {
	TLG aux;
	while(*aL) {
		aux = *aL;
		*aL = (*aL) -> urm;
		free(aux -> info);
		free(aux);
	}
}

void *InitQ(size_t d) {
	TCoada *c;
	c = (TCoada *)malloc(sizeof(TCoada));
	if(!c) return NULL;
	c -> dime = d;
	c -> ic = c -> sc = NULL;
	return (void *)c;
}

int IntrQ(void *c, void *ae) {
	TLG aux;
	TCoada *c1 = (TCoada *)c;
	aux = AlocCelula(ae, c1 -> dime);
	if(!aux) {
		return 0;
	}
	if(c1 -> ic == NULL && c1 -> sc == NULL) {
		c1 -> ic = c1 -> sc = aux;
	} else {
		c1 -> sc -> urm = aux;
		c1 -> sc = aux;
	}
	return 1;
}

// introducere la inceputul cozii
int IntrQInc(void *c, void *ae) {
	TCoada *c1 = (TCoada *)c;
	TLG aux = AlocCelula(ae, c1 -> dime);
	aux -> urm = c1 -> ic;
	c1 -> ic = aux;
	return 1;
}

int ExtrQ(void *c, void *ae) {
	TLG aux;
	TCoada *c1 = (TCoada *)c;
	aux = c1 -> ic;
	if(VidaQ(c) == 1) {
		c1 -> sc = NULL;
		return 0;
	}
	c1 -> ic = c1 -> ic -> urm;
	memcpy(ae, aux -> info, c1 -> dime);
	free (aux -> info);
	free (aux);
	if(c1 -> ic == NULL) {
		c1 -> sc = NULL;
	}
	return 1;
}

void AfiQ(void *c, TF AfiElem, FILE *o) {
	TCoada *c1 = (TCoada *)c;
	if(c1 -> ic == NULL && c1 -> sc == NULL) {
		return;
	}
	TLG aux = c1 -> ic;
	while (aux != NULL) {
		AfiElem(aux -> info, o);
		aux = aux -> urm;
	}
}

// concatenarea a doua cozi
void ConcatQ(void *d, void *s) {
	TCoada *c1 = (TCoada *)d;
	TCoada *c2 = (TCoada *)s;
	c1 -> sc -> urm = c2 -> ic;
	c1 -> sc = c2 -> sc;
	c2 -> ic = c2 -> sc = NULL;
}

void DistrQ(void **a, Elib f) {
	TCoada *c1 = *(TCoada **)a;
	TLG aux = c1 -> ic, p;
	while (aux != NULL) {
		p = aux; aux = aux -> urm;
		f(p -> info);
		free(p);
	}
	free(*a);
}

int VidaQ(void *c) {
	TCoada *c1 = (TCoada *)c;
	if (c1 -> ic == NULL && c1 -> sc == NULL) {
		return 1;
	}
	return 0;
}

void *InitS(size_t d) {
	TStiva *s;
	s = (TStiva *)malloc (sizeof(TStiva));
	if(!s) return NULL;
	s -> dime = d;
	s -> vf = NULL;
	return (void *)s;
}

int Push(void *a, void *ae) {
	TStiva *s = (TStiva *)a;
	TLG aux = AlocCelula(ae, s -> dime);
	if(!aux) return 0;
	aux -> urm = s -> vf;
	s -> vf = aux;
	return 1;
}

int Pop(void *a, void *ae) {
	TStiva *s = (TStiva *)a;
	TLG aux = s -> vf;
	if (aux == NULL) {
		return 0;
	}
	memcpy(ae, s -> vf -> info, s -> dime);
	s -> vf = aux -> urm;
	free(aux -> info);
	free(aux);
	return 1;
}

int VidaS(void *a) {
	TStiva *s = (TStiva *)a;
	if(s -> vf == NULL) {
		return 1;
	}
	return 0;
}

void DistrS(void **a) {
	TStiva *s = *(TStiva **)a;
	TLG p = s -> vf, aux;
	while(p != NULL) {
		aux = p; p = p -> urm;
		free(aux -> info);
		free(aux);
	}
	free(*a);
}
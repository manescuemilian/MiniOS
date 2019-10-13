#ifndef FUNCTII
#define FUNCTII

#include "structuri.h"
#define LMAX 23000
#define CMAX 25
#define MEM_MAX 3145728UL

int CompC(void *, void *);
void IntrOrd(void *, void *);
void InsOrdL(aLG, void *);

void SortarePID(aLG);
void Defragmentare(aLG);

int Adauga(void *, FILE *, aLG, size_t, int, void *, void *);
int ExistaQ(void *, short);
int Inserare(aLG, void *, size_t, void *, void *, FILE *, int);

short PIDmax (TLG);
unsigned long MinimMem(TLG);
short Extr_PidMin(void *);
TMemorie Extr_MemMin(void *, unsigned long, int *);
int Compara(void *, void *);
int ComparaMem(void *, void *);
int Minim(int, int );

void IntrOrdMem(void *, void *);
void Ruleaza(void *, int, int, void *, void *, aLG, void *);

void print_terminate(void *, FILE *);
void print_asteptare(void *, FILE *);
void Stare(void *, void *, short, FILE *);
void print_stiva(void *, short, FILE *);

void Push_Proc(void *, short, int, FILE *);
void Pop_Proc(void *, short, FILE *);

int timp_term(void *, void *);

void AfiProc(void *, FILE *);
void AfiProcT(void *, FILE *);
void EliberareProc(void *);
void executare_comenzi(char fisier[LMAX][CMAX], int linii, 
	void *c, FILE *o, int cuanta);

#endif
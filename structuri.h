#ifndef STRUCTURI
#define STRUCTURI

typedef void (*TF)(void *, FILE *o);
typedef void (*Elib)(void *);
typedef int (*TFCmp)(void *, void *);

typedef struct celula {
	void *info;
	struct celula *urm;
} TCelulaG, *TLG, **aLG;

typedef struct {
	size_t dime;
	TLG ic, sc;
} TCoada;

typedef struct {
	size_t dime;
	TLG vf;
} TStiva;

typedef struct {
	short PID;
	signed char prioritate;
	int timp_exec;
	int timp;
	int cuanta;
	int info_pushed;
	int push_max;
	unsigned long mem;
	unsigned long mem_inceput;
	unsigned long mem_sf;
	unsigned long mem_fin;
	TStiva *stiva;
} TProces;

typedef struct {
	unsigned long mem_inc;
	unsigned long mem_sf;
} TMemorie;

TLG AlocCelula(void *, size_t);
void AfiLista(TLG, TF, FILE *);
int EliminaLista(aLG, void *, TFCmp, size_t);
void DistrugeL(aLG);

void *InitQ(size_t);
int IntrQ(void *, void *);
int IntrQInc(void *, void *);
int ExtrQ(void *, void *);
void AfiQ(void *, TF, FILE *);
void ConcatQ(void *, void *);
void DistrQ(void **, Elib );
int VidaQ(void *);

void *InitS(size_t );
int Push(void *, void *);
int Pop(void *, void *);
int VidaS(void *);
void DistrS(void **);	
#endif
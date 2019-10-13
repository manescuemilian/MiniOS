#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "functii.h"
#include "structuri.h"

/* MANESCU Emilian-Claudiu - 313 CB */

// functie de comparare pentru a introduce in coada de memorii libere
int CompC(void *a, void *b) {
	TProces *p1 = (TProces *)a;
	TProces *p2 = (TProces *)b;
	int rez = p1 -> prioritate - p2 -> prioritate;
	if(rez != 0) return rez;
	rez = p2 -> timp - p1 -> timp;
	if(rez != 0) return rez;
	rez = p2 -> PID - p1 -> PID;
	return rez;
}

void IntrOrd(void *c, void *ae) { 
	TProces p, *paux; 
	paux = (TProces*)ae; 
	TCoada *c1 = (TCoada *)c; 
	void *caux = InitQ(c1 -> dime); 
	int rez = 0; 
	if(VidaQ(c1) == 1) 
	{ 
		IntrQ(caux, paux); 
	} else { 
		ExtrQ(c1,&p); 
		IntrQ(caux, &p); 
		while(ExtrQ(c1, &p) && (rez = CompC(&p, paux)) >0) { 
			IntrQ(caux, &p); 
		} 

		if(rez < 0) { 
			IntrQ(caux, paux); 
			IntrQ(caux, &p); 
		} else { 
			IntrQ(caux, paux); 
		} 
	} 
	ConcatQ(caux, c); 
	memcpy(c, caux, sizeof(TCoada)); 
	free(caux); 
}

// functie de inserare ordonata in lista de memorie
void InsOrdL(aLG aL, void *info) {
	TProces *p = (TProces *)info, *pr;
	TLG ant = NULL, L = *aL, r, aux;
	aux = AlocCelula(info, sizeof(TProces));
	if(L == NULL) {
		// daca lista este vida
		*aL = aux;
		return;
	}
	pr = (TProces *)(L -> info);
	for(; L && pr -> PID <= p -> PID; ant = L, L = L -> urm) {
		pr = (TProces *)(L -> info);
	}
	r = L;
	if(r != NULL) {
		// se insereaza la inceputul listei
		if(ant == NULL) {
			aux -> urm = *aL;
			*aL = aux;
		} else {
			ant -> urm = aux;
			aux -> urm = r;
		}
	} else {
		// daca am ajuns la sfarsitul listei
		ant -> urm = aux;
	}
}

// sortarea listei de memorie dupa PID-uri
void SortarePID(aLG aL) {
	TLG p;
	TProces *proc;
	TLG lista_noua = NULL;
	// se creeaza o lista noua, ordonata dupa PID
	for(p = *aL; p != NULL; p = p -> urm) {
		if(p -> info != NULL) {
			proc = (TProces *)(p -> info);
			InsOrdL(&lista_noua, (void *) proc);
		}
	}
	DistrugeL(aL);
	*aL = lista_noua;
}

// actualizarea zonelor de memorie de inceput si de sfarsit pentru procesee
void Defragmentare(aLG aL) {
	TLG p;
	TProces *proc;
	int mem_inc = 0;
	SortarePID(aL);
	for(p = *aL; p != NULL; p = p -> urm) {
		proc = (TProces *)(p -> info);
		proc -> mem_inceput = mem_inc;
		proc -> mem_sf = proc -> mem_fin = mem_inc + proc -> mem;
		mem_inc = proc -> mem_sf;
	}
}

// functie de adaugare unui proces in memorie
int Adauga(void *c, FILE *o, aLG aL, size_t dim_s, 
			int cuanta, void *pid, void *mem) {
	TProces p;
	TMemorie m1;
	// initializarea procesului ce va fi adaugat
	p.mem = atoi(strtok(NULL, " "));
	p.push_max = p.mem / 4;
 	p.info_pushed = 0;
	p.timp = atoi(strtok(NULL, " "));
	p.timp_exec = p.timp;
	p.prioritate = atoi(strtok(NULL, " \n"));
	p.cuanta = cuanta;
	p.stiva = InitS(dim_s);
	// ultimul parametru pentru functia de Inserare:
	// 0 - nu a avut loc defragmentarea
	// 1 - a avut loc defragmentarea
	if(!Inserare(aL, &p, ((TCoada *)c) -> dime, pid, mem, o, 0)) {
		void *s = (void *)(p.stiva);
		Defragmentare(aL);
		while(ExtrQ(mem, &m1));
		if(!Inserare(aL, &p, ((TCoada *)c) -> dime, pid, mem, o, 1)) {
			DistrS(&s);
			return 0;
		}
	}
	IntrOrd(c, &p);
	fprintf(o, "Process created successfully: PID: %d, ", p.PID);
	fprintf(o, "Memory starts at 0x%lx.\n", p.mem_inceput);
	return 1;
}

// verifica daca un anumit proces exista in coada de procese
int ExistaQ(void *c, short PID) {
	TProces p;
	TCoada *c1 = (TCoada *)c;
	int exista = 0;
	void *caux = InitQ(c1 -> dime);
	while(ExtrQ(c1, &p)) {
		if(p.PID == PID) {
			exista = 1;
		}
		IntrQ(caux, &p);
	}

	while(ExtrQ(caux, &p)) {
		IntrQ(c1, &p);
	}
	DistrQ(&caux, EliberareProc);
	return exista;
}

// PID-ul maxim din lista de memorie
short PIDmax (TLG L) {
	short max = 0;
	for(; L != NULL; L = L -> urm) {
		TProces *p = (TProces *)(L -> info);
		if(p != NULL && p -> PID > max) {
			max = p -> PID;
		} 
	}
	return max;
}

// memoria minima ocupata de un proces in lista de memorie
unsigned long MinimMem(TLG L) {
	unsigned long min = UINT_MAX;
	for(; L != NULL; L = L -> urm) {
		TProces *p = (TProces *)(L -> info);
		if(p != NULL) {
			min = p -> mem_inceput;
			break;
		}
	}
	
	for(; L != NULL; L = L -> urm) {
		TProces *p = (TProces *)(L -> info);
		if(p != NULL && p -> mem_inceput < min) {
			min = p -> mem_inceput;
		} 
	}
	return min;
}

// extrage PID-ul minim din coada de PID-uri disponibile
short Extr_PidMin(void *c) {
	short PID_min;
	TCoada *c1 = (TCoada *)c;
	void *caux = InitQ(c1 -> dime);
	TProces p;
	ExtrQ(c1, &p); PID_min = p.PID; IntrQ(caux, &p);
	while(ExtrQ(c1, &p)) {
		if(p.PID < PID_min) {
			PID_min = p.PID;
		}
		IntrQ(caux, &p);
	}

	while(ExtrQ(caux, &p)) {
		if(p.PID != PID_min) {
			IntrQ(c1, &p);
		}
	}
	DistrQ(&caux, free);
	return PID_min; 
}

// extrage memoria minima corespunzatoare pentru un proces
TMemorie Extr_MemMin(void *c, unsigned long dim, int *exista) {
	TCoada *c1 = (TCoada *)c;
	void *caux = InitQ(c1 -> dime);
	TMemorie p, min;
	min.mem_inc = UINT_MAX, min.mem_sf = UINT_MAX;
	*exista = 0;

	while(ExtrQ(c1, &p)) {
		if(*exista == 0 && p.mem_sf - p.mem_inc >= dim) {
			*exista = 1;
			memcpy(&min, &p, sizeof(TMemorie));
		} else {
			IntrQ(caux, &p);
		}
	}

	while(ExtrQ(caux, &p)) {
			IntrQ(c1, &p);
	}
	DistrQ(&caux, free);
	return min; 
}

unsigned long Extr_MemMax(TLG L) {
	unsigned long max = 0;
	TProces *paux;
	while(L != NULL) {
		if(L -> info != NULL) {
			paux = (TProces *)(L -> info);
			if(paux -> mem_fin > max) {
				max = paux -> mem_fin;
			}
		}
		L = L -> urm;
	}
	return max;
}

// inserarea propriu-zisa in memorie
int Inserare(aLG aL, void *a, size_t d, void *pid, void *m, 
			FILE *o, int defrag) {
	TLG aux, lista = *aL;
	short PID = 1, PIDaux;
	unsigned long mem = 0, mem_liber = 0;
	int exista = 0;
	TProces *p, *paux;
	TMemorie m1;
	p = (TProces *)a;
	void *caux = InitQ(sizeof(TMemorie));

	if(lista != NULL) mem = MinimMem(lista);
	while((*aL) != NULL) {
		if((*aL) -> info != NULL) {
			paux = (TProces *)((*aL) -> info);
			// calcularea memoriei totale ocupate de procese
			if((*aL) -> urm != NULL) {
				mem += (paux -> mem_sf - paux -> mem_inceput);
			} else {
				paux -> mem_sf = paux -> mem_fin;
				mem += (paux -> mem_fin - paux -> mem_inceput);
			}
		}
		aL = &(*aL) -> urm;
	}

	mem = Extr_MemMax(lista);

	if(VidaQ(pid) == 0) {
		PID = Extr_PidMin(pid);
	} else {
		PIDaux = PIDmax(lista);
		PID = PIDaux + 1;
	}

	//de modificat actualizarea memoriei...
	//actualizarea zonelor continue de memorie libera
	if(VidaQ(m) == 0) {
		//de extras adresa minima care respecta
		m1 = Extr_MemMin(m, p -> mem, &exista);
		if(exista == 1) {
			if(p -> mem <= (m1.mem_sf - m1.mem_inc)) {
				mem =  m1.mem_inc;
				//memoria libera ramasa dupa introducerea unui proces
				mem_liber = (m1.mem_sf - m1.mem_inc) - p -> mem;
			} else {
				IntrQ(caux, &m1);
			}
			while(ExtrQ(caux, &m1)) {
				IntrQ(m, &m1);
			}
		}

	}
	DistrQ(&caux, free);
	p -> PID = PID;
 	p -> mem_inceput = mem;
 	p -> mem_sf = p -> mem_inceput + p -> mem + mem_liber;
 	// daca exista memorie libera dupa introducerea procesului,
 	// actualizam coada de memorii libere
 	if(exista == 1 && mem_liber > 0) {
 		m1.mem_inc += p -> mem;
 		IntrOrdMem(m, &m1);
 	}
 	p -> mem_fin = mem + p -> mem;
 	if(defrag == 1 && p -> mem_fin > MEM_MAX) {
 		fprintf(o, "Cannot reserve memory for PID %d.\n", p -> PID);
 		return 0;
 	} else if(p -> mem_fin > MEM_MAX && defrag == 0) {
 		return 0;
 	}
	aux = AlocCelula(a, d);
	if (!aux) return 0;
	*aL = aux;
	return 1;
}

// functie de comparatie a doua procese, dupa PID
int Compara(void *a, void *b) {
	TProces p1 = *(TProces *)a;
	TProces p2 = *(TProces *)b;
	return p1.PID - p2.PID;
}

// functie de comparatie a doua procese, dupa memoria de inceput
int ComparaMem(void *a, void *b) {
	TMemorie p1 = *(TMemorie *)a;
	TMemorie p2 = *(TMemorie *)b;
	return p1.mem_inc - p2.mem_inc;
}

// introducere ordonata a zonelor de memorie libere,
// cu unirea zonelor de memorie libere adiacente
void IntrOrdMem(void *c, void *m) {
	TCoada *c1 = (TCoada *)c;
	TMemorie *m1 = (TMemorie *)m;
	void *caux = InitQ(c1 -> dime);
	TMemorie p;
	int rez;
	if(VidaQ(c1) == 1) {
		IntrQ(caux, m1);
	} else {
		while(ExtrQ(c1, &p) && (rez = ComparaMem(&p, m1)) < 0) {
			IntrQ(caux, &p);
		}

		if(rez > 0) {
			// daca doua zone de memorie libere sunt una langa alta
			if(m1 -> mem_sf >= p.mem_inc) {
				m1 -> mem_sf = p.mem_sf;
				IntrQ(caux, m1);
			} else {
				IntrQ(caux, m1);
				IntrQ(caux, &p);
			}
		} else {
			IntrQ(caux, m1);
		}

		while(ExtrQ(c1, &p)) {
			IntrQ(caux, &p);
		}
	}

	while(ExtrQ(caux, &p)) {
		IntrQ(c1, &p);
	}
	DistrQ(&caux, free);
}

int Minim(int a, int b) {
	int minim;
	if(a > b) minim = b;
	else if (b > a) minim = a;
	else if (b == a) minim = a;
	return minim;
}

void Ruleaza(void *c, int timp, int cuanta, void *t, 
			void *pid, aLG aL, void *mem) {
	TProces p;
	TMemorie m;
	int timp_rulat, min1, min2;
	while(timp > 0 && VidaQ(c) == 0) {
		ExtrQ(c, &p);
		// timpul rulat = minimul dintre:
		// cuanta procesului, parametrul de la run si timpul ramas unui proces
		min1 = Minim(p.cuanta, p.timp);
		min2 = Minim(p.timp, timp);
		timp_rulat = Minim(min1, min2);
		timp -= timp_rulat;
		p.timp -= timp_rulat;
		if(p.timp <= 0) {
			p.cuanta = 0;
		} else {
			p.cuanta -= timp_rulat;
		}

		if(p.cuanta > 0) {
			// procesul nu si-a terminat cuanta
			// ramane in starea running
			IntrQInc(c, &p);
		} else {
			if(p.timp > 0) {
				// daca procesul si-a terminat cuanta, dar totusi nu timpul
				// cuanta se reactualizeaza
				p.cuanta = cuanta;
				IntrOrd(c, &p);
			} else if (p.timp <= 0) {
				// procesul si-a terminat executia
				// se introduce PID-ul liber in coada de PID-uri
				IntrQ(pid, &(p.PID));
				// se actualizeaza zonele de memorie libera
				m.mem_inc = p.mem_inceput;
				m.mem_sf = p.mem_fin;
				IntrOrdMem(mem, &m);
				// se introduce in coada de procese terminate
				IntrQ(t, &p);
				// se sterge din lista de memorie procesul
				EliminaLista(aL, &p.PID, Compara, sizeof(TProces));
			}
		}
	}
}

// afisarea cozii de procese terminate
void print_terminate(void *t, FILE *o) {
	TProces p;
	TCoada *c = (TCoada *)t;
	int nr = 0;
	void *caux = InitQ(c -> dime);
	while(ExtrQ(c, &p)) {
		nr++;
		IntrQ(caux, &p);
	}

	fprintf(o, "Finished queue:\n[");

	while(ExtrQ(caux, &p)) {
		AfiProcT(&p, o);
		if(nr != 1) {
			fprintf(o, ",\n");	
		}
		IntrQ(c, &p);
		nr--;
	}
	fprintf(o, "]\n");
	DistrQ(&caux, EliberareProc);
}

// afisare cozii de procese in asteptare
void print_asteptare(void *c, FILE *o) {
	TProces p;
	TCoada *c1 = (TCoada *)c;
	int nr = 0, contor = 0;
	void *caux = InitQ(c1 -> dime);
	fprintf(o, "Waiting queue:\n[");
	while (ExtrQ(c1, &p)) {
		nr++;
		IntrQ(caux, &p);
	}

	while(ExtrQ(caux, &p)) {
		// primul proces din coada e de fapt procesul running
		if(contor > 0 && contor < nr) {
			AfiProc(&p, o);
			if(contor < nr - 1) {
				fprintf(o, ",\n");
			}
		}
		IntrQ(c1, &p);
		contor++;
	}
	fprintf(o, "]\n");
	DistrQ(&caux, EliberareProc);
}

// comanda get
void Stare(void *c, void *t, short PID, FILE *o) {
	int contor = 0;
	TProces p;
	TCoada *c1 = (TCoada *)c;
	TCoada *c2 = (TCoada *)t;
	if(ExistaQ(c1, PID) == 0 && ExistaQ(c2, PID) == 0) {
		fprintf(o, "Process %d not found.\n", PID);
		return;
	}
	void *caux = InitQ(c1 -> dime);
	void *caux_t = InitQ(c2 -> dime);
	// cozi auxiliare pentru procesele in asteptare si terminate
	while(ExtrQ(c1, &p)) {
		IntrQ(caux, &p);
	}

	while(ExtrQ(c2, &p)) {
		IntrQ(caux_t, &p);
	}

	contor = 0;
	while(ExtrQ(caux, &p)) {
		if(p.PID == PID) {
			if(contor == 0) {
				fprintf(o, "Process %d is running ", p.PID);
				fprintf(o, "(remaining_time: %d).\n", p.timp);
			} else {
				fprintf(o, "Process %d is waiting ", p.PID);
				fprintf(o, "(remaining_time: %d).\n", p.timp);
			}
		}
		IntrQ(c1, &p);
		contor++;
	}

	while(ExtrQ(caux_t, &p)) {
		if(p.PID == PID) {
			fprintf(o, "Process %d is finished.\n", p.PID);
		}
		IntrQ(c2, &p);
	}
	DistrQ(&caux, EliberareProc);
	DistrQ(&caux_t, EliberareProc);
}

void AfiProc(void *a, FILE *o) {
	TProces *p = (TProces *)a;
	fprintf(o, "(%d: priority = %d, remaining_time = %d)", 
		p -> PID, p -> prioritate, p -> timp);
}

void AfiProcT(void *a, FILE *o) {
	TProces *p = (TProces *)a;
	fprintf(o, "(%d: priority = %d, executed_time = %d)", 
		p -> PID, p -> prioritate, p -> timp_exec);
}

void Push_Proc(void *c, short PID, int info, FILE *o) {
	void *caux;
	TCoada *c1 = (TCoada *)c;
	TProces p;

	if(ExistaQ(c1, PID) == 0) {
		fprintf(o, "PID %d not found.\n", PID);
		return;
	}

	caux = InitQ(c1 -> dime);
	while(ExtrQ(c1, &p)) {
		if(p.PID == PID) {
			if(p.info_pushed < p.push_max) {
				Push(p.stiva, &info);
				p.info_pushed++;
			} else if(p.info_pushed >= p.push_max){
				fprintf(o, "Stack overflow PID %d.\n", PID);
			}
		}
		IntrQ(caux, &p);
	}

	while(ExtrQ(caux, &p)) {
		IntrQ(c1, &p);
	}
	DistrQ(&caux, EliberareProc);
}

void Pop_Proc(void *c, short PID, FILE *o) {
	void *caux;
	TCoada *c1 = (TCoada *)c;
	TProces p;
	int info;

	if(ExistaQ(c1, PID) == 0) {
		fprintf(o, "PID %d not found.\n", PID);
		return;
	}

	caux = InitQ(c1 -> dime);
	while(ExtrQ(c1, &p)) {
		if(p.PID == PID) {
			if(VidaS(p.stiva) == 1) {
				fprintf(o, "Empty stack PID %d.\n", p.PID);
			} else {
				Pop(p.stiva, &info);
				p.info_pushed--;
			}
		}
		IntrQ(caux, &p);
	}

	while(ExtrQ(caux, &p)) {
		IntrQ(c1, &p);
	}
	DistrQ(&caux, EliberareProc);
}

// comanda de finish
int timp_term(void *c, void *t) {
	TProces p;
	TCoada *c1 = (TCoada *)c;
	int timp = 0;
	while(ExtrQ(c1, &p)) {
		timp += p.timp;
		IntrQ(t, &p);
	}

	return timp;
}

void print_stiva(void *c, short PID, FILE *o) {
	void *caux, *saux = NULL;
	TStiva *s1;
	TCoada *c1 = (TCoada *)c;
	TProces p;
	int nr = 0;
	int info;

	if(ExistaQ(c1, PID) == 0) {
		fprintf(o, "PID %d not found.\n", PID);
		return;
	}

	caux = InitQ(c1 -> dime);
	while(ExtrQ(c1, &p)) {
		if(p.PID == PID) {
			if(VidaS(p.stiva) == 0) {
				fprintf(o, "Stack of PID %d: ", p.PID);
				s1 = (TStiva *)(p.stiva);
				saux = InitS(s1 -> dime);
				while(Pop(s1, &info)) {
					nr++;
					Push(saux, &info);
				}

				while(Pop(saux, &info)) {
					if (nr != 1) fprintf(o, "%d ", info);
					 else if(nr == 1) {
						fprintf(o, "%d.\n", info);
					}
					nr--;
					Push(s1, &info);
				}
			} else {
				fprintf(o, "Empty stack PID %d.\n", p.PID);
			}
		}
		IntrQ(caux, &p);
	}

	while(ExtrQ(caux, &p)) {
		IntrQ(c1, &p);
	}
	DistrQ(&caux, EliberareProc);
	if(saux != NULL) free(saux);
}

// functie de eliberare e unui proces (cu tot cu stiva)
void EliberareProc(void *a) {
	TProces *p = (TProces *)a;
	void *info = (void *)(p -> stiva);
	DistrS(&info);
	free(p);
}

void executare_comenzi(char fisier[LMAX][CMAX], int linii, 
	void *c, FILE *o, int cuanta) {
	int i, timp, info;
	void *term = InitQ(((TCoada *)c) -> dime), *pid, *mem;
	pid = InitQ(sizeof(short));
	mem = InitQ(sizeof(TMemorie));
	char *sir, *status;
	short PID;
	TLG L = NULL;

	for (i = 0; i < linii; i++) {
		sir = strtok(fisier[i], " \n");
		switch(sir[0]) {
			case 'a':
				Adauga(c, o, &L, sizeof(int), cuanta, pid, mem);
				break;
			case 'r':
				timp = atoi(strtok(NULL, " \n"));
				Ruleaza(c, timp, cuanta, term, pid, &L, mem);

				break;
			case 'p':
				switch(sir[1]) {
					case 'r':
						status = strtok(NULL, " \n");
						if(status[0] == 'f') {
							print_terminate(term, o);
						} else if(status[0] == 'w') {
							print_asteptare(c, o);
						} else if(status[0] == 's') {
							PID = atoi(strtok(NULL, " \n"));
							print_stiva(c, PID, o);
						}	
						break;
					case 'u':
						PID = atoi(strtok(NULL, " "));
						info = atoi(strtok(NULL, " \n"));
						Push_Proc(c, PID, info, o);
						break;
					case 'o':
						PID = atoi(strtok(NULL, " \n"));
						Pop_Proc(c, PID, o);
						break;
				}
				break;
			case 'g':
				PID = atoi(strtok(NULL, " \n"));
				Stare(c, term, PID, o);
				break;
			case 'f':
				fprintf(o, "Total time: %d\n", timp_term(c, term));
				c = NULL;
				break;
		}
	}

	DistrugeL(&L);
	DistrQ(&pid, free); DistrQ(&mem, free); 
	DistrQ(&term, EliberareProc);
}
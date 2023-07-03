#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#include <stdbool.h>
#pragma warning(disable:4996)
#define _CRTDBG_MAP_ALLOC
#define LINE_SIZE 128

typedef struct InfoReteta Reteta;
typedef struct ListaDubla nodLS;
typedef struct Heap Heap;
typedef struct ListaDeFrecventa Nod;

struct InfoReteta
{
	unsigned int nr;
	char* numePacient;
	char* numeMedic;
	char* dataEmitere;
	float valoare;
};

struct ListaDubla
{
	Reteta info;
	nodLS* next, * prev;
};

struct Heap
{
	unsigned int* vect;
	char nrElem;
};

struct ListaDeFrecventa
{
	char* numeMedic;
	unsigned char nrAp;
	Nod* next;
};

nodLS* creareNodShallow(Reteta r)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = r;
	nou->next = NULL;
	nou->prev = NULL;
	return nou;
}

void insereazaListaDubla(nodLS** cap, nodLS** coada, Reteta r)
{
	nodLS* nou = creareNodShallow(r);
	if (*cap == NULL)
	{
		*cap = *coada = nou;
	}
	else
	{
		nodLS* aux = *cap;
		while (aux->next)
			aux = aux->next;
		nou->prev = aux;
		aux->next = nou;
		*coada = nou;
	}
}

void citireFisier(nodLS** cap, nodLS** coada, const char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	if (!f)
		exit(-1);
	Reteta r;
	*cap = NULL;
	*coada = NULL;
	unsigned char nrRetete;
	char buffer[LINE_SIZE];
	fscanf(f, "%hhu", &nrRetete);
	for (unsigned char i = 0; i < nrRetete; i++)
	{
		fscanf(f, "%u %f", &r.nr, &r.valoare);
		fscanf(f, " %[^\n]", buffer);
		r.numePacient = strdup(buffer);
		fscanf(f, " %[^\n]", buffer);
		r.numeMedic = strdup(buffer);
		fscanf(f, "%s", buffer);
		r.dataEmitere = strdup(buffer);
		insereazaListaDubla(cap, coada, r);
	}
}

void traversareListaCapCoada(nodLS* cap)
{
	nodLS* temp = cap;
	while (temp)
	{
		printf("Nr. reteta: %u\nNume pacient: %s\nNume doctor: %s\nData emitere: %s\nValoare: %.2f RON\n\n", temp->info.nr, temp->info.numePacient, temp->info.numeMedic, temp->info.dataEmitere, temp->info.valoare);
		temp = temp->next;
	}
}

void traversareListaCoadaCap(nodLS* coada)
{
	nodLS* temp = coada;
	while (temp)
	{
		printf("Nr. reteta: %u\nNume pacient: %s\nNume doctor: %s\nData emitere: %s\nValoare: %.2f RON\n\n", temp->info.nr, temp->info.numePacient, temp->info.numeMedic, temp->info.dataEmitere, temp->info.valoare);
		temp = temp->prev;
	}
}

nodLS* retetaMax(nodLS* cap)
{
	float max = 0.0f;
	nodLS* temp = cap;
	nodLS* rez = NULL;
	while (temp)
	{
		if (temp->info.valoare > max)
			max = temp->info.valoare, rez = temp;
		temp = temp->next;
	}
	return rez;
}

void inflatie(nodLS* cap, const char* numePacient, const float procent, bool* flag)
{
	nodLS* temp = cap;
	*flag = false;
	while (temp)
	{
		if (!strcmp(temp->info.numePacient, numePacient))
			temp->info.valoare = (100 + procent) / 100 * temp->info.valoare, * flag = true;
		temp = temp->next;
	}
}

Nod* creareNodDict(char* nume)
{
	Nod* nou = (Nod*)malloc(sizeof(Nod));
	nou->numeMedic = strdup(nume);
	nou->nrAp = 1;
	nou->next = NULL;
	return nou;
}

void insereazaInDict(Nod** cap, char* nume)
{
	Nod* nou = creareNodDict(nume);
	if (*cap == NULL)
		*cap = nou;
	else nou->next = *cap, * cap = nou;
}

Nod* gasesteMedic(Nod* cap, const char* numeMedic)
{
	Nod* temp = cap;
	while (temp)
	{
		if (!strcmp(temp->numeMedic, numeMedic))
			return temp;
		temp = temp->next;
	}
	return NULL;
}

char* medicRetetar(nodLS* cap)
{
	nodLS* temp = cap;
	Nod* listaMedici = NULL;
	char* rez = NULL;
	while (temp)
	{
		Nod* gasit = gasesteMedic(listaMedici, temp->info.numeMedic);
		if (!gasit)
			insereazaInDict(&listaMedici, temp->info.numeMedic);
		else gasit->nrAp++;
		temp = temp->next;
	}
	Nod* aux = listaMedici;
	unsigned char max = 0;
	while (aux)
	{
		if (aux->nrAp > max)
			max = aux->nrAp, free(rez), rez = strdup(aux->numeMedic);
		aux = aux->next;
	}
	while (listaMedici)
	{
		aux = listaMedici;
		listaMedici = listaMedici->next;
		free(aux->numeMedic);
		free(aux);
	}
	return rez;
}

void filtrare(Heap h, char index)
{
	char indexRad = index;
	char st = 2 * index + 1;
	char dr = 2 * index + 2;
	if (st<h.nrElem && h.vect[st] > h.vect[indexRad])
		indexRad = st;
	if (dr<h.nrElem && h.vect[dr] > h.vect[indexRad])
		indexRad = dr;
	if (indexRad != index)
	{
		unsigned int aux = h.vect[indexRad];
		h.vect[indexRad] = h.vect[index];
		h.vect[index] = aux;
		filtrare(h, indexRad);
	}
}

void inserareHeap(Heap* h, const int info)
{
	unsigned int* nou = (int*)malloc(sizeof(unsigned int) * (h->nrElem + 1));
	for (char i = 0; i < h->nrElem; i++)
		nou[i] = h->vect[i];
	nou[h->nrElem] = info;
	h->nrElem++;
	free(h->vect);
	h->vect = nou;
	for (char i = (h->nrElem - 1) / 2; i >= 0; i--)
		filtrare(*h, i);
}

void ListaDubla2Heap(nodLS* cap, Heap* h, const char* data)
{
	nodLS* temp = cap;
	h->nrElem = 0;
	h->vect = NULL;
	while (temp)
	{
		if (!strcmp(temp->info.dataEmitere, data))
			inserareHeap(h, temp->info.nr);
		temp = temp->next;
	}
}

void afisareHeap(Heap h)
{
	for (char i = 0; i < h.nrElem; i++)
		printf("%u ", h.vect[i]);
}

void dezalocareLista(nodLS** cap)
{
	while (*cap)
	{
		nodLS* aux = *cap;
		*cap = (*cap)->next;
		free(aux->info.dataEmitere);
		free(aux->info.numeMedic);
		free(aux->info.numePacient);
		free(aux);
	}
}

void dezalocareHeap(Heap* h)
{
	free(h->vect);
	h->nrElem = -1;
}

void main()
{
	nodLS* cap;
	nodLS* coada;
	Heap h;
	const char* numeFisier = "date.txt";
	const float procent = 3.14f;
	const char* numePacient = "Ion Popescu";
	const char* data = "15/07/2023";
	bool flag;
	citireFisier(&cap, &coada, numeFisier);
	traversareListaCapCoada(cap);
	printf("\n\n\nTraversare lista coada-cap:\n\n");
	traversareListaCoadaCap(coada);
	nodLS* retetaMaxima = retetaMax(cap);
	printf("\n\nReteta cu valoarea maxima este:\n\n");
	printf("Nr. reteta: %u\nNume pacient: %s\nNume doctor: %s\nData emitere: %s\nValoare: %.2f RON\n\n", retetaMaxima->info.nr, retetaMaxima->info.numePacient, retetaMaxima->info.numeMedic, retetaMaxima->info.dataEmitere, retetaMaxima->info.valoare);
	inflatie(cap, numePacient, procent, &flag);
	if (!flag)
	{
		printf("\n\nPacientul %s nu are nicio reteta!\n\n", numePacient);
	}
	else
	{
		printf("\n\nModificarile pentru %s au fost realizate!\nConsultati lista dubla parcursa in ambele sensuri:\n\n", numePacient);
		traversareListaCapCoada(cap);
		printf("\n\n----------------\n\n");
		traversareListaCoadaCap(coada);
	}
	char* rez = medicRetetar(cap);
	printf("\n\nNumele medicului cu cel mai mare nr. de retete emise este: %s\n\n", rez);
	free(rez);
	ListaDubla2Heap(cap, &h, data);
	printf("\n\nHeap-ul:\n\n");
	afisareHeap(h);
	dezalocareLista(&cap);
	dezalocareHeap(&h);
	_CrtDumpMemoryLeaks();
}
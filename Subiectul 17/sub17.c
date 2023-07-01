#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#include <math.h>
#pragma warning(disable:4996)
#define _CRTDBG_MAP_ALLOC
#define LINE_SIZE 128

typedef struct InfoRezervare Rezervare;
typedef struct ArboreBinar nodArb;
typedef struct Vector Vector;
typedef struct Coada nodCoada;

struct InfoRezervare
{
	unsigned int id;
	char* denumire;
	unsigned char nrCamere;
	char* numeClient;
	float suma;
};

struct ArboreBinar
{
	Rezervare info;
	nodArb* st, * dr;
};

struct Vector
{
	unsigned char* vect;
	unsigned char nrElem;
};

struct Coada
{
	nodArb* nod;
	unsigned char nivel;
};

nodArb* creareNodArb(Rezervare r, nodArb* st, nodArb* dr)
{
	nodArb* nou = (nodArb*)malloc(sizeof(nodArb));
	nou->info = r;
	nou->st = st;
	nou->dr = dr;
	return nou;
}



void inserareArb(nodArb** rad, Rezervare r)
{
	if (*rad)
	{
		if (r.id > (*rad)->info.id)
			inserareArb(&(*rad)->dr, r);
		else if (r.id < (*rad)->info.id)
			inserareArb(&(*rad)->st, r);
	}
	else *rad = creareNodArb(r, NULL, NULL);
}

unsigned char citireFisier(nodArb** rad, const unsigned char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	if (!f)
		exit(-1);
	*rad = NULL;
	unsigned char nrRezervari;
	Rezervare r;
	fscanf(f, "%hhu", &nrRezervari);
	char buffer[LINE_SIZE];
	for (unsigned char i = 0; i < nrRezervari; i++)
	{
		fscanf(f, "%u %hhu %f", &r.id, &r.nrCamere, &r.suma);
		fscanf(f, " %[^\n]", buffer);
		r.denumire = strdup(buffer);
		fscanf(f, " %[^\n]", buffer);
		r.numeClient = strdup(buffer);
		inserareArb(rad, r);
	}
	fclose(f);
	return nrRezervari;
}

void inordine(nodArb* rad)
{
	if (rad)
	{
		inordine(rad->st);
		printf("Id rezervare: %u\nDenumire hotel: %s\nNumar camere: %hhu\nNume client: %s\nSuma: %.2f RON\n\n", rad->info.id, rad->info.denumire, rad->info.nrCamere, rad->info.numeClient, rad->info.suma);
		inordine(rad->dr);
	}
}

void preordine(nodArb* rad)
{
	if (rad)
	{
		printf("Id rezervare: %u\nDenumire hotel: %s\nNumar camere: %hhu\nNume client: %s\nSuma: %.2f RON\n\n", rad->info.id, rad->info.denumire, rad->info.nrCamere, rad->info.numeClient, rad->info.suma);
		preordine(rad->st);
		preordine(rad->dr);
	}
}

void contravaloareRezervariClient(const char* numeClient, nodArb* rad, float* total)
{
	if (rad)
	{
		contravaloareRezervariClient(numeClient, rad->st, total);
		if (!strcmp(rad->info.numeClient, numeClient))
			(*total) += rad->info.suma;
		contravaloareRezervariClient(numeClient, rad->dr, total);
	}
}

void deleteMin(nodArb** rad) {

	if (*rad == NULL) {
		return;
	}

	if ((*rad)->st == NULL) {
		nodArb* temp = (*rad)->dr;
		free((*rad)->info.denumire);
		free((*rad)->info.numeClient);
		free(*rad);
		*rad = temp;
	}
	else {
		deleteMin(&(*rad)->st);
	}
}

unsigned char max(unsigned char a, unsigned char b)
{
	return a > b ? a : b;
}

unsigned char inaltimeArb(nodArb* rad)
{
	if (rad != NULL)
		return 1 + max(inaltimeArb(rad->st), inaltimeArb(rad->dr));
	return 0;
}

void numaraNoduriPeNivel(nodArb* rad, Vector* v, const unsigned char nrNoduri)
{
	v->nrElem = inaltimeArb(rad);
	v->vect = (unsigned char*)malloc(sizeof(unsigned char) * v->nrElem);
	for(unsigned char i=0; i<v->nrElem; i++)
		v->vect[i] = 0;
	nodCoada* coada = (nodCoada*)malloc(sizeof(nodCoada) * nrNoduri);
	unsigned char cap_coada = 0;
	unsigned char coada_count = 1;

	coada[0].nod = rad;
	coada[0].nivel = 0;

	while (cap_coada < coada_count)
	{
		nodArb* curent = coada[cap_coada].nod;
		unsigned char nivel_curent = coada[cap_coada].nivel;
		cap_coada++;
		v->vect[nivel_curent]++;
		if (curent->st)
		{
			coada[coada_count].nod = curent->st;
			coada[coada_count].nivel = nivel_curent + 1;
			coada_count++;
		}
		if(curent->dr)
		{
			coada[coada_count].nod = curent->dr;
			coada[coada_count].nivel = nivel_curent + 1;
			coada_count++;
		}
	}
	free(coada);
}

void traversareVector(Vector v)
{
	for (unsigned char i = 0; i < v.nrElem; i++)
		printf("Pe nivelul %hhu al arborelui sunt %hhu noduri.\n", i+1, v.vect[i]);
}

void dezalocareVector(Vector* v)
{
	free(v->vect);
	v->nrElem = 0;
}

void dezalocareArb(nodArb** rad)
{
	if (*rad)
	{
		dezalocareArb(&(*rad)->st);
		dezalocareArb(&(*rad)->dr);
		free((*rad)->info.denumire);
		free((*rad)->info.numeClient);
		free(*rad);
		*rad = NULL;
	}
}

void main()
{
	nodArb* rad;
	Vector v;
	const unsigned char* numeFisier = "date.txt";
	const char* numeClient = "John Smith";
	const unsigned char nrRezervari = citireFisier(&rad, numeFisier);
	preordine(rad);
	float total = 0.0f;
	contravaloareRezervariClient(numeClient, rad, &total);
	if (!total)
	{
		printf("\nNu exista clientul %s in arbore", numeClient);
	}
	else
	{
		printf("\nValoarea totala a rezervarilor clientului %s este %.2f RON.", numeClient, total);
	}
	deleteMin(&rad);
	printf("\n\n\n");
	inordine(rad);
	numaraNoduriPeNivel(rad, &v, nrRezervari);
	traversareVector(v);
	dezalocareArb(&rad);
	dezalocareVector(&v);
	_CrtDumpMemoryLeaks();
}
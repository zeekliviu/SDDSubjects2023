#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#include <stdbool.h>
#pragma warning(disable:4996)
#define _CRTDBG_MAP_ALLOC
#define LINE_SIZE 128

typedef struct CursaFeroviara CF;
typedef struct DataCalendaristica Data;
typedef struct ArboreBinar nodArb;
typedef struct Vector Vector;

struct DataCalendaristica
{
	unsigned char zi, luna;
	unsigned short an;
};

struct CursaFeroviara
{
	unsigned int id;
	Data data;
	unsigned char nrVagoane;
	unsigned short bileteCumparate[2];
	float pretBilete[2];
};

struct ArboreBinar
{
	CF info;
	nodArb* st, * dr;
};

struct Vector
{
	unsigned int id;
};

nodArb* creareNod(nodArb* st, nodArb* dr, CF cursa)
{
	nodArb* nou = (nodArb*)malloc(sizeof(nodArb));
	nou->info = cursa;
	nou->dr = dr;
	nou->st = st;
	return nou;
}

void inserareArb(nodArb** rad, CF cursa)
{
	if (*rad)
	{
		if (cursa.id > (*rad)->info.id)
			inserareArb(&(*rad)->dr, cursa);
		else if (cursa.id < (*rad)->info.id)
			inserareArb(&(*rad)->st, cursa);
	}
	else *rad = creareNod(NULL, NULL, cursa);
}

void citireFisier(nodArb** rad, const unsigned char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	unsigned char nrCurse;
	fscanf(f, "%hhu", &nrCurse);
	CF cursa;
	for (unsigned char i = 0; i < nrCurse; i++)
	{
		fscanf(f, "%u", &cursa.id);
		fscanf(f, "%hhu %hhu %hu", &cursa.data.zi, &cursa.data.luna, &cursa.data.an);
		fscanf(f, "%hhu", &cursa.nrVagoane);
		fscanf(f, "%hu %hu", &cursa.bileteCumparate[0], &cursa.bileteCumparate[1]);
		fscanf(f, "%f %f", &cursa.pretBilete[0], &cursa.pretBilete[1]);
		inserareArb(rad, cursa);
	}
	fclose(f);
}

void inordine(nodArb* rad)
{
	if (rad)
	{
		inordine(rad->st);
		printf("Id tren: %u\nData: %hhu/%hhu/%hu\nNr. vagoane: %hhu\nBilete cumparate la clasa I: %hu\nBilete cumparate la clasa a II-a: %hu\nPret bilet la clasa I: %.2f\nPret bilet la clasa a II-a: %.2f\n\n", rad->info.id, rad->info.data.zi, rad->info.data.luna, rad->info.data.an, rad->info.nrVagoane, rad->info.bileteCumparate[0], rad->info.bileteCumparate[1], rad->info.pretBilete[0], rad->info.pretBilete[1]);
		inordine(rad->dr);
	}
}

void nrBileteLaClasaA2A(nodArb* rad, unsigned int* nrBilete)
{
	if (rad)
	{
		nrBileteLaClasaA2A(rad->st, nrBilete);
		(*nrBilete) += rad->info.bileteCumparate[1];
		nrBileteLaClasaA2A(rad->dr, nrBilete);
	}
}

bool dateEgale(Data d1, Data d2)
{
	return d1.an == d2.an && d1.luna == d2.luna && d1.zi == d2.zi;
}

void incasariMaxime(nodArb* rad, float* incasari)
{
	if (rad)
	{
		incasariMaxime(rad->st, incasari);
		float incasariCursa = rad->info.bileteCumparate[0] * rad->info.pretBilete[0] + rad->info.bileteCumparate[1] * rad->info.pretBilete[1];
		if (incasariCursa > *incasari)
			*incasari = incasariCursa;
		incasariMaxime(rad->dr, incasari);
	}
}

void modificaProcentaj(nodArb* rad, const Data data, const float procent)
{
	if (rad)
	{
		modificaProcentaj(rad->st, data, procent);
		if (dateEgale(rad->info.data, data))
			rad->info.pretBilete[0] = rad->info.pretBilete[0] * (100 + procent) / 100;
		modificaProcentaj(rad->dr, data, procent);
	}
}

void parcurgePentruIncasari(nodArb* rad, CF* cursa, const float incasari)
{
	if (rad)
	{
		parcurgePentruIncasari(rad->st, cursa, incasari);
		float incasariCursa = rad->info.bileteCumparate[0] * rad->info.pretBilete[0] + rad->info.bileteCumparate[1] * rad->info.pretBilete[1];
		if (incasariCursa == incasari)
			*cursa = rad->info;
		parcurgePentruIncasari(rad->dr, cursa, incasari);
	}
}

void nrFrunzele(nodArb* rad, unsigned char* nrFrunze)
{
	if (rad)
	{
		nrFrunzele(rad->st, nrFrunze);
		if (rad->st == NULL && rad->dr == NULL)
			(*nrFrunze)++;
		nrFrunzele(rad->dr, nrFrunze);
	}
}

void populeazaVector(nodArb* rad, Vector* v, unsigned char nrElem, unsigned char* aux)
{
	if (rad)
	{
		populeazaVector(rad->st, v, nrElem, aux);
		if (rad->dr == NULL && rad->st == NULL)
			v[nrElem - (*aux)].id = rad->info.id, (*aux)--;
		populeazaVector(rad->dr, v, nrElem, aux);
	}
}

CF cursaIncasariMaxime(nodArb* rad)
{
	float max = 0;
	incasariMaxime(rad, &max);
	CF cursaCuIncasariMaxime;
	parcurgePentruIncasari(rad, &cursaCuIncasariMaxime, max);
	return cursaCuIncasariMaxime;
}

void alocaVectorFrunze(nodArb* rad, Vector** v, unsigned char* nrElem)
{
	nrFrunzele(rad, nrElem);
	*v = (Vector*)malloc(sizeof(Vector) * (*nrElem));
	unsigned char aux = *nrElem;
	populeazaVector(rad, *v, *nrElem, &aux);
}

void traversareVector(Vector* v, unsigned char dim)
{
	for (unsigned char i = 0; i < dim; i++)
		printf("\nTrenul cu ID: %u", v[i].id);
}

void dezalocareVector(Vector** v, unsigned char* dim)
{
	*dim = -1;
	free((*v));
}

void dezalocaArb(nodArb** rad)
{
	if (*rad)
	{
		dezalocaArb(&(*rad)->st);
		dezalocaArb(&(*rad)->dr);
		free((*rad));
		*rad = NULL;
	}
}

void main()
{
	nodArb* rad = NULL;
	CF cursaMaxima;
	Vector* v;
	unsigned char nrElem = 0;
	unsigned int nrBilete = 0;
	const Data data = { 14,2,2024 };
	const float procent = 12.3f;
	citireFisier(&rad, "date.txt");
	inordine(rad);
	nrBileteLaClasaA2A(rad, &nrBilete);
	printf("\n\nNumarul totul de bilete la clasa a 2-a este: %u\n\n", nrBilete);
	printf("\n\nParcurgerea inordine dupa scumpirea biletelor de la clasa I cu %.2f%% a trenurilor care au plecat la data de %hhu/%hhu/%hu (daca este vreunul)\n\n", procent, data.zi, data.luna, data.an);
	modificaProcentaj(rad, data, procent);
	inordine(rad);
	printf("\n\nCursa cu incasari maxime a fost:\n\n");
	cursaMaxima = cursaIncasariMaxime(rad);
	printf("Id tren: %u\nData: %hhu/%hhu/%hu\nNr. vagoane: %hhu\nBilete cumparate la clasa I: %hu\nBilete cumparate la clasa a II-a: %hu\nPret bilet la clasa I: %.2f\nPret bilet la clasa a II-a: %.2f\n\n", cursaMaxima.id, cursaMaxima.data.zi, cursaMaxima.data.luna, cursaMaxima.data.an, cursaMaxima.nrVagoane, cursaMaxima.bileteCumparate[0], cursaMaxima.bileteCumparate[1], cursaMaxima.pretBilete[0], cursaMaxima.pretBilete[1]);
	printf("\n\nVectorul cu id-urile frunzelor:\n\n");
	alocaVectorFrunze(rad, &v, &nrElem);
	traversareVector(v, nrElem);
	dezalocareVector(&v, &nrElem);
	dezalocaArb(&rad);
	_CrtDumpMemoryLeaks();
}
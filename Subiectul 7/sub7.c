#include <stdio.h>
#include <malloc.h>
#include <crtdbg.h>
#include <string.h>
#include <stdbool.h>
#pragma warning(disable:4996)
#define LINE_SIZE 128
#define _CRTDBG_MAP_ALLOC

typedef struct Spectacol Spectacol;
typedef struct ArboreBinar nodArb;
typedef struct VectorDeSpectacole Vector;
typedef struct VectorDictionar Dict;

struct Spectacol
{
	unsigned char idSpectacol;
	float pretBilet;
	char* numeClient;
	char dataSustinere[10];
	char* locatie;
	unsigned char durata; // in minute => 120 <=> 2h // durata maxima spectacol 255 => 4h si 15 minute
};

struct ArboreBinar
{
	Spectacol info;
	nodArb* st, * dr;
};

struct VectorDeSpectacole
{
	Spectacol* vect;
	unsigned char nrElem;
};

struct VectorDictionar
{
	char* numeClient;
	float costTotal;
};

nodArb* creareNodArb(Spectacol s, nodArb* st, nodArb* dr)
{
	nodArb* nou = (nodArb*)malloc(sizeof(nodArb));
	nou->info = s;
	nou->st = st;
	nou->dr = dr;
	return nou;
}

void inserareArbore(nodArb** rad, Spectacol s)
{
	if (*rad != NULL)
	{
		if (s.idSpectacol < (*rad)->info.idSpectacol)
			inserareArbore(&(*rad)->st, s);
		else if (s.idSpectacol > (*rad)->info.idSpectacol)
			inserareArbore(&(*rad)->dr, s);
	}
	else *rad = creareNodArb(s, NULL, NULL);
}

unsigned char citireFisier(nodArb** rad, const unsigned char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	char buffer[LINE_SIZE];
	Spectacol s;
	unsigned char nrSpectacole;
	fscanf(f, "%hhu", &nrSpectacole);
	for (unsigned char i = 0; i < nrSpectacole; i++)
	{
		fscanf(f, "%hhu", &s.idSpectacol);
		fscanf(f, "%f", &s.pretBilet);
		fscanf(f, " %[^\n]", buffer);
		s.numeClient = strdup(buffer);
		fscanf(f, "%s", s.dataSustinere);
		fscanf(f, " %[^\n]", buffer);
		s.locatie = strdup(buffer);
		fscanf(f, "%hhu", &s.durata);
		inserareArbore(rad, s);
	}
	fclose(f);
	return nrSpectacole;
}

void inordine(nodArb* rad)
{
	if (rad != NULL)
	{
		inordine(rad->st);
		printf("Id spectacol: %hhu\nPret: %.2f\nNume client: %s\nLocatie: %s\nData sustinere: %s\nDurata: %hhu\n\n", rad->info.idSpectacol, rad->info.pretBilet, rad->info.numeClient, rad->info.locatie, rad->info.dataSustinere, rad->info.durata);
		inordine(rad->dr);
	}
}

Spectacol DeepCopy(Spectacol s)
{
	Spectacol nou;
	strcpy(nou.dataSustinere, s.dataSustinere);
	nou.durata = s.durata;
	nou.idSpectacol = s.idSpectacol;
	nou.locatie = strdup(s.locatie);
	nou.numeClient = strdup(s.numeClient);
	nou.pretBilet = s.pretBilet;
	return nou;
}

void parcurgeSiVerifica(nodArb* rad, const unsigned char* data, Vector* v)
{
	if (rad != NULL)
	{
		parcurgeSiVerifica(rad->st, data, v);
		if (!strcmp(rad->info.dataSustinere, data)) {
			v->vect = (Spectacol*)realloc(v->vect, ++v->nrElem * sizeof(Spectacol));
			v->vect[v->nrElem-1] = DeepCopy(rad->info);
		}
		parcurgeSiVerifica(rad->dr, data, v);
	}
}

void parcurgeVectorSpectacole(Vector v)
{
	for (unsigned char i = 0; i < v.nrElem; i++)
		printf("Id spectacol: %hhu\nPret: %.2f\nNume client: %s\nLocatie: %s\nData sustinere: %s\nDurata: %hhu\n\n", v.vect[i].idSpectacol, v.vect[i].pretBilet, v.vect[i].numeClient, v.vect[i].locatie, v.vect[i].dataSustinere, v.vect[i].durata);
}

void arboreLaVectorDict(Dict** vec, nodArb* rad, unsigned char* dimVec)
{
	if (rad)
	{
		arboreLaVectorDict(vec, rad->st, dimVec);
		bool flag = false;
		unsigned char i;
		for (i = 0; i < *dimVec; i++)
			if (!strcmp(rad->info.numeClient, (*vec)[i].numeClient))
			{
				flag = true;
				break;
			}
		if (!flag)
		{
			*vec = (Dict*)realloc(*vec, ++(*dimVec) * sizeof(Dict));
			(*vec)[*dimVec - 1].numeClient = strdup(rad->info.numeClient);
			(*vec)[*dimVec - 1].costTotal = rad->info.pretBilet;
		}
		else
		{
			(*vec)[i].costTotal += rad->info.pretBilet;
		}
		arboreLaVectorDict(vec, rad->dr, dimVec);
	}
}

void afisareVectorDict(Dict* dict, unsigned char dim)
{
	for (unsigned char i = 0; i < dim; i++)
		printf("\nNume: %s\nCost total: %.2f\n\n", dict[i].numeClient, dict[i].costTotal);
}

void dezalocareArb(nodArb** rad)
{
	if (*rad != NULL)
	{
		dezalocareArb(&(*rad)->dr);
		dezalocareArb(&(*rad)->st);
		free((*rad)->info.locatie);
		free((*rad)->info.numeClient);
		free(*rad);
		*rad = NULL;
	}
}

nodArb* creareNodArbNou(Spectacol s, nodArb* st, nodArb* dr)
{
	nodArb* nou = (nodArb*)malloc(sizeof(nodArb));
	nou->info = s;
	nou->st = st;
	nou->dr = dr;
	return nou;
}

void inserareArboreNou(nodArb** rad, Spectacol s)
{
	if (*rad != NULL)
	{
		if (s.idSpectacol < (*rad)->info.durata)
			inserareArboreNou(&(*rad)->st, s);
		else if (s.idSpectacol > (*rad)->info.durata)
			inserareArboreNou(&(*rad)->dr, s);
	}
	else *rad = creareNodArbNou(s, NULL, NULL);
}

void spargereArbore(nodArb* rad, nodArb** radSt, nodArb** radDr)
{
	if (rad->st != NULL)
	{
		inserareArboreNou(radSt, rad->st->info);
		spargereArbore(rad->st, radSt, radDr);
	}
	if (rad->dr != NULL)
	{
		inserareArboreNou(radDr, rad->dr->info);
		spargereArbore(rad->dr, radSt, radDr);
	}
}

void dezalocareVector(Vector* v)
{
	for (unsigned char i = 0; i < v->nrElem; i++)
	{
		free(v->vect[i].numeClient);
		free(v->vect[i].locatie);
	}
	free(v->vect);
	v->nrElem = -1;
}

void dezalocareDict(Dict** dict, unsigned char* dim)
{
	for (unsigned char i = 0; i < *dim; i++)
		free((*dict)[i].numeClient);
	free(*dict);
	*dim = -1;
}

void dezalocareArboreSecundar(nodArb** rad)
{
	if (*rad != NULL)
	{
		dezalocareArboreSecundar(&(*rad)->st);
		dezalocareArboreSecundar(&(*rad)->dr);
		free((*rad));
		*rad = NULL;
	}
}

void main()
{
	nodArb* rad = NULL;
	nodArb* radSt = NULL;
	nodArb* radDr = NULL;
	Vector v;
	v.vect = NULL;
	v.nrElem = 0;
	Dict* dict = NULL;
	unsigned char dim = 0;
	v.nrElem = 0;
	const char data[] = "01.07.2023";
	unsigned char nrSpectacole = citireFisier(&rad, "date.txt");
	inordine(rad);
	parcurgeSiVerifica(rad, data, &v);
	printf("\nSpectacolele care au avut loc in data de %s sunt urmatoarele:\n\n", data);
	parcurgeVectorSpectacole(v);
	arboreLaVectorDict(&dict, rad, &dim);
	printf("\n\nCostul agregat pe clienti:\n\n");
	afisareVectorDict(dict, dim);
	spargereArbore(rad, &radSt, &radDr);
	inserareArboreNou(&radDr, rad->info);
	printf("\nArbore stang:\n");
	inordine(radSt);
	printf("\nArbore drept:\n");
	inordine(radDr);
	dezalocareArb(&rad);
	dezalocareArboreSecundar(&radSt);
	dezalocareArboreSecundar(&radDr);
	dezalocareVector(&v);
	dezalocareDict(&dict, &dim);
	_CrtDumpMemoryLeaks();
}
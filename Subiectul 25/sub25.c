#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#pragma warning(disable:4996)
#define LINE_SIZE 128

typedef struct ABC nodArb;
typedef struct InfoRezervare Rezervare;
typedef struct VectorRezervari Vector;

struct InfoRezervare
{
	unsigned int nrRezervare;
	char* denumireHotel;
	unsigned char nrCamere;
	char* numeClient;
	char* dataRezervare;
};

struct ABC
{
	Rezervare info;
	nodArb* st, * dr;
};

struct VectorRezervari
{
	Rezervare* vect;
	unsigned char nrElem;
};

nodArb* creareNodArb(Rezervare r)
{
	nodArb* nou = (nodArb*)malloc(sizeof(nodArb));
	nou->info = r;
	nou->st = NULL;
	nou->dr = NULL;
	return nou;
}

void inserareArb(nodArb** rad, Rezervare r)
{
	if (*rad)
	{
		if (r.nrRezervare > (*rad)->info.nrRezervare)
			inserareArb(&(*rad)->dr, r);
		else if (r.nrRezervare < (*rad)->info.nrRezervare)
			inserareArb(&(*rad)->st, r);
	}
	else *rad = creareNodArb(r);
}

void citireFisier(nodArb** rad, const char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	if (!f)
	{
		printf("Fisier inexistent!");
		exit(-1);
	}
	*rad = NULL;
	unsigned char nrRez;
	fscanf(f, "%hhu", &nrRez);
	char buffer[LINE_SIZE];
	Rezervare r;
	for (unsigned char i = 0; i < nrRez; i++)
	{
		fscanf(f, "%u %hhu", &r.nrRezervare, &r.nrCamere);
		fscanf(f, " %[^\n]", buffer);
		r.denumireHotel = strdup(buffer);
		fscanf(f, " %[^\n]", buffer);
		r.numeClient = strdup(buffer);
		fscanf(f, "%s", buffer);
		r.dataRezervare = strdup(buffer);
		inserareArb(rad, r);
	}
	fclose(f);
}


void inordine(nodArb* rad)
{
	if (rad)
	{
		inordine(rad->st);
		printf("Numar rezervare: %u\nDenumire hotel: %s\nNumar camere rezervate: %hhu\nNume client: %s\nData: %s\n\n", rad->info.nrRezervare, rad->info.denumireHotel, rad->info.nrCamere, rad->info.numeClient, rad->info.dataRezervare);
		inordine(rad->dr);
	}
}

void nrMax(nodArb* rad, unsigned char* max)
{
	if (rad)
	{
		nrMax(rad->st, max);
		if (rad->info.nrCamere > *max)
			*max = rad->info.nrCamere;
		nrMax(rad->dr, max);
	}
}

Rezervare nrMaximCamere(nodArb* rad, const unsigned char max)
{
	if (rad)
	{
		Rezervare rez = nrMaximCamere(rad->st, max);
		if (rez.nrCamere == max)
			return rez;
		if (rad->info.nrCamere == max)
			return rad->info;
		rez = nrMaximCamere(rad->dr, max);
		if (rez.nrCamere == max)
			return rez;
	}
	Rezervare def;
	def.nrCamere = 0;
	return def;
}

void modificaNrCamere(nodArb* rad, const unsigned int nrRezervare, const unsigned char nrCamereNou)
{
	if (rad)
	{
		modificaNrCamere(rad->st, nrRezervare, nrCamereNou);
		if (rad->info.nrRezervare == nrRezervare)
			rad->info.nrCamere = nrCamereNou;
		modificaNrCamere(rad->dr, nrRezervare, nrCamereNou);
	}
}

void cautaRezervare(nodArb* rad, const unsigned int id)
{
	if (rad)
	{
		cautaRezervare(rad->st, id);
		if(rad->info.nrRezervare == id)
			printf("Numar rezervare: %u\nDenumire hotel: %s\nNumar camere rezervate: %hhu\nNume client: %s\nData: %s\n\n", rad->info.nrRezervare, rad->info.denumireHotel, rad->info.nrCamere, rad->info.numeClient, rad->info.dataRezervare);
		cautaRezervare(rad->dr, id);
	}
}

void nrNoduri(nodArb* rad, unsigned char* nrNod)
{
	if (rad)
	{
		(*nrNod)++;
		nrNoduri(rad->st, nrNod);
		nrNoduri(rad->dr, nrNod);
	}
}

Rezervare DeepCopy(Rezervare r)
{
	Rezervare noua = r;
	noua.dataRezervare = strdup(r.dataRezervare);
	noua.denumireHotel = strdup(r.denumireHotel);
	noua.numeClient = strdup(r.numeClient);
	return noua;
}

void rezervariPanaLa(nodArb* rad, const unsigned int id, Vector* v)
{
	if (rad)
	{
		rezervariPanaLa(rad->st, id, v);
		if (rad->info.nrRezervare <= id)
		{
			v->vect = (Rezervare*)realloc(v->vect, ++v->nrElem * sizeof(Rezervare));
			v->vect[v->nrElem - 1] = DeepCopy(rad->info);
		}
		rezervariPanaLa(rad->dr, id, v);
	}
}

void traversareVector(Vector v)
{
	for (unsigned char i = 0; i < v.nrElem; i++)
		printf("Numar rezervare: %u\nDenumire hotel: %s\nNumar camere rezervate: %hhu\nNume client: %s\nData: %s\n\n", v.vect[i].nrRezervare, v.vect[i].denumireHotel, v.vect[i].nrCamere, v.vect[i].numeClient, v.vect[i].dataRezervare);
}

void dezalocareArb(nodArb** rad)
{
	if (*rad)
	{
		dezalocareArb(&(*rad)->st);
		dezalocareArb(&(*rad)->dr);
		free((*rad)->info.dataRezervare);
		free((*rad)->info.numeClient);
		free((*rad)->info.denumireHotel);
		free((*rad));
		*rad = NULL;
	}
}

void dezalocareVector(Vector* v)
{
	for (unsigned char i = 0; i < v->nrElem; i++)
		free(v->vect[i].dataRezervare), free(v->vect[i].numeClient), free(v->vect[i].denumireHotel);
	free(v->vect);
	v->vect = NULL;
	v->nrElem = 0;
}

void main()
{
	nodArb* rad;
	Vector v;
	v.nrElem = 0;
	v.vect = NULL;
	const char* numeFisier = "date.txt";
	const unsigned int idRezervare = 2654;
	const unsigned char nrCamere = 2;
	citireFisier(&rad, numeFisier);
	inordine(rad);
	unsigned char max = 0;
	nrMax(rad, &max);
	Rezervare maxima = nrMaximCamere(rad, max);
	printf("\nRezervarea cu nr. maxim de camere este:\n");
	printf("Numar rezervare: %u\nDenumire hotel: %s\nNumar camere rezervate: %hhu\nNume client: %s\nData: %s\n\n", maxima.nrRezervare, maxima.denumireHotel, maxima.nrCamere, maxima.numeClient, maxima.dataRezervare);
	printf("\nRezervarea inainte de modificare:\n\n");
	cautaRezervare(rad, idRezervare);
	modificaNrCamere(rad, idRezervare, nrCamere);
	printf("\nRezervarea modificata:\n\n");
	cautaRezervare(rad, idRezervare);
	unsigned char nrNoduriArb = 0;
	nrNoduri(rad, &nrNoduriArb);
	printf("\n\nNumarul de noduri din arbore este: %hhu", nrNoduriArb);
	printf("\n\nVectorul:\n\n");
	rezervariPanaLa(rad, idRezervare, &v);
	traversareVector(v);
	dezalocareArb(&rad);
	dezalocareVector(&v);
	_CrtDumpMemoryLeaks();
}
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#pragma warning(disable:4996)
#define LINE_SIZE 128

typedef struct InfoProiect Proiect;
typedef struct ArboreBinar nodArb;
typedef struct Vector Vector;

struct InfoProiect
{
	unsigned int cod;
	char* titlu;
	char* benefiar;
	unsigned short durata;
	float buget;
};

struct ArboreBinar
{
	Proiect info;
	nodArb* st, * dr;
};

struct Vector
{
	Proiect* vect;
	unsigned char nrElem;
};

nodArb* creareNod(nodArb* st, nodArb* dr, Proiect p)
{
	nodArb* nou = (nodArb*)malloc(sizeof(nodArb));
	nou->info = p;
	nou->st = st;
	nou->dr = dr;
	return nou;
}

void inserareArb(nodArb** rad, Proiect p)
{
	if (*rad)
	{
		if (p.cod > (*rad)->info.cod)
			inserareArb(&(*rad)->dr, p);
		else if (p.cod < (*rad)->info.cod)
			inserareArb(&(*rad)->st, p);
	}
	else *rad = creareNod(NULL, NULL, p);
}

void inordine(nodArb* rad)
{
	if (rad)
	{
		inordine(rad->st);
		printf("Cod: %u\nTitlu: %s\nBeneficiar: %s\nDurata: %hu zile\nBuget: %.2f RON\n\n", rad->info.cod, rad->info.titlu, rad->info.benefiar, rad->info.durata, rad->info.buget);
		inordine(rad->dr);
	}
}

void citireFisier(nodArb** rad, const char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	*rad = NULL;
	if (!f)
		exit(-1);
	unsigned char nrProiecte;
	fscanf(f, "%hhu", &nrProiecte);
	Proiect p;
	char buffer[LINE_SIZE];
	for (unsigned char i = 0; i < nrProiecte; i++)
	{
		fscanf(f, "%u", &p.cod);
		fscanf(f, " %[^\n]", buffer);
		p.titlu = strdup(buffer);
		fscanf(f, " %[^\n]", buffer);
		p.benefiar = strdup(buffer);
		fscanf(f, "%hu", &p.durata);
		fscanf(f, "%f", &p.buget);
		inserareArb(rad, p);
	}
}

void nrAcelasiBeneficiar(nodArb* rad, const char* beneficiar, unsigned char* nr)
{
	if (rad)
	{
		nrAcelasiBeneficiar(rad->st, beneficiar, nr);
		if (!strcmp(rad->info.benefiar, beneficiar))
			(*nr)++;
		nrAcelasiBeneficiar(rad->dr, beneficiar, nr);
	}
}

nodArb* cautaDupaID(nodArb* rad, const unsigned int ID)
{
	if (rad)
	{
		if (ID > rad->info.cod)
			return cautaDupaID(rad->dr, ID);
		else if (ID < rad->info.cod)
			return cautaDupaID(rad->st, ID);
		else return rad;
	}
	return NULL;
}

void modificaBuget(nodArb* rad, const float procent, const unsigned int ID)
{
	if (rad)
	{
		if (ID > rad->info.cod)
			modificaBuget(rad->dr, procent, ID);
		else if (ID < rad->info.cod)
			modificaBuget(rad->st, procent, ID);
		else rad->info.buget *= (100.0 + procent) / 100;
	}
}

void nrFrunze(nodArb* rad, unsigned char* nrF)
{
	if (rad)
	{
		nrFrunze(rad->st, nrF);
		if (rad->dr == NULL && rad->st == NULL)
			(*nrF)++;
		nrFrunze(rad->dr, nrF);
	}
}

void nrElemVector(nodArb* rad, unsigned char* nrElem, const unsigned int prag)
{
	if (rad)
	{
		nrElemVector(rad->st, nrElem, prag);
		if (rad->info.durata > prag)
			(*nrElem)++;
		nrElemVector(rad->dr, nrElem, prag);
	}
}

Proiect DeepCopy(Proiect p)
{
	Proiect nou = p;
	nou.benefiar = strdup(p.benefiar);
	nou.titlu = strdup(p.titlu);
	return nou;
}

void populareVector(nodArb* rad, Vector v, const unsigned short prag, unsigned char* auxInt)
{
	if (rad)
	{
		populareVector(rad->st, v, prag, auxInt);
		if (rad->info.durata > prag)
			v.vect[v.nrElem - (*auxInt)--] = DeepCopy(rad->info);
		populareVector(rad->dr, v, prag, auxInt);
	}
}

void Arb2Vector(nodArb* rad, Vector* v, const unsigned short prag)
{
	v->nrElem = 0;
	nrElemVector(rad, &v->nrElem, prag);
	v->vect = (Proiect*)malloc(sizeof(Proiect) * v->nrElem);
	unsigned char auxInt = v->nrElem;
	populareVector(rad, *v, prag, &auxInt);
}

void traversareVector(Vector v)
{
	for (unsigned char i = 0; i < v.nrElem; i++)
		printf("Cod: %u\nTitlu: %s\nBeneficiar: %s\nDurata: %hu zile\nBuget: %.2f RON\n\n", v.vect[i].cod, v.vect[i].titlu, v.vect[i].benefiar, v.vect[i].durata, v.vect[i].buget);
}

void dezalocareVect(Vector* v)
{
	for (unsigned char i = 0; i < v->nrElem; i++)
		free(v->vect[i].benefiar), free(v->vect[i].titlu);
	free(v->vect);
	v->nrElem = 0;
}

void dezalocareArb(nodArb** rad)
{
	if (*rad)
	{
		dezalocareArb(&(*rad)->st);
		dezalocareArb(&(*rad)->dr);
		free((*rad)->info.benefiar);
		free((*rad)->info.titlu);
		free(*rad);
	}
}

void main()
{
	nodArb* rad;
	Vector v;
	const char* numeFisier = "date.txt";
	const char* beneficiarCautat = "Universitatea Babes-Bolyai Cluj";
	const float procent = 3.14f;
	const unsigned int codProiect = 2849;
	const unsigned short prag = 24;
	citireFisier(&rad, numeFisier);
	unsigned char nrProiecte = 0;
	unsigned char nrF = 0;
	nrAcelasiBeneficiar(rad, beneficiarCautat, &nrProiecte);
	inordine(rad);
	if (!nrProiecte)
	{
		printf("\n\nNu exista proiecte cu beneficiarul %s\n", beneficiarCautat);
	}
	else
	{
		printf("\n\nNumarul proiectelor cu beneficiarul %s sunt %hhu\n", beneficiarCautat, nrProiecte);
	}
	nodArb* rez = cautaDupaID(rad, codProiect);
	if (rez)
	{
		printf("\n\nProiectul inainte de modificare:\n");
		printf("Cod: %u\nTitlu: %s\nBeneficiar: %s\nDurata: %hu zile\nBuget: %.2f RON\n\n", rez->info.cod, rez->info.titlu, rez->info.benefiar, rez->info.durata, rez->info.buget);
		modificaBuget(rad, procent, codProiect);
		printf("\n\nProiectul dupa modificare:\n");
		printf("Cod: %u\nTitlu: %s\nBeneficiar: %s\nDurata: %hu zile\nBuget: %.2f RON\n\n", rez->info.cod, rez->info.titlu, rez->info.benefiar, rez->info.durata, rez->info.buget);

	}
	else
	{
		printf("\n\nProiectul cu ID %u nu se afla in arbore!", codProiect);
	}
	nrFrunze(rad, &nrF);
	printf("\n\nSunt %hhu frunze in arbore\n", nrF);
	Arb2Vector(rad, &v, prag);
	if (v.nrElem)
	{
		printf("\n\nProiectele cu durata mai mare decat %hu\n\n", prag);
		traversareVector(v);
	}
	else
	{
		printf("\n\nNu exista proiecte cu durata mai mare decat %hu!\n", prag);
	}
	dezalocareArb(&rad);
	dezalocareVect(&v);
	_CrtDumpMemoryLeaks();
}
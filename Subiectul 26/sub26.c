#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <crtdbg.h>
#include <stdbool.h>
#include <stdlib.h>
#pragma warning(disable:4996)
#define LINE_SIZE 128

typedef struct DataCalendaristica Data;
typedef struct InfoComanda Comanda;
typedef struct ListaDubla nodLS;
typedef struct VectorComenzi Vector;

struct DataCalendaristica
{
	unsigned char zi, luna;
	unsigned short an;
};

struct InfoComanda
{
	unsigned int nrC;
	Data dataProgramata;
	Data dataEfectuata;
	char* nume;
	float suma;
};

struct ListaDubla
{
	Comanda info;
	nodLS* next, * prev;
};

struct VectorComenzi
{
	int* vector;
	unsigned char nrElem;
};

nodLS* creareNodLista(Comanda c)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = c;
	nou->next = NULL;
	nou->prev = NULL;
	return nou;
}

void inserareLista(nodLS** cap, nodLS** coada, Comanda c)
{
	nodLS* nou = creareNodLista(c);
	if (!*cap)
		*cap = *coada = nou;
	else
	{
		nodLS* temp = *cap;
		while (temp->next)
			temp = temp->next;
		temp->next = nou;
		nou->prev = temp;
		*coada = nou;
	}
}

void citireFisier(nodLS** cap, nodLS** coada, const char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	if (!f)
	{
		printf("Fisierul %s nu exista!", numeFisier);
		exit(-1);
	}
	*cap = *coada = NULL;
	unsigned char nrComenzi;
	fscanf(f, "%hhu", &nrComenzi);
	Comanda c;
	char buffer[LINE_SIZE];
	for (unsigned char i = 0; i < nrComenzi; i++)
	{
		fscanf(f, "%u", &c.nrC);
		fscanf(f, "%hhu %hhu %hu", &c.dataProgramata.zi, &c.dataProgramata.luna, &c.dataProgramata.an);
		fscanf(f, "%hhu %hhu %hu", &c.dataEfectuata.zi, &c.dataEfectuata.luna, &c.dataEfectuata.an);
		fscanf(f, " %[^\n]", buffer);
		c.nume = strdup(buffer);
		fscanf(f, "%f", &c.suma);
		inserareLista(cap, coada, c);
	}
	fclose(f);
}

void afisareComanda(Comanda c)
{
	printf("Nr. comanda: %u\nData programata: %hhu/%hhu/%hu\nData livrata: %hhu/%hhu/%hu\nNume client: %s\nSuma: %.2f RON\n\n", c.nrC, c.dataProgramata.zi, c.dataProgramata.luna, c.dataProgramata.an, c.dataEfectuata.zi, c.dataEfectuata.luna, c.dataEfectuata.an, c.nume, c.suma);
}

void parcurgereCapCoada(nodLS* cap)
{
	while (cap)
	{
		afisareComanda(cap->info);
		cap = cap->next;
	}
}

void parcurgereCoadaCap(nodLS* cap)
{
	while (cap)
	{
		afisareComanda(cap->info);
		cap = cap->prev;
	}
}

unsigned short diffDates(Data d1, Data d2)
{
	return (d1.an - d2.an) * 365 + (d1.luna - d2.luna) * 30 + (d1.zi - d2.zi);
}

Comanda cmdMaxima(nodLS* cap)
{
	unsigned short dif = 0;
	Comanda c;
	while (cap)
	{
		unsigned short diffCurr = diffDates(cap->info.dataEfectuata, cap->info.dataProgramata);
		if (diffCurr > dif)
			dif = diffCurr, c = cap->info;
		cap = cap->next;
	}
	return c;
}

void modificaValoare(nodLS* cap, const float procent)
{
	nodLS* temp = cap;
	float max = 0;
	while (temp)
	{
		if (temp->info.suma > max)
			max = temp->info.suma;
		temp = temp->next;
	}
	while (cap)
	{
		if (cap->info.suma == max)
			cap->info.suma *= (100 - procent) / 100;
		cap = cap->next;
	}
}

typedef struct Dict Dict;

struct Dict
{
	char* nume;
	unsigned char nrComenzi;
};

char* clientComandos(nodLS* cap)
{
	Dict* dict = NULL;
	unsigned char nrElem = 0;
	while (cap)
	{
		bool flag = false;
		unsigned char i;
		for(i=0; i<nrElem; i++)
			if (!strcmp(dict[i].nume, cap->info.nume))
			{
				flag = true;
				break;
			}
		if (!flag)
		{
			dict = (Dict*)realloc(dict, ++nrElem * sizeof(Dict));
			dict[nrElem - 1].nume = strdup(cap->info.nume);
			dict[nrElem - 1].nrComenzi = 1;
		}
		else
		{
			dict[i].nrComenzi++;
		}
		cap = cap->next;
	}
	unsigned char max = 0;
	unsigned char poz;
	for (unsigned char i = 0; i < nrElem; i++)
		if (dict[i].nrComenzi > max)
			max = dict[i].nrComenzi, poz = i;
	char* rez = strdup(dict[poz].nume);
	for (unsigned char i = 0; i < nrElem; i++)
		free(dict[i].nume);
	free(dict);
	return rez;
}

Vector comenziDinLuna(const unsigned char luna, nodLS* cap)
{
	Vector v = { .nrElem = 0, .vector = NULL };
	while (cap)
	{
		if (cap->info.dataProgramata.luna == luna)
		{
			v.vector = (int*)realloc(v.vector, sizeof(int) * (++v.nrElem));
			v.vector[v.nrElem - 1] = cap->info.nrC;
		}
		cap = cap->next;
	}
	return v;
}

void traversareVector(Vector v)
{
	for (unsigned char i = 0; i < v.nrElem; i++)
		printf("%d ", v.vector[i]);
}

void dezalocareVector(Vector* v)
{
	free(v->vector);
	v->vector = NULL;
	v->nrElem = 0;
}

void dezalocareLista(nodLS** cap)
{
	if (!*cap)
		return;
	dezalocareLista(&(*cap)->next);
	free((*cap)->info.nume);
	free(*cap);
	*cap = NULL;
}

void main()
{	
	nodLS* cap, *coada;
	Vector v;
	citireFisier(&cap, &coada, "date.txt");
	parcurgereCapCoada(cap);
	printf("\n\n");
	parcurgereCoadaCap(coada);
	Comanda c = cmdMaxima(cap);
	printf("Comanda cu diferenta maxima este:\n\n");
	afisareComanda(c);
	const float procent = 3.14f;
	const unsigned char luna = 8;
	modificaValoare(cap, procent);
	printf("\n\nLista modificata:\n\n");
	parcurgereCapCoada(cap);
	char* numeClientComandos = clientComandos(cap);
	printf("\n\nNumele clientului comandos este: %s", numeClientComandos);
	free(numeClientComandos);
	v = comenziDinLuna(luna, cap);
	printf("\n\nVectorul\n\n");
	traversareVector(v);
	dezalocareLista(&cap);
	dezalocareVector(&v);
	_CrtDumpMemoryLeaks();
}
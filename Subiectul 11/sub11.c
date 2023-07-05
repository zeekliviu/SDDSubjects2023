#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#include <stdbool.h>
#pragma warning(disable:4996)
#define _CRTDBG_MAP_ALLOC
#define LINE_SIZE 128

typedef struct CursaAeriana CA;
typedef struct ListaDubla nodLS;
typedef struct Vector Vector;
typedef struct DataCalendaristica Data;

struct DataCalendaristica
{
	unsigned char zi, luna;
	unsigned short an;
};

struct CursaAeriana
{
	unsigned int id;
	char* destinatie;
	Data data;
	unsigned short minuteIntarziere;
	unsigned char nrPasageri;
};

struct ListaDubla
{
	CA info;
	nodLS* next, * prev;
};

struct Vector
{
	char* destinatie;
	unsigned short nrTotalPasageri;
};

nodLS* creareNod(CA cursa)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->next = NULL;
	nou->prev = NULL;
	nou->info = cursa;
	return nou;
}

void inserareListaDubla(nodLS** cap, CA cursa)
{
	if (*cap)
	{
		nodLS* nou = creareNod(cursa);
		nodLS* temp = *cap;
		while (temp->next)
			temp = temp->next;
		temp->next = nou;
		nou->prev = temp;
	}
	else *cap = creareNod(cursa);
}

void citireFisier(nodLS** cap, const char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	unsigned char nrCurse;
	char buffer[LINE_SIZE];
	CA cursa;
	fscanf(f, "%hhu", &nrCurse);
	for (unsigned char i = 0; i < nrCurse; i++)
	{
		fscanf(f, "%u", &cursa.id);
		fscanf(f, " %[^\n]", buffer);
		cursa.destinatie = strdup(buffer);
		fscanf(f, "%hhu %hhu %hu", &cursa.data.zi, &cursa.data.luna, &cursa.data.an);
		fscanf(f, "%hu", &cursa.minuteIntarziere);
		fscanf(f, "%hhu", &cursa.nrPasageri);
		inserareListaDubla(cap, cursa);
	}
	fclose(f);
}

void traversareCapCoada(nodLS* cap)
{
	nodLS* temp = cap;
	while (temp)
	{
		printf("Id zbor: %u\nDestinatie: %s\nData plecare: %hhu/%hhu/%hu\nNr. minute intarziere: %hu\nNr. pasageri inregistrati: %hhu\n\n", temp->info.id, temp->info.destinatie, temp->info.data.zi, temp->info.data.luna, temp->info.data.an, temp->info.minuteIntarziere, temp->info.nrPasageri);
		temp = temp->next;
	}
}

void getCoadaLista(nodLS* cap, nodLS** coada)
{
	nodLS* temp = cap;
	while (temp->next)
		temp = temp->next;
	*coada = temp;
}

void traversareCoadaCap(nodLS* coada)
{
	nodLS* temp = coada;
	while (temp)
	{
		printf("Id zbor: %u\nDestinatie: %s\nData plecare: %hhu/%hhu/%hu\nNr. minute intarziere: %hu\nNr. pasageri inregistrati: %hhu\n\n", temp->info.id, temp->info.destinatie, temp->info.data.zi, temp->info.data.luna, temp->info.data.an, temp->info.minuteIntarziere, temp->info.nrPasageri);
		temp = temp->prev;
	}
}

unsigned short nrMinuteIntarziere(nodLS* cap, bool* flag, const unsigned char* destinatie)
{
	unsigned short total = 0;
	nodLS* temp = cap;
	while (temp)
	{
		if (!strcmp(temp->info.destinatie, destinatie))
			total += temp->info.minuteIntarziere, *flag = true;
		temp = temp->next;
	}
	return total;
}

float nrMediuPasageriPeDestinatia(const unsigned char* destinatie, nodLS* cap, bool* flag)
{
	float totalPasageri = 0;
	unsigned char nrCurse = 0;

	nodLS* temp = cap;
	while (temp)
	{
		if (!strcmp(temp->info.destinatie, destinatie))
			totalPasageri += temp->info.nrPasageri, nrCurse++, *flag = true;
		temp = temp->next;
	}
	if (*flag)
		return totalPasageri / nrCurse;
	return 0;
}

void stergeDinLista(nodLS** cap, const unsigned char* destinatie)
{
	nodLS* temp = *cap, * next;
	while (temp)
		if (!strcmp(temp->info.destinatie, destinatie))
		{
			next = temp->next;
			if (temp->prev != NULL)
				temp->prev->next = temp->next;
			else *cap = temp->next;
			if (temp->next != NULL)
				temp->next->prev = temp->prev;
			free(temp->info.destinatie);
			free(temp);
			temp = next;
		}
		else temp = temp->next;
}

unsigned char nrElementeLista(nodLS* cap)
{
	nodLS* temp = cap;
	unsigned char nr = 0;
	while (temp)
	{
		nr++;
		temp = temp->next;
	}
	return nr;
}

void agregarePasageriPeDestinatiiInVector(nodLS* cap, Vector** v, unsigned char* nrElemVector)
{
	*nrElemVector = 0;
	*v = NULL;
	nodLS* temp = cap;
	while (temp)
	{
		bool flag = false;
		unsigned char i;
		for (i = 0; i < *nrElemVector; i++)
				if (!strcmp(temp->info.destinatie, (*v)[i].destinatie))
				{
					flag = true;
					break;
				}
		if (!flag)
		{
			(*v) = (Vector*)realloc(*v, sizeof(Vector) * (++(*nrElemVector)));
			(*v)[*nrElemVector - 1].destinatie = strdup(temp->info.destinatie);
			(*v)[*nrElemVector - 1].nrTotalPasageri = temp->info.nrPasageri;
		}
		else
		{
			(*v)[i].nrTotalPasageri += temp->info.nrPasageri;
		}
		temp = temp->next;
	}
}

void traversareVector(Vector* v, const unsigned char dim)
{
	for (unsigned char i = 0; i < dim; i++)
		printf("Destinatia: %s\nNr. total pasageri: %hu\n\n", v[i].destinatie, v[i].nrTotalPasageri);
}

void dezalocareLista(nodLS** cap)
{
	while (*cap)
	{
		nodLS* aux = *cap;
		*cap = (*cap)->next;
		free(aux->info.destinatie);
		free(aux);
	}
}

void dezalocareVector(Vector** v, unsigned char* dim)
{
	for (unsigned char i = 0; i < *dim; i++)
		free((*v)[i].destinatie);
	free(*v);
	*v = NULL;
	*dim = 0;
}

void main()
{
	nodLS* cap = NULL;
	nodLS* coada = NULL;
	bool gasit = false;
	Vector* v;
	unsigned char nrElemVector = 0;
	unsigned char nrElemLista;
	unsigned short minuteIntarziere;
	const char destinatie[] = "Paris";
	float nrMediuPasageri;
	citireFisier(&cap, "date.txt");
	printf("\nTraversare cap-coada:\n\n");
	traversareCapCoada(cap);
	getCoadaLista(cap, &coada);
	printf("\nTraversare coada-cap\n\n");
	traversareCoadaCap(coada);
	minuteIntarziere = nrMinuteIntarziere(cap, &gasit, destinatie);
	if (!gasit)
	{
		printf("\nDestinatia %s nu a fost gasita in lista!\n", destinatie);
	}
	else
	{
		printf("\nNumarul total de minute intarziate pentru destinatia %s este %hu\n", destinatie, minuteIntarziere);
	}
	gasit = false;
	nrMediuPasageri = nrMediuPasageriPeDestinatia(destinatie, cap, &gasit);
	if (!gasit)
	{
		printf("\nDestinatia %s nu a fost gasita in lista!\n", destinatie);
	}
	else
	{
		printf("\nNumarul mediu de pasageri pentru destinatia %s este %.2f\n", destinatie, nrMediuPasageri);
	}
	printf("\n\nLista de la cap-coada dupa ce au fost sterse cursele cu destinatia %s", destinatie);
	stergeDinLista(&cap, destinatie);
	traversareCapCoada(cap);
	printf("\n\nLista de la coada-cap dupa ce au fost sterse cursele cu destinatia %s\n\n", destinatie);
	getCoadaLista(cap, &coada);
	traversareCoadaCap(coada); 
	agregarePasageriPeDestinatiiInVector(cap, &v, &nrElemVector);
	traversareVector(v, nrElemVector);
	dezalocareLista(&cap);
	dezalocareVector(&v, &nrElemVector);
	_CrtDumpMemoryLeaks();
}
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#include <stdbool.h>
#pragma warning(disable:4996)
#define _CRTDBG_MAP_ALLOC
#define LINE_SIZE 128
#define DIM 12

typedef struct DataCalendaristica Data;
typedef struct InfoProgramare Programare;
typedef struct ListaDubla nodLS;
typedef struct Vector Vector;
typedef struct TabelaDispersie hashT;

struct DataCalendaristica
{
	unsigned char zi, luna;
	unsigned short an;
};

struct InfoProgramare
{
	char* cod;
	Data data;
	char* nume_solicitant;
	char* numeDoctor;
	char* unitate;
	float pret;
	bool platit;
};

struct ListaDubla
{
	Programare info;
	nodLS* next, * prev;
};

struct Vector
{
	Programare* vect;
	unsigned char nrElem;
};

struct TabelaDispersie
{
	nodLS** vect;
	unsigned char nrElem;
};

nodLS* creareNodListaDubla(Programare p)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = p;
	nou->next = NULL;
	nou->prev = NULL;
	return nou;
}

void inserareLista(nodLS** cap, nodLS** coada, Programare p)
{
	if (*cap == NULL)
		*cap = creareNodListaDubla(p), *coada = *cap;
	else
	{
		nodLS* nou = creareNodListaDubla(p);
		nodLS* temp = *cap;
		while (temp->next)
		{
			temp = temp->next;
		}
		nou->prev = temp;
		temp->next = nou;
		*coada = nou;
	}
}

void inserareListaHashT(nodLS** cap, Programare p)
{
	if (*cap == NULL)
		*cap = creareNodListaDubla(p);
	else
	{
		nodLS* nou = creareNodListaDubla(p);
		nou->next = *cap;
		(*cap)->prev = nou;
		*cap = nou;
	}
}

void citireFisier(nodLS** cap, nodLS** coada, const unsigned char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	if (!f)
		exit(-1);
	unsigned char nrProgramari;
	*cap = NULL;
	*coada = NULL;
	Programare p;
	char buffer[LINE_SIZE];
	fscanf(f, "%hhu", &nrProgramari);
	for (unsigned char i = 0; i < nrProgramari; i++)
	{
		fscanf(f, "%s", buffer);
		p.cod = strdup(buffer);
		fscanf(f, "%hhu %hhu %hu", &p.data.zi, &p.data.luna, &p.data.an);
		fscanf(f, " %[^\n]", buffer);
		p.nume_solicitant = strdup(buffer);
		fscanf(f, " %[^\n]", buffer);
		p.numeDoctor = strdup(buffer);
		fscanf(f, " %[^\n]", buffer);
		p.unitate = strdup(buffer);
		fscanf(f, "%f %hhu", &p.pret, &p.platit);
		inserareLista(cap, coada, p);
	}
	fclose(f);
}

void traversareCapCoada(nodLS* cap)
{
	nodLS* temp = cap;
	while (temp)
	{
		printf("Cod: %s\nData: %hhu/%hhu/%hu\nNume solicitant: %s\nNume doctor: %s\nUnitate: %s\nPret: %.2f RON\nAchitat: %s\n\n", temp->info.cod, temp->info.data.zi, temp->info.data.luna, temp->info.data.an, temp->info.nume_solicitant, temp->info.numeDoctor, temp->info.unitate, temp->info.pret, temp->info.platit ? "Da" : "Nu");
		temp = temp->next;
	}
}

void traversareCoadaCap(nodLS* coada)
{
	nodLS* temp = coada;
	while (temp)
	{
		printf("Cod: %s\nData: %hhu/%hhu/%hu\nNume solicitant: %s\nNume doctor: %s\nUnitate: %s\nPret: %.2f RON\nAchitat: %s\n\n", temp->info.cod, temp->info.data.zi, temp->info.data.luna, temp->info.data.an, temp->info.nume_solicitant, temp->info.numeDoctor, temp->info.unitate, temp->info.pret, temp->info.platit ? "Da" : "Nu");
		temp = temp->prev;
	}
}

Programare DeepCopy(Programare p)
{
	Programare noua;
	noua = p;
	noua.cod = strdup(p.cod);
	noua.numeDoctor = strdup(p.numeDoctor);
	noua.nume_solicitant = strdup(p.nume_solicitant);
	noua.unitate = strdup(p.unitate);
	return noua;
}

void adaugareProgramariDinLuna(const unsigned char luna, nodLS* capLista, Vector* v)
{
	v->vect = NULL;
	v->nrElem = 0;
	nodLS* temp = capLista;
	while (temp)
	{
		if (temp->info.data.luna == luna) {
			v->vect = (Programare*)realloc(v->vect, sizeof(Programare) * (++v->nrElem));
			v->vect[v->nrElem - 1] = DeepCopy(temp->info);
		}
		temp = temp->next;
	}
}

void traversareVector(Vector v)
{
	for (unsigned char i = 0; i < v.nrElem; i++)
		printf("Cod: %s\nData: %hhu/%hhu/%hu\nNume solicitant: %s\nNume doctor: %s\nUnitate: %s\nPret: %.2f RON\nAchitat: %s\n\n", v.vect[i].cod, v.vect[i].data.zi, v.vect[i].data.luna, v.vect[i].data.an, v.vect[i].nume_solicitant, v.vect[i].numeDoctor, v.vect[i].unitate, v.vect[i].pret, v.vect[i].platit ? "Da" : "Nu");
}

nodLS* DeepCopyHashT(Programare p)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = p;
	nou->info.cod = strdup(p.cod);
	nou->info.numeDoctor = strdup(p.numeDoctor);
	nou->info.nume_solicitant = strdup(p.nume_solicitant);
	nou->info.unitate = strdup(p.unitate);
	nou->next = NULL;
	nou->prev = NULL;
	return nou;
}

void Lista2HashT(hashT* tabela, nodLS* cap)
{
	tabela->nrElem = DIM;
	tabela->vect = (nodLS**)calloc(tabela->nrElem, sizeof(nodLS*));
	nodLS* temp = cap;
	while (temp)
	{
		inserareListaHashT(&tabela->vect[temp->info.data.luna - 1], DeepCopy(temp->info));
		temp = temp->next;
	}
}

void traversareHashT(hashT tabela)
{
	for (unsigned char i = 0; i < tabela.nrElem; i++)
		if (tabela.vect[i])
			printf("\nLuna %hhu\n\n", i+1), traversareCapCoada(tabela.vect[i]);
}

void maximLunar(nodLS* cap, unsigned char* lunaMax, unsigned char* maxProgramari) // de ce din Lista??? total ineficient! De ce am mai facut hashT?
{
	*lunaMax = 1;
	*maxProgramari = 0;
	for (unsigned char i = 1; i < 13; i++)
	{
		unsigned char nrProgramari = 0;
		nodLS* temp = cap;
		while (temp)
		{
			if (temp->info.data.luna == i)
				nrProgramari++;
			temp = temp->next;
		}
		if (nrProgramari > *maxProgramari)
			*lunaMax = i, * maxProgramari = nrProgramari;
	}
}

void dezalocareLS(nodLS** cap)
{
	while (*cap)
	{
		nodLS* aux = *cap;
		*cap = (*cap)->next;
		free(aux->info.cod);
		free(aux->info.numeDoctor);
		free(aux->info.nume_solicitant);
		free(aux->info.unitate);
		free(aux);
	}
}

void dezalocareHashT(hashT* tabela)
{
	for (unsigned char i = 0; i < tabela->nrElem; i++)
		if (tabela->vect[i])
			dezalocareLS(&tabela->vect[i]);
	free(tabela->vect);
	tabela->nrElem = 0;
}

void dezalocareVector(Vector* v)
{
	for (unsigned char i = 0; i < v->nrElem; i++)
	{
		free(v->vect[i].cod);
		free(v->vect[i].numeDoctor);
		free(v->vect[i].nume_solicitant);
		free(v->vect[i].unitate);
	}
	free(v->vect);
	v->nrElem = 0;
}

void main()
{
	nodLS* capListaDubla;
	nodLS* coadaListaDubla;
	Vector v;
	hashT tabela;
	const char* numeFisier = "date.txt";
	const unsigned char luna = 7;
	citireFisier(&capListaDubla, &coadaListaDubla, numeFisier);
	printf("Afisare cap-coada:\n");
	traversareCapCoada(capListaDubla);
	printf("\n\nAfisare coada-cap:\n");
	traversareCoadaCap(coadaListaDubla);
	adaugareProgramariDinLuna(luna, capListaDubla, &v);
	if (!v.nrElem)
	{
		printf("\nNu exista programari in luna %hhu\n", luna);
	}
	else
	{
		printf("\nProgramarile din luna %hhu\n\n", luna);
		traversareVector(v);
	}
	printf("\n\nAfisare hashT:\n\n");
	Lista2HashT(&tabela, capListaDubla);
	traversareHashT(tabela);
	unsigned char lunaMaxima, nrProgramari;
	maximLunar(capListaDubla, &lunaMaxima, &nrProgramari);
	printf("\n\nMaximul programarilor a fost atins in luna %hhu, acesta fiind de %hhu.\n\n", lunaMaxima, nrProgramari);
	dezalocareHashT(&tabela);
	dezalocareVector(&v);
	dezalocareLS(&capListaDubla);
	_CrtDumpMemoryLeaks();
}
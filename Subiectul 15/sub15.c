#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#include <stdbool.h>
#pragma warning(disable:4996)
#define LINE_SIZE 128
#define _CRTDBG_MAP_ALLOC

typedef struct InfoLocalitate Localitate;
typedef struct ListaDubla nodLS;
typedef struct Vector Vector;
typedef enum TipLocalitate Tip;

enum TipLocalitate
{
	sat,
	comuna,
	oras,
	municipiu
};

struct InfoLocalitate
{
	char* denumire;
	char* judet;
	unsigned int populatie;
	Tip tip;
	float buget;
};

struct ListaDubla
{
	Localitate info;
	nodLS* next, * prev;
};

struct Vector
{
	Localitate* vect;
	unsigned char nrElem;
};

nodLS* creareNodLS(Localitate l)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = l;
	nou->next = NULL;
	nou->prev = NULL;
	return nou;
}

void inserareListaDubla(nodLS** cap, nodLS** coada, Localitate l)
{
	if (*cap)
	{
		nodLS* temp = *cap;
		nodLS* nou = creareNodLS(l);
		while (temp->next)
		{
			temp = temp->next;
		}
		nou->prev = temp;
		temp->next = nou;
		*coada = nou;
	}
	else { 
		*cap = creareNodLS(l);
		*coada = *cap;
	}
}

void traversareListaDublaDeLaCap(nodLS* cap)
{
	nodLS* temp = cap;
	while (temp)
	{
		printf("Denumire: %s\nJudet: %s\nPopulatie: %u locuitori\nTip: %s\nBuget: %.2f RON\n\n", temp->info.denumire, temp->info.judet, temp->info.populatie, temp->info.tip == 0 ? "sat" : temp->info.tip == 1 ? "comuna" : temp->info.tip == 2 ? "oras" : "municipiu", temp->info.buget);
		temp = temp->next;
	}
}

void citireFisier(nodLS** cap, nodLS** coada, const unsigned char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	if (!f)
		exit(-1);
	*cap = NULL;
	char buffer[LINE_SIZE];
	Localitate l;
	unsigned char nrLocalitati;
	fscanf(f, "%hhu", &nrLocalitati);
	for (unsigned char i = 0; i < nrLocalitati; i++)
	{
		fscanf(f, " %[^\n]", buffer);
		l.denumire = strdup(buffer);
		fscanf(f, " %[^\n]", buffer);
		l.judet = strdup(buffer);
		fscanf(f, "%u", &l.populatie);
		fscanf(f, "%s", buffer);
		if (!strcmp(buffer, "sat"))
			l.tip = sat;
		else if (!strcmp(buffer, "comuna"))
			l.tip = comuna;
		else if (!strcmp(buffer, "oras"))
			l.tip = oras;
		else l.tip = municipiu;
		fscanf(f, "%f", &l.buget);
		inserareListaDubla(cap, coada, l);
	}
}

unsigned int populatieTotalaDin(Tip tip, nodLS* cap, bool* flag)
{
	unsigned int total = 0;
	*flag = false;
	nodLS* temp = cap;
	while (temp)
	{
		if (temp->info.tip == tip)
			total += temp->info.populatie, *flag = true;
		temp = temp->next;
	}
	return total;
}

nodLS* getCoadaListaDubla(nodLS* cap)
{
	nodLS* temp = cap;
	while (temp->next)
	{
		temp = temp->next;
	}
	return temp;
}

void traversareListaDublaDeLaCoada(nodLS* coada)
{
	nodLS* temp = coada;
	while (temp)
	{
		printf("Denumire: %s\nJudet: %s\nPopulatie: %u locuitori\nTip: %s\nBuget: %.2f RON\n\n", temp->info.denumire, temp->info.judet, temp->info.populatie, temp->info.tip == 0 ? "sat" : temp->info.tip == 1 ? "comuna" : temp->info.tip == 2 ? "oras" : "municipiu", temp->info.buget);
		temp = temp->prev;
	}
}

void modificaPopulatieInJudetul(const char* judet, nodLS* cap, const float procent, bool* flag)
{
	nodLS* temp = cap;
	*flag = false;
	while (temp)
	{
		if (!strcmp(temp->info.judet, judet))
			temp->info.populatie *= (100.0f + procent), temp->info.populatie/=100, *flag = true;
		temp = temp->next;
	}
}

float medieLista(nodLS* cap)
{
	float rez = 0.0f;
	unsigned char nrElem = 0;
	nodLS* temp = cap;
	while (temp)
	{
		rez += temp->info.populatie;
		nrElem++;
		temp = temp->next;
	}
	return rez / nrElem;
}

void nrElem(nodLS* cap, unsigned char* nrElem, const float medie)
{
	nodLS* aux = cap;
	while (aux)
	{
		if (aux->info.populatie > medie)
			(*nrElem)++;
		aux = aux->next;
	}
}

void stergeOraseSubMedie(nodLS** cap, nodLS** coada)
{
	const float medie = medieLista(*cap);
	nodLS* temp = *cap;
	while (temp)
		if (temp->info.populatie < medie)
		{
			if (temp == *cap)
			{
				temp->next->prev = NULL;
				*cap = temp->next;
				free(temp->info.denumire);
				free(temp->info.judet);
				free(temp);
				temp = *cap;
			}
			else if (temp->prev != NULL && temp->next != NULL)
			{
				nodLS* aux = temp->next;
				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
				free(temp->info.denumire);
				free(temp->info.judet);
				free(temp);
				temp = aux;
			}
			else
			{
				temp->prev->next = NULL;
				*coada = temp->prev;
				free(temp->info.denumire);
				free(temp->info.judet);
				free(temp);
				temp = NULL;
			}
		}
		else temp = temp->next;
}

Localitate DeepCopy(Localitate l)
{
	Localitate noua = l;
	noua.denumire = strdup(l.denumire);
	noua.judet = strdup(l.judet);
	return noua;
}

void populareVector(nodLS* cap, Vector v, unsigned char* auxInt, const float medie)
{
	nodLS* temp = cap;
	while (temp)
	{
		if (temp->info.populatie > medie)
			v.vect[v.nrElem - (*auxInt)--] = DeepCopy(temp->info);
		temp = temp->next;
	}
}

void orasePesteMedie(nodLS* cap, Vector* v)
{
	v->nrElem = 0;
	nrElem(cap, &v->nrElem, medieLista(cap));
	v->vect = (Localitate*)malloc(sizeof(Localitate) * v->nrElem);
	unsigned char auxInt = v->nrElem;
	populareVector(cap, *v, &auxInt, medieLista(cap));
}

void traversareVector(Vector v)
{
	for (unsigned char i = 0; i < v.nrElem; i++)
		printf("Denumire: %s\nJudet: %s\nPopulatie: %u locuitori\nTip: %s\nBuget: %.2f RON\n\n", v.vect[i].denumire, v.vect[i].judet, v.vect[i].populatie, v.vect[i].tip == 0 ? "sat" : v.vect[i].tip == 1 ? "comuna" : v.vect[i].tip == 2 ? "oras" : "municipiu", v.vect[i].buget);
}

void dezalocareLista(nodLS** cap, nodLS** coada)
{
	while (*cap)
	{
		nodLS* aux = *cap;
		*cap = (*cap)->next;
		if ((*cap) == NULL)
			*coada = NULL;
		free(aux->info.denumire);
		free(aux->info.judet);
		free(aux);
	}
}

void dezalocareVector(Vector* v)
{
	for (unsigned char i = 0; i < v->nrElem; i++)
	{
		free(v->vect[i].denumire);
		free(v->vect[i].judet);
	}
	free(v->vect);
	v->nrElem = 0;
}

void main()
{
	nodLS* cap, *coada;
	Vector v;
	bool flag;
	const unsigned char* numeFisier = "date.txt";
	const char* judet = "Cluj";
	const float procent = 3.14f;
	const Tip tip = municipiu;
	citireFisier(&cap, &coada, numeFisier);
	traversareListaDublaDeLaCap(cap);
	unsigned int totalPopulatie = populatieTotalaDin(tip, cap, &flag);
	if (!flag)
	{
		printf("\nLocatia %hhu nu exista in lista!\n", tip);
	}
	else
	{
		printf("\nNumarul total de oameni de la %s este: %u\n", tip == 0 ? "sat" : tip == 1 ? "comuna" : tip == 2 ? "oras" : "municipiu", totalPopulatie);
	}
	modificaPopulatieInJudetul(judet, cap, procent, &flag);
	if (!flag)
	{
		printf("\nNu exista judetul %s in lista!\n", judet);
	}
	else
	{
		printf("\nLista parcursa in ambele sensuri pentru a demonstra modificarile de populatie din orasele din judetul %s\n", judet);
		traversareListaDublaDeLaCap(cap);
		printf("\n\n------------------------------------------------\n\n");
		traversareListaDublaDeLaCoada(coada);
	}
	printf("\n\n------------------------------------------------\n\n");
	stergeOraseSubMedie(&cap, &coada);
	traversareListaDublaDeLaCap(cap);
	printf("\n\n------------------------------------------------\n\n");
	traversareListaDublaDeLaCoada(coada);
	printf("\n\n------------------------------------------------\n\n");
	orasePesteMedie(cap, &v);
	traversareVector(v);
	dezalocareLista(&cap, &coada);
	dezalocareVector(&v);
	_CrtDumpMemoryLeaks();
}
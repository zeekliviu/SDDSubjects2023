#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#include <stdbool.h>
#pragma warning(disable:4996)
#define _CRTDBG_MAP_ALLOC
#define LINE_SIZE 128

typedef struct InfoAngajat Angajat;
typedef struct ListaSimpla nodLS;
typedef struct ArboreBinar nodArb;
typedef struct Vector Vector;

struct InfoAngajat
{
	unsigned char marca;
	char* nume;
	float salariu;
	char* departament;
	bool sex;
};

struct ListaSimpla
{
	Angajat info;
	nodLS* next, *prev;
};

struct ArboreBinar
{
	Angajat info;
	nodArb* st, * dr;
};

struct Vector
{
	Angajat* vect;
	unsigned char nrElem;
};

nodLS* creareNodLista(Angajat a)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = a;
	nou->next = NULL;
	nou->prev = NULL;
	return nou;
}

void inserareListaDubla(nodLS** cap, Angajat a)
{
	if (*cap)
	{
		nodLS* temp = *cap;
		while (temp->next)
			temp = temp->next;
		nodLS* nou = creareNodLista(a);
		nou->prev = temp;
		temp->next = nou;
	}
	else *cap = creareNodLista(a);
}

void citireFisier(nodLS** cap, const unsigned char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	unsigned char nrAng;
	char buffer[LINE_SIZE];
	Angajat a;
	fscanf(f, "%hhu", &nrAng);
	for (unsigned char i = 0; i < nrAng; i++)
	{
		fscanf(f, "%hhu", &a.marca);
		fscanf(f, " %[^\n]", buffer);
		a.nume = strdup(buffer);
		fscanf(f, "%f", &a.salariu);
		fscanf(f, " %[^\n]", buffer);
		a.departament = strdup(buffer);
		fscanf(f, "%hhu", &a.sex);
		inserareListaDubla(cap, a);
	}
	fclose(f);
}

void traversareCapCoada(nodLS* cap)
{
	nodLS* temp = cap;
	while (temp)
	{
		printf("\nMarca: %hhu\nNume: %s\nDepartament: %s\nSalariu: %.2f\nSex: %s\n\n", temp->info.marca, temp->info.nume, temp->info.departament, temp->info.salariu, temp->info.sex ? "Masculin" : "Feminin");
		temp = temp->next;
	}
}

void traversareCoadaCap(nodLS* coada)
{
	nodLS* temp = coada;
	while (temp)
	{
		printf("\nMarca: %hhu\nNume: %s\nDepartament: %s\nSalariu: %.2f\nSex: %s\n\n", temp->info.marca, temp->info.nume, temp->info.departament, temp->info.salariu, temp->info.sex ? "Masculin" : "Feminin");
		temp = temp->prev;
	}
}

nodLS* getCoada(nodLS* cap)
{
	nodLS* temp = cap;
	while (temp->next)
		temp = temp->next;
	return temp;
}

void stergeDinLista(nodLS** cap, const float pragSalariu)
{
	nodLS* temp = *cap;
	while(temp)
		if (temp->info.salariu > pragSalariu)
		{
			nodLS* aux;
			if (temp == *cap)
			{
				*cap = temp->next;
				temp->next->prev = NULL;
				aux = *cap;
				free(temp->info.departament);
				free(temp->info.nume);
				free(temp);
			}
			else if(temp->next!=NULL)
			{
				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
				aux = temp->next;
				free(temp->info.departament);
				free(temp->info.nume);
				free(temp);
			}
			else {
				temp->prev->next = NULL;
				aux = NULL;
				free(temp->info.departament);
				free(temp->info.nume);
				free(temp);
			}
			temp = aux;
		}
		else
		{
			temp = temp->next;
		}
}

nodArb* creareNodArbore(Angajat a, nodArb* st, nodArb* dr)
{
	nodArb* nou = (nodArb*)malloc(sizeof(nodArb));
	nou->dr = dr;
	nou->st = st;
	nou->info = a;
	nou->info.departament = strdup(a.departament);
	nou->info.nume = strdup(a.nume);
	return nou;
}

void inserareArb(nodArb** rad, Angajat a)
{
	if (*rad)
	{
		if (a.salariu > (*rad)->info.salariu)
			inserareArb(&(*rad)->dr, a);
		else if (a.salariu < (*rad)->info.salariu)
			inserareArb(&(*rad)->st, a);
	}
	else *rad = creareNodArbore(a, NULL, NULL);
}

void inordine(nodArb* rad)
{
	if (rad)
	{
		inordine(rad->st);
		printf("\nMarca: %hhu\nNume: %s\nDepartament: %s\nSalariu: %.2f\nSex: %s\n\n", rad->info.marca, rad->info.nume, rad->info.departament, rad->info.salariu, rad->info.sex ? "Masculin" : "Feminin");
		inordine(rad->dr);
	}
}

void List2Arb(nodLS* cap, nodArb** rad)
{
	nodLS* temp = cap;
	while (temp)
	{
		inserareArb(rad, temp->info);
		temp = temp->next;
	}
}

void nrNoduri(nodArb* rad, unsigned char* nr, const float salariu)
{
	if (rad)
	{
		nrNoduri(rad->st, nr, salariu);
		if (rad->info.salariu < salariu)
			(*nr)++;
		nrNoduri(rad->dr, nr, salariu);
	}
}

Angajat DeepCopy(Angajat a)
{
	Angajat nou = a;
	nou.departament = strdup(a.departament);
	nou.nume = strdup(a.nume);
	return nou;
}

void populareVector(nodArb* rad, Vector v, unsigned char* aux, const float salariu)
{
	if (rad)
	{
		populareVector(rad->st, v, aux, salariu);
		if (rad->info.salariu < salariu)
			v.vect[v.nrElem - (*aux)--] = DeepCopy(rad->info);
		populareVector(rad->dr, v, aux, salariu);
	}
}

void Arb2Vector(nodArb* rad, Vector* v, const float salariuPrag)
{
	v->nrElem = 0;
	nrNoduri(rad, &v->nrElem, salariuPrag);
	v->vect = (Angajat*)malloc(sizeof(Angajat) * v->nrElem);
	unsigned char aux = v->nrElem;
	populareVector(rad, *v, &aux, salariuPrag);
}

void traversareVector(Vector v)
{
	for (unsigned char i = 0; i < v.nrElem; i++)
		printf("\nMarca: %hhu\nNume: %s\nDepartament: %s\nSalariu: %.2f\nSex: %s\n\n", v.vect[i].marca, v.vect[i].nume, v.vect[i].departament, v.vect[i].salariu, v.vect[i].sex ? "Masculin" : "Feminin");
}

void dezalocareLista(nodLS** cap)
{
	while (*cap)
	{
		nodLS* aux = *cap;
		*cap = (*cap)->next;
		free(aux->info.departament);
		free(aux->info.nume);
		free(aux);
	}
}

void dezalocareArb(nodArb** rad)
{
	if (*rad)
	{
		dezalocareArb(&(*rad)->st);
		dezalocareArb(&(*rad)->dr);
		free((*rad)->info.departament);
		free((*rad)->info.nume);
		free(*rad);
	}
}

void dezalocareVector(Vector* v)
{
	for (unsigned char i = 0; i < v->nrElem; i++)
	{
		free(v->vect[i].departament);
		free(v->vect[i].nume);
	}
	free(v->vect);
	v->nrElem = 0;
}

void main()
{
	nodLS* cap = NULL;
	nodLS* coada;
	nodArb* rad = NULL;
	Vector v;
	const unsigned char numeFisier[] = "date.txt";
	const float pragSalariu = 2500.0f;
	const float pragSalariuNou = 2300.0f;
	citireFisier(&cap, numeFisier);
	traversareCapCoada(cap);
	coada = getCoada(cap);
	printf("\n\nTraversare coada-cap\n\n");
	traversareCoadaCap(coada);
	printf("\n\nStergere angajati cu salariul peste 2500:\n\n");
	stergeDinLista(&cap, pragSalariu);
	traversareCapCoada(cap);
	coada = getCoada(cap);
	printf("\n\n\n\n\n");
	traversareCoadaCap(coada);
	printf("\n\n\n\nParcurgere inordine arbore:\n\n");
	List2Arb(cap, &rad);
	inordine(rad);
	printf("\n\n\n\n\n");
	Arb2Vector(rad, &v, pragSalariuNou);
	traversareVector(v);
	dezalocareArb(&rad);
	dezalocareLista(&cap);
	dezalocareVector(&v);
	_CrtDumpMemoryLeaks();
}
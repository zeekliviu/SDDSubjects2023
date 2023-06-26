#include <stdio.h>
#include <malloc.h>
#include <crtdbg.h>
#include <string.h>
#include <stdbool.h>
#pragma warning(disable:4996)
#define LINE_SIZE 128
#define _CRTDBG_MAP_ALLOC

typedef struct ListaSimpla nodLS;
typedef struct ListaListe nodLP;
typedef struct Coada Coada;
typedef struct Vector Vector;
typedef struct Angajat Angajat;

struct Angajat
{
	unsigned int cod_departament;
	unsigned char marca;
	float salariu;
	char* nume;
	unsigned char nrOreLucrateSaptamanal;
	bool sex;
};

struct ListaSimpla
{
	Angajat info;
	nodLS* next;
};

struct ListaListe
{
	nodLS* info;
	nodLP* next;
};

struct Coada
{
	Angajat info;
	Coada* next;
};

struct Vector
{
	Angajat* vect;
	unsigned char nrElem;
};

bool cautaDepartament(nodLP* cap, unsigned char dep)
{
	nodLP* index = cap;
	while (index)
	{
		if (index->info->info.cod_departament == dep)
			return true;
		index = index->next;
	}
	return false;
}

nodLS* creareNodListaSimpla(Angajat a)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = a;
	nou->next = NULL;
	return nou;
}

void inserareListaListe(nodLP** cap, Angajat a)
{
	if (!cautaDepartament(*cap, a.cod_departament))
	{
		nodLP* nou = (nodLP*)malloc(sizeof(nodLP));
		nou->info = creareNodListaSimpla(a);
		nou->next = *cap;
		*cap = nou;
	}
	else
	{
		nodLS* nou = creareNodListaSimpla(a);
		nodLP* aux = *cap;
		while (aux)
		{
			if (aux->info->info.cod_departament == a.cod_departament)
			{
				nou->next = aux->info;
				aux->info = nou;
			}
			aux = aux->next;
		}
	}
}

void citireFisier(nodLP** listaListe, const char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	unsigned char nrAngajati;
	fscanf(f, "%hhu", &nrAngajati);
	char buffer[LINE_SIZE];
	Angajat a;
	for (unsigned char i = 0; i < nrAngajati; i++)
	{
		fscanf(f, "%u", &a.cod_departament);
		fscanf(f, "%hhu", &a.marca);
		fscanf(f, "%f", &a.salariu);
		fscanf(f, " %[^\n]", buffer);
		a.nume = strdup(buffer);
		fscanf(f, "%hhu", &a.nrOreLucrateSaptamanal);
		fscanf(f, "%hhu", &a.sex);
		inserareListaListe(listaListe, a);
	}
}

void traversareLS(nodLS* cap)
{
	nodLS* index = cap;
	while (index)
	{
		printf("\nCod departament: %u Marca: %hhu Nume: %s Salariu: %.2f Nr. ore lucrare saptamanal: %hhu Sex: %s", index->info.cod_departament, index->info.marca, index->info.nume, index->info.salariu, index->info.nrOreLucrateSaptamanal, index->info.sex ? "Barbat" : "Femeie");
		index = index->next;
	}
}

void traversareLP(nodLP* cap)
{
	nodLP* index = cap;
	unsigned char i = 1;
	while (index)
	{
		printf("\n\nSublista %hhu\n\n", i++);
		traversareLS(index->info);
		index = index->next;
	}
}

Angajat DeepCopy(Angajat a)
{
	Angajat nou;
	nou.cod_departament = a.cod_departament;
	nou.marca = a.marca;
	nou.nrOreLucrateSaptamanal = a.nrOreLucrateSaptamanal;
	nou.salariu = a.salariu;
	nou.sex = a.sex;
	nou.nume = strdup(a.nume);
	return nou;
}

void ListaListe2Vector(nodLP* listaListe, Vector* v, float pragSalariu)
{
	v->nrElem = 0;
	nodLP* aux = listaListe;
	while (aux)
	{
		nodLS* aux2 = aux->info;
		while (aux2)
		{
			if (aux2->info.salariu > pragSalariu)
				v->nrElem++;
			aux2 = aux2->next;
		}
		aux = aux->next;
	}
	v->vect = (Angajat*)malloc(sizeof(Angajat) * v->nrElem);
	unsigned char auxInteger = v->nrElem;
	aux = listaListe;
	while (aux)
	{
		nodLS* aux2 = aux->info;
		while (aux2)
		{
			if (aux2->info.salariu > pragSalariu)
				v->vect[v->nrElem - auxInteger] = DeepCopy(aux2->info), auxInteger--;
			aux2 = aux2->next;
		}
		aux = aux->next;
	}
}

void traversareVector(Vector v)
{
	for (unsigned char i = 0; i < v.nrElem; i++)
	{
		printf("\nSalariu: %.2f Cod departament: %u Nume: %s Sex: %s, Nr. ore lucrate saptamanal: %hhu Marca: %hhu", v.vect[i].salariu, v.vect[i].cod_departament, v.vect[i].nume, v.vect[i].sex ? "Barbat" : "Femeie", v.vect[i].nrOreLucrateSaptamanal, v.vect[i].marca);
	}
}

Coada* creareNodCoada(Angajat a)
{
	Coada* nou = (Coada*)malloc(sizeof(Coada));
	nou->info.cod_departament = a.cod_departament;
	nou->info.marca = a.marca;
	nou->info.nrOreLucrateSaptamanal = a.nrOreLucrateSaptamanal;
	nou->info.salariu = a.salariu;
	nou->info.sex = a.sex;
	nou->info.nume = strdup(a.nume);
	nou->next = NULL;
	return nou;
}

void put(Coada** baza, Coada** varf, Angajat a)
{
	Coada* nou = (Coada*)malloc(sizeof(Coada));
	nou->info.cod_departament = a.cod_departament;
	nou->info.marca = a.marca;
	nou->info.nrOreLucrateSaptamanal = a.nrOreLucrateSaptamanal;
	nou->info.salariu = a.salariu;
	nou->info.sex = a.sex;
	nou->info.nume = strdup(a.nume);
	nou->next = NULL;

	if (*varf != NULL)
	{
		(*varf)->next = nou;
	}

	*varf = nou;

	if (*baza == NULL)
	{
		*baza = nou;
	}
}

void ListaListe2Coada(nodLP* listaListe, Coada** baza, Coada** varf, unsigned int dep)
{
	nodLP* aux = listaListe;
	while (aux)
	{
		nodLS* aux2 = aux->info;
		if (aux2->info.cod_departament == dep)
		{
			while (aux2)
			{
				put(baza, varf, aux2->info);
				aux2 = aux2->next;
			}
			break;
		}
		aux = aux->next;
	}
}

Angajat* get(Coada** baza, Coada** varf)
{
	if (*baza != NULL && *varf != NULL)
	{
		Angajat* a = (Angajat*)malloc(sizeof(Angajat));
		a->cod_departament = (*baza)->info.cod_departament;
		a->marca = (*baza)->info.marca;
		a->nrOreLucrateSaptamanal = (*baza)->info.nrOreLucrateSaptamanal;
		a->salariu = (*baza)->info.salariu;
		a->sex = (*baza)->info.sex;
		a->nume = strdup((*baza)->info.nume);
		Coada* temp = *baza;
		*baza = (*baza)->next;
		free(temp->info.nume);
		free(temp);
		return a;
	}
	if (*baza == NULL)
	{
		*varf = NULL;
		return NULL;
	}
}


void traversareCoadaFaraExtragere(Coada* varf)
{
	Coada* index = varf;
	while (index)
	{
		printf("\nCod departament: %u Marca: %hhu Nume: %s Salariu: %.2f Nr. ore lucrare saptamanal: %hhu Sex: %s", index->info.cod_departament, index->info.marca, index->info.nume, index->info.salariu, index->info.nrOreLucrateSaptamanal, index->info.sex ? "Barbat" : "Femeie");
		index = index->next;
	}
}

void minim3Salarii(Coada* varf, Vector* v)
{
	v->nrElem = 0;
	float maxim = 0.0f;
	Coada* index = varf;
	while (index)
	{
		if (index->info.salariu > maxim)
			maxim = index->info.salariu;
		v->nrElem++;
		index = index->next;
	}
	if (v->nrElem < 3)
	{
		v->vect = NULL;
		v->nrElem = -1;
		return;
	}
	v->nrElem = 3;
	v->vect = (Angajat*)malloc(sizeof(Angajat) * v->nrElem);
	float salMin1, salMin2, salMin3;
	salMin1 = salMin2 = salMin3 = maxim;
	char* nume1 = NULL, * nume2 = NULL, * nume3 = NULL;

	index = varf;
	while (index)
	{
		if (index->info.salariu <= salMin1 && (nume1 == NULL || strcmp(index->info.nume, nume1) != 0))
		{
			salMin3 = salMin2;
			nume3 = nume2;
			salMin2 = salMin1;
			nume2 = nume1;
			salMin1 = index->info.salariu;
			nume1 = index->info.nume;
		}
		else if (index->info.salariu <= salMin2 && index->info.salariu > salMin1
			&& (nume2 == NULL || strcmp(index->info.nume, nume2) != 0))
		{
			salMin3 = salMin2;
			nume3 = nume2;
			salMin2 = index->info.salariu;
			nume2 = index->info.nume;
		}
		else if (index->info.salariu <= salMin3 && index->info.salariu > salMin2
			&& (nume3 == NULL || strcmp(index->info.nume, nume3) != 0))
		{
			salMin3 = index->info.salariu;
			nume3 = index->info.nume;
		}
		index = index->next;
	}

	index = varf;
	while (index)
	{
		if (!strcmp(index->info.nume, nume1))
			v->vect[0] = DeepCopy(index->info);
		else if (!strcmp(index->info.nume, nume2))
			v->vect[1] = DeepCopy(index->info);
		else if (!strcmp(index->info.nume, nume3))
			v->vect[2] = DeepCopy(index->info);
		index = index->next;
	}
}

void dezalocareVector(Vector* v)
{
	for (unsigned int i = 0; i < v->nrElem; i++)
		free(v->vect[i].nume);
	free(v->vect);
}

void dezalocareLS(nodLS** cap)
{
	while (*cap)
	{
		nodLS* aux = *cap;
		(*cap) = (*cap)->next;
		free(aux->info.nume);
		free(aux);
	}
}

void dezalocareLP(nodLP** cap)
{
	while (*cap)
	{
		nodLP* aux = *cap;
		(*cap) = (*cap)->next;
		dezalocareLS(&aux->info);
		free(aux);
	}
}

void dezalocareCoada(Coada** varf, Coada** baza)
{
	Angajat* a;
	while (a = get(varf, baza))
	{
		free(a->nume);
		free(a);
	}
}

void main()
{
	nodLP* listaListe = NULL;
	Coada* coada = NULL;
	Coada* varf = NULL;
	Vector v;
	Vector w;
	float pragSalarial = 3000.0f;
	unsigned int departament = 10;
	citireFisier(&listaListe, "date.txt");
	traversareLP(listaListe);
	printf("\n\nSalariatii peste pragul salarial de %.2f RON.", pragSalarial);
	ListaListe2Vector(listaListe, &v, pragSalarial);
	traversareVector(v);
	printf("\n\nAngajatii din departamentul %u sunt:\n", departament);
	ListaListe2Coada(listaListe, &varf, &coada, departament);
	traversareCoadaFaraExtragere(varf);
	printf("\n\nAngajatii cu cele mai mici 3 salarii din coada:\n");
	minim3Salarii(varf, &w);
	traversareVector(w);
	dezalocareVector(&v);
	dezalocareVector(&w);
	dezalocareLP(&listaListe);
	dezalocareCoada(&varf, &coada);
	_CrtDumpMemoryLeaks();
}
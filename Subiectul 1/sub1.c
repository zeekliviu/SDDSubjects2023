#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
#pragma warning(disable:4996)

typedef struct Reteta Reteta;
typedef struct nodLS nodLS;
typedef struct Vector Vector;

struct Reteta
{
	unsigned int nrReteta;
	unsigned char nrMedicamente;
	char** listaMedicamente;
	unsigned char* cantitatiMedicamente;
	float* preturiMedicamente;
	unsigned char* procenteCompensare;
	char* numeMedic;
};

struct nodLS
{
	Reteta* info;
	nodLS* next, *prev;
};

struct Vector
{
	Reteta** info;
	int nrElem;
};

Reteta* creareRetetaNoua(Reteta* r)
{
	Reteta* noua = (Reteta*)malloc(sizeof(Reteta));
	noua->nrReteta = r->nrReteta;
	noua->nrMedicamente = r->nrMedicamente;
	noua->numeMedic = strdup(r->numeMedic);
	noua->listaMedicamente = (char**)malloc(r->nrMedicamente * sizeof(char*));
	noua->procenteCompensare = (unsigned char*)malloc(r->nrMedicamente * sizeof(unsigned char));
	noua->preturiMedicamente = (float*)malloc(r->nrMedicamente * sizeof(float));
	noua->cantitatiMedicamente = (unsigned char*)malloc(r->nrMedicamente * sizeof(unsigned char));
	for (unsigned char i = 0; i < r->nrMedicamente; i++)
	{
		noua->listaMedicamente[i] = strdup(r->listaMedicamente[i]);
		noua->procenteCompensare[i] = r->procenteCompensare[i];
		noua->preturiMedicamente[i] = r->preturiMedicamente[i];
		noua->cantitatiMedicamente[i] = r->cantitatiMedicamente[i];
	}
	return noua;
}

Reteta* creareReteta(unsigned int nrReteta, unsigned char nrMedicamente, char** listaMedicamente, unsigned char* cantitatiMedicamente, float* preturiMedicamente, unsigned char* procenteCompensare, char* numeMedic)
{
	Reteta* r = (Reteta*)malloc(sizeof(Reteta));
	r->nrReteta = nrReteta;
	r->nrMedicamente = nrMedicamente;
	r->listaMedicamente = listaMedicamente;
	r->cantitatiMedicamente = cantitatiMedicamente;
	r->preturiMedicamente = preturiMedicamente;
	r->procenteCompensare = procenteCompensare;
	r->numeMedic = numeMedic;
	return r;
}

nodLS* creareNod(Reteta* r)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = r;
	nou->next = NULL;
	nou->prev = NULL;
	return nou;
}

void inserareLaInceput(nodLS** cap, nodLS** coada, Reteta* r)
{
	nodLS* nou = creareNod(r);
	nou->next = *cap;
	if (*cap)
	{
		(*cap)->prev = nou;
		if (*coada == NULL)
			*coada = *cap;
	}
	*cap = nou;
}

void citireElementeLista(nodLS** cap, nodLS** coada, const char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	unsigned char nrElem;
	fscanf(f, "%hhu", &nrElem);
	Reteta r;
	char buffer[256];
	for (unsigned char i = 0; i < nrElem; i++)
	{
		fscanf(f, "%u", &r.nrReteta);
		fscanf(f, "%hhu", &r.nrMedicamente);
		r.listaMedicamente = (char**)malloc(r.nrMedicamente * sizeof(char*));
		for (unsigned char j = 0; j < r.nrMedicamente; j++)
		{
			fscanf(f, " %[^\n]", buffer);
			r.listaMedicamente[j] = strdup(buffer);
		}
		r.preturiMedicamente = (float*)malloc(r.nrMedicamente * sizeof(float));
		r.procenteCompensare = (unsigned char*)malloc(r.nrMedicamente * sizeof(unsigned char));
		r.cantitatiMedicamente = (unsigned char*)malloc(r.nrMedicamente * sizeof(unsigned char));
		for (unsigned char j = 0; j < r.nrMedicamente; j++)
		{
			fscanf(f, "%hhu", &r.cantitatiMedicamente[j]);
		}
		for (unsigned char j = 0; j < r.nrMedicamente; j++)
		{
			fscanf(f, "%f", &r.preturiMedicamente[j]);
		}
		for (unsigned char j = 0; j < r.nrMedicamente; j++)
		{
			fscanf(f, "%hhu", &r.procenteCompensare[j]);
		}
		fscanf(f, " %[^\n]", buffer);
		r.numeMedic = strdup(buffer);
		Reteta* deInserat = creareReteta(r.nrReteta, r.nrMedicamente, r.listaMedicamente, r.cantitatiMedicamente, r.preturiMedicamente, r.procenteCompensare, r.numeMedic);
		inserareLaInceput(&*cap, &*coada, deInserat);
	}
	fclose(f);
}

void afisareLista(nodLS* cap)
{
	nodLS* temp = cap;
	while (temp)
	{
		printf("Numar reteta: %u\nEmisa de: %s\nNumar medicamente: %hhu\nLista medicamente:\n", temp->info->nrReteta, temp->info->numeMedic, temp->info->nrMedicamente);
		for (unsigned char i = 0; i < temp->info->nrMedicamente; i++)
		{
			printf("Medicamentul %s in cantitatea %hhu are pretul %.2f si o compensare de %hhu%%\n", temp->info->listaMedicamente[i], temp->info->cantitatiMedicamente[i], temp->info->preturiMedicamente[i], temp->info->procenteCompensare[i]);
		}
		printf("\n");
		temp = temp->next;
	}
}

void afisareListaReversed(nodLS* coada)
{
	nodLS* temp = coada;
	while (temp)
	{
		printf("Numar reteta: %u\nEmisa de: %s\nNumar medicamente: %hhu\nLista medicamente:\n", temp->info->nrReteta, temp->info->numeMedic, temp->info->nrMedicamente);
		for (unsigned char i = 0; i < temp->info->nrMedicamente; i++)
		{
			printf("Medicamentul %s in cantitatea %hhu are pretul %.2f si o compensare de %hhu%%\n", temp->info->listaMedicamente[i], temp->info->cantitatiMedicamente[i], temp->info->preturiMedicamente[i], temp->info->procenteCompensare[i]);
		}
		printf("\n");
		temp = temp->prev;
	}
}

void totalIncasari(nodLS* cap, const char* denumire)
{
	nodLS* temp = cap;
	float total = 0.0f;
	while (temp)
	{
		for (unsigned char i = 0; i < temp->info->nrMedicamente; i++)
		{
			if (strcmp(temp->info->listaMedicamente[i], denumire)==0)
				total += (float)temp->info->cantitatiMedicamente[i] * temp->info->preturiMedicamente[i] * (100 - (float)temp->info->procenteCompensare[i]);
		}
		temp = temp->next;
	}
	if (total == 0.0f)
		printf("\nNu exista medicamentul cu denumirea %s in lista de retete!", denumire);
	else
		printf("\nTotalul generat de prescrierea medicamentului %s in lista de retete este %.2f RON.", denumire, total);
}

void conversieListaLaVector(nodLS* cap, Vector* v, const char* denumire)
{
	nodLS* temp = cap;
	unsigned char nr=0;
	while (temp)
	{
		if (strcmp(temp->info->numeMedic, denumire) == 0)
		{
			nr++;
		}
		temp = temp->next;
	}
	if (nr != 0)
	{
		temp = cap;
		v->nrElem = nr;
		unsigned char aux = nr;
		v->info = (Reteta**)malloc(sizeof(Reteta*) * nr);
		while (temp)
		{
			if (strcmp(temp->info->numeMedic, denumire) == 0)
			{
				v->info[nr - aux] = creareRetetaNoua(temp->info);
				aux--;
			}
			temp = temp->next;
		}
	}
	else
	{
		v->nrElem = 0;
		v->info = NULL;
	}
}

void afisareVector(Vector v)
{
	for (unsigned char i = 0; i < v.nrElem; i++)
	{
		printf("v[%d]=Numar reteta: %u\nEmisa de: %s\nNumar medicamente: %hhu\nLista medicamente:\n",i, v.info[i]->nrReteta, v.info[i]->numeMedic, v.info[i]->nrMedicamente);
		for (unsigned char j = 0; j < v.info[i]->nrMedicamente; j++)
		{
			printf("Medicamentul %s in cantitatea %hhu are pretul %.2f si o compensare de %hhu%%\n", v.info[i]->listaMedicamente[j], v.info[i]->cantitatiMedicamente[j], v.info[i]->preturiMedicamente[j], v.info[i]->procenteCompensare[j]);
		}
		printf("\n");
	}
}

void totalCompensat(Vector v)
{
	if (v.nrElem == 0)
		return;
	float total = 0.0f;
	for (unsigned char i = 0; i < v.nrElem; i++)
	{
		for (unsigned char j = 0; j < v.info[i]->nrMedicamente; j++)
		{
			total += (float)v.info[i]->cantitatiMedicamente[j] * v.info[i]->preturiMedicamente[j] * (float)v.info[i]->procenteCompensare[j] / 100;
		}
	}
	printf("\nTotalul valorii compensate a valorilor din vectorul create la punctul anterior este %.2f RON.", total);
}

void totalCantitate(nodLS* cap, const char* denumire)
{
	nodLS* temp = cap;
	unsigned short total = 0;
	while (temp)
	{
		for (unsigned char i = 0; i < temp->info->nrMedicamente; i++)
		{
			if (strcmp(temp->info->listaMedicamente[i], denumire) == 0)
				total += temp->info->cantitatiMedicamente[i];
		}
		temp = temp->next;
	}
	if (total == 0)
	{
		printf("\nMedicamentul %s nu se gaseste in lista!", denumire);
	}
	else printf("\nMedicamentul %s a fost vandut intr-o cantitate totala de %hu unitati.", denumire, total);
}

void dezalocareLista(nodLS** cap)
{
	while (*cap)
	{
		nodLS* aux = *cap;
		*cap = (*cap)->next;
		free(aux->info->numeMedic);
		free(aux->info->cantitatiMedicamente);
		free(aux->info->preturiMedicamente);
		free(aux->info->procenteCompensare);
		for (unsigned char i = 0; i < aux->info->nrMedicamente; i++)
		{
			free(aux->info->listaMedicamente[i]);
		}
		free(aux->info->listaMedicamente);
		free(aux->info);
		free(aux);          
	}
}

void dezalocareVector(Vector* v)
{
	if (v->nrElem == 0)
		return;
	for (unsigned char i = 0; i < v->nrElem; i++)
	{
		free(v->info[i]->cantitatiMedicamente);
		free(v->info[i]->numeMedic);
		free(v->info[i]->preturiMedicamente);
		free(v->info[i]->procenteCompensare);
		for (unsigned char j = 0; j < v->info[i]->nrMedicamente; j++)
			free(v->info[i]->listaMedicamente[j]);
		free(v->info[i]->listaMedicamente);
		free(v->info[i]);
	}
	free(v->info);
}

void main()
{
	nodLS* cap = NULL, *coada = NULL;
	Vector v;
	const char numeMedic[] = "Dr. Maria Floarea";
	const char numeMedicament[] = "Losartan 50mg";
	citireElementeLista(&cap, &coada, "date.txt");
	printf("Afisare de la cap:\n");
	afisareLista(cap);
	printf("Afisare de la coada:\n");
	afisareListaReversed(coada);
	totalIncasari(cap, "Diazepam 100g");
	printf("\n\nIncercare conversie lista dubla pentru medicul %s...", numeMedic);
	conversieListaLaVector(cap, &v, numeMedic);
	if (v.nrElem != 0)
	{
		printf("\n%s a fost gasit in lista si s-a format urmatorul vector:\n", numeMedic);
	}
	else
	{
		printf("\nMedicul %s n-a fost gasit in lista!", numeMedic);
	}
	afisareVector(v);
	totalCompensat(v);
	printf("\n");
	totalCantitate(cap, numeMedicament);
	dezalocareLista(&cap);
	dezalocareVector(&v);
	_CrtDumpMemoryLeaks();
}
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#include <stdbool.h>
#pragma warning(disable:4996)
#define _CRTDBG_MAP_ALLOC
#define DIM 31
#define LINE_SIZE 128

typedef struct TabelaDispersie hashT;
typedef struct ListaSimpla nodLS;
typedef struct ContBancar ContBancar;
typedef struct Vector1 Vector1;
typedef struct Vector2 Vector2;
typedef struct Vector3 Vector3;

struct TabelaDispersie
{
	nodLS** vect;
	unsigned char nrElem;
};

struct ListaSimpla
{
	ContBancar* info;
	nodLS* next;
};

struct ContBancar
{
	char* nume_client;
	float sold;
	char* valuta_cont;
	bool premium;
	float dobanda;
	unsigned short limita_retragere;
	char* banca;
};

struct Vector1
{
	ContBancar* vect;
	unsigned char nrElem;
};

struct Vector2
{
	unsigned char dimCluster, indexTabela;
};

struct Vector3
{
	char* numeClient;
	float soldTotal;
};

unsigned char hash(hashT tabela, const char* nume)
{
	unsigned int valoare = 5381;
	unsigned char c;
	while (c = *nume++)
	{
		valoare = ((valoare << 7) + valoare) + c;
	}
	return valoare % tabela.nrElem;
}

nodLS* creareNod(ContBancar* cb)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = cb;
	nou->next = NULL;
	return nou;
}

void inserareHashT(hashT tabela, ContBancar* cb)
{
	unsigned char pozitie = hash(tabela, cb->nume_client);
	nodLS* nou = creareNod(cb);
	nou->next = tabela.vect[pozitie];
	tabela.vect[pozitie] = nou;
}

void citireFisier(hashT tabela, const char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	char buffer[LINE_SIZE];
	unsigned char nrElem;
	fscanf(f, "%hhu", &nrElem);
	for (unsigned char i = 0; i < nrElem; i++)
	{
		ContBancar* cb = (ContBancar*)malloc(sizeof(ContBancar));
		fscanf(f, " %[^\n]", buffer);
		cb->nume_client = strdup(buffer);
		fscanf(f, "%f", &cb->sold);
		fscanf(f, " %[^\n]", buffer);
		cb->valuta_cont = strdup(buffer);
		fscanf(f, "%hhu", &cb->premium);
		fscanf(f, "%f", &cb->dobanda);
		fscanf(f, "%hu", &cb->limita_retragere);
		fscanf(f, " %[^\n]", buffer);
		cb->banca = strdup(buffer);
		inserareHashT(tabela, cb);
	}
}

void traversareLista(nodLS* cap)
{
	nodLS* index = cap;
	while (index)
	{
		printf("\nNume client: %s\nValuta: %s\nBanca: %s\nSold: %.2f\nPremium: %s\nDobanda: %.2f%%\nLimita retragere zilnica: %hu\n", index->info->nume_client, index->info->valuta_cont, index->info->banca, index->info->sold, index->info->premium ? "Da" : "Nu", index->info->dobanda, index->info->limita_retragere);
		index = index->next;
	}
}

void traversareHashT(hashT tabela)
{
	for (unsigned char i = 0; i < tabela.nrElem; i++)
		if (tabela.vect[i] != NULL)
		{
			printf("\nPozitia %hhu", i);
			traversareLista(tabela.vect[i]);
		}
}

ContBancar DeepCopy(ContBancar* cb)
{
	ContBancar nou;
	nou.banca = strdup(cb->banca);
	nou.dobanda = cb->dobanda;
	nou.limita_retragere = cb->limita_retragere;
	nou.nume_client = strdup(cb->nume_client);
	nou.premium = cb->premium;
	nou.valuta_cont = strdup(cb->valuta_cont);
	nou.sold = cb->sold;
	return nou;
}

void sortareDupaValuta(hashT tabela,Vector1* v, const char* valuta)
{
	unsigned char nrElemVector = 0;
	for(unsigned char i=0; i<tabela.nrElem; i++)
		if (tabela.vect[i])
		{
			nodLS* aux = tabela.vect[i];
			while (aux)
			{
				if (!strcmp(aux->info->valuta_cont, valuta))
					nrElemVector++;
				aux = aux->next;
			}
		}
	if (nrElemVector == 0)
	{
		v->nrElem = 0;
		v->vect = NULL;
		return;
	}
	v->nrElem = nrElemVector;
	unsigned char auxIndex = v->nrElem;
	v->vect = (ContBancar*)malloc(sizeof(ContBancar) * v->nrElem);
	for(unsigned char i=0; i<tabela.nrElem; i++)
		if (tabela.vect[i])
		{
			nodLS* aux = tabela.vect[i];
			while (aux)
			{
				if (!strcmp(aux->info->valuta_cont, valuta))
				{
					v->vect[v->nrElem - auxIndex] = DeepCopy(aux->info);
					auxIndex--;
				}
				aux = aux->next;
			}
		}
}

void traversareVector1(Vector1 v)
{
	for (unsigned char i = 0; i < v.nrElem; i++)
	{
		printf("\nNume client: %s\nValuta: %s\nBanca: %s\nSold: %.2f\nPremium: %s\nDobanda: %.2f%%\nLimita retragere zilnica: %hu\n", v.vect[i].nume_client, v.vect[i].valuta_cont, v.vect[i].banca, v.vect[i].sold, v.vect[i].premium ? "Da" : "Nu", v.vect[i].dobanda, v.vect[i].limita_retragere);
	}
}

Vector2* caracterizareClustere(hashT tabela, unsigned char* nrClustere)
{
	*nrClustere = 0;
	for(unsigned char i=0; i<tabela.nrElem; i++)
		if (tabela.vect[i])
		{
			unsigned char nrElementeLista = 0;
			nodLS* aux = tabela.vect[i];
			while (aux)
			{
				nrElementeLista++;
				aux = aux->next;
			}
			if (nrElementeLista > 1)
				(*nrClustere)++;
		}
	Vector2* v = (Vector2*)malloc(sizeof(Vector2) * (*nrClustere));
	unsigned char auxInteger = *nrClustere;
	for (unsigned char i = 0; i < tabela.nrElem; i++)
		if (tabela.vect[i])
		{
			unsigned char nrElementeLista = 0;
			nodLS* aux = tabela.vect[i];
			while (aux)
			{
				nrElementeLista++;
				aux = aux->next;
			}
			if (nrElementeLista > 1)
			{
				v[*nrClustere - auxInteger].dimCluster = nrElementeLista;
				v[*nrClustere - auxInteger].indexTabela = i;
				auxInteger--;
			}
		}
	return v;
}

void traversareVector2(Vector2* v, unsigned char dim)
{
	for (unsigned char i = 0; i < dim; i++)
	{
		printf("\nIndex hashT: %hhu, Dimensiune cluster: %hhu", v[i].indexTabela, v[i].dimCluster);
	}
}

Vector3* solduriLaNivelDeClient(hashT tabela, Vector1 v, unsigned char* nrElem)
{
	char** names = (char**)malloc(sizeof(char*) * v.nrElem);
	for (unsigned char i = 0; i < v.nrElem; i++)
		names[i] = NULL;
	names[0] = strdup(v.vect[0].nume_client);
	unsigned char k = 1;
	for (unsigned char i = 0; i < v.nrElem; i++)
	{
		bool flag = false;
		for (unsigned char j = 0; j < k; j++)
			if (!strcmp(v.vect[i].nume_client, names[j]))
			{
				flag = true; 
				break;
			}
		if (!flag)
		{
			names[k++] = strdup(v.vect[i].nume_client);
		}
	}
	*nrElem = k;
	unsigned char auxInteger = *nrElem;
	Vector3* w = (Vector3*)malloc(sizeof(Vector3) * (*nrElem));
	for (unsigned char i = 0; i < *nrElem; i++)
	{
		float total = 0.0f;
		unsigned char pozitie = hash(tabela, names[i]);
		nodLS* aux = tabela.vect[pozitie];
		while (aux)
		{
			if (!strcmp(aux->info->valuta_cont, v.vect[0].valuta_cont))
			{
				total += aux->info->sold;
			}
			aux = aux->next;
		}
		w[*nrElem - auxInteger].numeClient = strdup(names[i]);
		w[*nrElem - auxInteger].soldTotal = total;
		auxInteger--;
		free(names[i]);
	}
	free(names);
	return w;
}

void traversareVector3(Vector3* v, unsigned char dim)
{
	for (unsigned char i = 0; i < dim; i++)
		printf("\nClientul %s are soldul agregat %.2f", v[i].numeClient, v[i].soldTotal);
}

void dezalocareVector3(Vector3** v, unsigned char* dim)
{
	for (unsigned char i = 0; i < *dim; i++)
		free((*v)[i].numeClient);
	free(*v);
	*v = NULL;
	*dim = -1;
}

void dezalocareVector1(Vector1* v)
{
	for (unsigned char i = 0; i < v->nrElem; i++)
	{
		free(v->vect[i].banca);
		free(v->vect[i].nume_client);
		free(v->vect[i].valuta_cont);
	}
	free(v->vect);
	v->vect = NULL;
	v->nrElem = -1;
}

void dezalocareLista(nodLS** cap)
{
	while (*cap)
	{
		nodLS* aux = *cap;
		*cap = (*cap)->next;
		free(aux->info->banca);
		free(aux->info->nume_client);
		free(aux->info->valuta_cont);
		free(aux->info);
		free(aux);
	}
}

void dezalocareHashT(hashT* tabela)
{
	for (unsigned char i = 0; i < tabela->nrElem; i++)
	{
		if (tabela->vect[i])
			dezalocareLista(&tabela->vect[i]);
	}
	free(tabela->vect);
	tabela->nrElem = -1;
}

void main()
{
	hashT tabela;
	Vector1 v1;
	Vector2* v2;
	Vector3* v3;
	unsigned char nrElemV2;
	unsigned char nrElemV3;
	tabela.nrElem = DIM;
	tabela.vect = (nodLS**)malloc(sizeof(nodLS*) * tabela.nrElem);
	for (unsigned char i = 0; i < tabela.nrElem; i++)
		tabela.vect[i] = NULL;
	const char valuta[] = "RON";
	citireFisier(tabela, "date.txt");
	traversareHashT(tabela);
	sortareDupaValuta(tabela, &v1, valuta);
	printf("\n\nConturi care au valuta %s\n", valuta);
	traversareVector1(v1);
	printf("\nCaracterizare a clusterelor din hashT:\n");
	v2 = caracterizareClustere(tabela, &nrElemV2);
	traversareVector2(v2, nrElemV2);
	v3 = solduriLaNivelDeClient(tabela, v1, &nrElemV3);
	printf("\n\nSolduri agregate la nivel de client:\n");
	traversareVector3(v3, nrElemV3);
	free(v2);
	nrElemV2 = -1;
	dezalocareVector1(&v1);
	dezalocareVector3(&v3, &nrElemV3);
	dezalocareHashT(&tabela);
	_CrtDumpMemoryLeaks();
}
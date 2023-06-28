#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#include <stdbool.h>
#pragma warning(disable:4996)
#define LINE_SIZE 128
#define _CRTDBG_MAP_ALLOC

typedef struct Comanda Comanda;
typedef struct Heap Heap;
typedef struct Vector1 Vector1;
typedef struct Vector2 Vector2;
typedef struct ArboreBinarDeCautare nodArb;

struct Comanda
{
	int timp_livrare;
	int cod_client;
	int id_comanda;
	char* numeClient;
	float pret;
	char* adresaLivrare;
	char* status;
};

struct Vector1
{
	Comanda* vect;
	unsigned char nrElem;
};

struct Vector2
{
	int cod_client;
	float suma;
};

struct Heap
{
	Comanda* vect;
	unsigned char nrElem;
};

struct ArboreBinarDeCautare
{
	Comanda info;
	nodArb* st, *dr;
};

nodArb* creareNodArb(Comanda c, nodArb* st, nodArb* dr)
{
	nodArb* nou = (nodArb*)malloc(sizeof(nodArb));
	nou->info.cod_client = c.cod_client;
	nou->info.id_comanda = c.id_comanda;
	nou->info.pret = c.pret;
	nou->info.timp_livrare = c.timp_livrare;
	nou->info.numeClient = strdup(c.numeClient);
	nou->info.adresaLivrare = strdup(c.adresaLivrare);
	nou->info.status = strdup(c.status);
	nou->st = st;
	nou->dr = dr;
	return nou;
}

void inserareInArbore(nodArb** rad, Comanda c)
{
	if (*rad != NULL)
	{
		if (c.id_comanda < (*rad)->info.id_comanda)
		{
			inserareInArbore(&(*rad)->st, c);
		}
		else if (c.id_comanda > (*rad)->info.id_comanda)
		{
			inserareInArbore(&(*rad)->dr, c);
		}
	}
	else *rad = creareNodArb(c, NULL, NULL);
}

void preordine(nodArb* rad)
{
	if (rad != NULL)
	{
		printf("\nId comanda: %d\nCod client: %d\nTimp livrare: %d\nNume client: %s\nAdresa livrare:%s\nPret: %.2f\nStatus: %s\n", rad->info.id_comanda, rad->info.cod_client, rad->info.timp_livrare, rad->info.numeClient, rad->info.adresaLivrare, rad->info.pret, rad->info.status);
		preordine(rad->st);
		preordine(rad->dr);
	}
}

void inordine(nodArb* rad)
{
	if (rad != NULL)
	{
		inordine(rad->st);
		printf("\nId comanda: %d\nCod client: %d\nTimp livrare: %d\nNume client: %s\nAdresa livrare:%s\nPret: %.2f\nStatus: %s\n", rad->info.id_comanda, rad->info.cod_client, rad->info.timp_livrare, rad->info.numeClient, rad->info.adresaLivrare, rad->info.pret, rad->info.status);
		inordine(rad->dr);
	}
}

void citireDinFisier(nodArb** rad, const unsigned char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	Comanda c;
	char buffer[LINE_SIZE];
	unsigned char nrElem;
	fscanf(f, "%hhu", &nrElem);
	for (unsigned char i = 0; i < nrElem; i++)
	{
		fscanf(f, "%d", &c.timp_livrare);
		fscanf(f, "%d", &c.cod_client);
		fscanf(f, "%d", &c.id_comanda);
		fscanf(f, " %[^\n]", buffer);
		c.numeClient = strdup(buffer);
		fscanf(f, "%f", &c.pret);
		fscanf(f, " %[^\n]", buffer);
		c.adresaLivrare = strdup(buffer);
		fscanf(f, " %[^\n]", buffer);
		c.status = strdup(buffer);
		inserareInArbore(rad, c);
		free(c.numeClient);
		free(c.adresaLivrare);
		free(c.status);
	}
	fclose(f);
}

void nrComenziPesteTimp(nodArb* rad, const int timpLivrare, Vector1* v)
{
	if (rad != NULL)
	{
		nrComenziPesteTimp(rad->st, timpLivrare, v);
		if (rad->info.timp_livrare > timpLivrare)
			v->nrElem++;
		nrComenziPesteTimp(rad->dr, timpLivrare, v);
	}
}

Comanda DeepCopyArb(Comanda c)
{
	Comanda nou;
	nou.cod_client = c.cod_client;
	nou.pret = c.pret;
	nou.id_comanda = c.id_comanda;
	nou.timp_livrare = c.timp_livrare;
	nou.adresaLivrare = strdup(c.adresaLivrare);
	nou.numeClient = strdup(c.numeClient);
	nou.status = strdup(c.status);
	return nou;
}

void populareComenziPesteTimp(nodArb* rad, const int timpLivrare, Vector1 v, int* aux)
{
	if (rad != NULL)
	{
		populareComenziPesteTimp(rad->st, timpLivrare, v, aux);
		if (rad->info.timp_livrare > timpLivrare)
		{
			v.vect[v.nrElem - (*aux)] = DeepCopyArb(rad->info);
			(*aux)--;
		}
		populareComenziPesteTimp(rad->dr, timpLivrare, v, aux);
	}
}

void traversareVector1(Vector1 v)
{
	for (unsigned char i = 0; i < v.nrElem; i++)
	{
		printf("\nId comanda: %d\nCod client: %d\nTimp livrare: %d\nNume client: %s\nAdresa livrare:%s\nPret: %.2f\nStatus: %s\n", v.vect[i].id_comanda, v.vect[i].cod_client, v.vect[i].timp_livrare, v.vect[i].numeClient, v.vect[i].adresaLivrare, v.vect[i].pret, v.vect[i].status);
	}
}

void filtrare(Heap h, unsigned char index)
{
	unsigned char indexRad = index;
	unsigned char indexSt = 2 * index + 1;
	unsigned char indexDr = 2 * index + 2;
	if (indexSt < h.nrElem && h.vect[indexSt].timp_livrare > h.vect[indexRad].timp_livrare)
		indexRad = indexSt;
	if (indexDr < h.nrElem && h.vect[indexDr].timp_livrare > h.vect[indexRad].timp_livrare)
		indexRad = indexDr;
	if (index != indexRad)
	{
		Comanda aux = h.vect[index];
		h.vect[index] = h.vect[indexRad];
		h.vect[indexRad] = aux;
		filtrare(h, indexRad);
	}
}

void inserare(Heap* h, Comanda c)
{
	Comanda* vectNou = (Comanda*)malloc((h->nrElem + 1) * sizeof(Comanda));
	for (unsigned char i = 0; i < h->nrElem; i++)
		vectNou[i] = h->vect[i];
	h->nrElem++;
	vectNou[h->nrElem - 1] = DeepCopyArb(c);
	free(h->vect);
	h->vect = vectNou;
	for (char i = (h->nrElem - 1) / 2; i >= 0; i--)
		filtrare(*h, i);
}

void BST2Heap(nodArb* rad, Heap* h)
{
	if (rad != NULL)
	{
		BST2Heap(rad->st, h);
		inserare(h, rad->info);
		BST2Heap(rad->dr, h);
	}
}

void ClientiUnici(nodArb* rad, int** coduri, unsigned char* k)
{
	if (rad != NULL)
	{
		ClientiUnici(rad->st, coduri, k);
		bool flag = false;
		for (unsigned char i = 0; i < *k; i++)
		{
			if ((*coduri)[i]==rad->info.cod_client)
			{
				flag = true;
				break;
			}
		}
		if (!flag)
		{
			(*coduri)[(*k)++] = rad->info.cod_client;
		}
		ClientiUnici(rad->dr, coduri, k);
	}
}

void populareVector2(nodArb* rad, int* coduri, unsigned char nrC, Vector2** w)
{
	if (rad != NULL)
	{
		populareVector2(rad->st, coduri, nrC, w);
		for(unsigned char i = 0; i<nrC; i++)
			if (coduri[i] == rad->info.cod_client)
			{
				(*w)[i].suma += rad->info.pret;
				break;
			}
		populareVector2(rad->dr, coduri, nrC, w);
	}
}

void afisareVector2(Vector2* v, unsigned char k)
{
	for (unsigned char i = 0; i < k; i++)
		printf("\nCod client: %d Suma totala a comenzilor: %.2f", v[i].cod_client, v[i].suma);
}

void dezalocareArb(nodArb** rad)
{
	if (*rad != NULL)
	{
		dezalocareArb(&(*rad)->st);
		dezalocareArb(&(*rad)->dr);
		free((*rad)->info.adresaLivrare);
		free((*rad)->info.numeClient);
		free((*rad)->info.status);
		free(*rad);
		*rad = NULL;
	}
}

void dezalocareHeap(Heap* h)
{
	for (unsigned char i = 0; i < h->nrElem; i++)
	{
		free(h->vect[i].adresaLivrare);
		free(h->vect[i].status);
		free(h->vect[i].numeClient);
	}
	free(h->vect);
	h->nrElem = -1;
}

void dezalocareVector1(Vector1* v)
{
	for (unsigned char i = 0; i < v->nrElem; i++)
	{
		free(v->vect[i].adresaLivrare);
		free(v->vect[i].status);
		free(v->vect[i].numeClient);
	}
	free(v->vect);
	v->nrElem = -1;
}

void dezalocareVector2(Vector2** v)
{
	free(*v);
	*v = NULL;
}

void main()
{
	nodArb* rad = NULL;
	Vector1 v;
	Heap h;
	h.nrElem = 0;
	h.vect = NULL;
	v.nrElem = 0;
	const int timpLivrare = 35;
	int timpLivrareMaxim = 0;
	citireDinFisier(&rad, "date.txt");
	inordine(rad);
	nrComenziPesteTimp(rad, timpLivrare, &v);
	v.vect = (Comanda*)malloc(sizeof(Comanda) * v.nrElem);
	int aux = v.nrElem;
	populareComenziPesteTimp(rad, timpLivrare, v, &aux);
	printf("\nComenzile cu un timp de livrare mai mare de %d\n", timpLivrare);
	traversareVector1(v);
	BST2Heap(rad, &h);
	int* coduriClient = (int*)malloc(sizeof(int) * h.nrElem);
	unsigned char k = 0;
	ClientiUnici(rad, &coduriClient, &k);
	Vector2* w;
	w = (Vector2*)malloc(sizeof(Vector2) * k);
	for (unsigned char i = 0; i < k; i++)
		w[i].cod_client = coduriClient[i], w[i].suma = 0.0f;
	populareVector2(rad, coduriClient, k, &w);
	afisareVector2(w, k);
	dezalocareArb(&rad);
	dezalocareHeap(&h);
	dezalocareVector1(&v);
	dezalocareVector2(&w);
	free(coduriClient);
	_CrtDumpMemoryLeaks();
}
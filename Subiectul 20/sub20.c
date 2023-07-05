#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#include <stdbool.h>
#pragma warning(disable:4996)
#define _CRTDBG_MAP_ALLOC
#define DIM 31
#define LINE_SIZE 128

typedef struct InfoGradi Gradinita;
typedef struct TabelaDispersie hashT;
typedef struct ListaDubla nodLS;

struct InfoGradi
{
	char* nume;
	char* oras;
	unsigned char capacitate;
	unsigned char nrActualCopii;
	bool areProgramPrelungit;
};

struct TabelaDispersie
{
	nodLS** vect;
	unsigned char nrElem;
};

struct ListaDubla
{
	Gradinita info;
	nodLS* next, *prev;
};

unsigned char hash(const char* str, unsigned char dim)
{
	unsigned int valoare = 5381;
	unsigned char c;
	while (c = *str++)
		valoare = ((valoare << 5) + valoare) ^ c;
	return valoare % dim;
}

nodLS* creareNodListaShallow(Gradinita g)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = g;
	nou->next = NULL;
	nou->prev = NULL;
	return nou;
}

void inserareListaDubla(nodLS** cap, Gradinita g)
{
	if (*cap == NULL)
	{
		*cap = creareNodListaShallow(g);
	}
	else
	{
		nodLS* nou = creareNodListaShallow(g);
		nodLS* temp = *cap;
		while (temp->next)
		{
			temp = temp->next;
		}
		nou->prev = temp;
		temp->next = nou;
	}
}

void inserareHashT(hashT tabela, Gradinita g)
{
	unsigned char pozitie = hash(g.oras, tabela.nrElem);
	inserareListaDubla(&tabela.vect[pozitie], g);
}

void citireFisier(hashT* tabela, const unsigned char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	if (!f)
		exit(-1);
	tabela->nrElem = DIM;
	tabela->vect = (nodLS**)calloc(tabela->nrElem, sizeof(nodLS*));
	unsigned char nrElem;
	Gradinita g;
	fscanf(f, "%hhu", &nrElem);
	char buffer[LINE_SIZE];
	for (unsigned char i = 0; i < nrElem; i++)
	{
		fscanf(f, " %[^\n]", buffer);
		g.nume = strdup(buffer);
		fscanf(f, " %[^\n]", buffer);
		g.oras = strdup(buffer);
		fscanf(f, "%hhu %hhu %hhu", &g.capacitate, &g.nrActualCopii, &g.areProgramPrelungit);
		inserareHashT(*tabela, g);
	}
	fclose(f);
}

void traversareListaDubla(nodLS* cap)
{
	nodLS* index = cap;
	while (index)
	{
		printf("\nNume gradinita: %s\nOras: %s\nNr. actual copii/Capacitate: %hhu/%hhu\n%s program prelungit\n", index->info.nume, index->info.oras, index->info.nrActualCopii, index->info.capacitate, index->info.areProgramPrelungit ? "Are" : "N-are");
		index = index->next;
	}
}

void traversareHashT(hashT tabela)
{
	for (unsigned char i = 0; i < tabela.nrElem; i++)
		if (tabela.vect[i])
			printf("\n\nSublista %hhu\n\n", i + 1), traversareListaDubla(tabela.vect[i]);
}

void schimbaOrasulGradinitelorDin(const char* orasVechi, const char* orasNou, hashT tabela, bool* flag)
{
	if (tabela.vect[hash(orasVechi, tabela.nrElem)] == NULL)
	{
		*flag = false;
		return;
	}
	nodLS* nouaPozitie = tabela.vect[hash(orasNou, tabela.nrElem)];
	if (nouaPozitie == NULL)
	{
		tabela.vect[hash(orasNou, tabela.nrElem)] = tabela.vect[hash(orasVechi, tabela.nrElem)];
	}
	else
	{
		while (nouaPozitie->next)
			nouaPozitie = nouaPozitie->next;
		nouaPozitie->next = tabela.vect[hash(orasVechi, tabela.nrElem)];
		tabela.vect[hash(orasVechi, tabela.nrElem)]->prev = nouaPozitie;
	}
	nodLS* temp = tabela.vect[hash(orasVechi, tabela.nrElem)];
	while (temp)
	{
		free(temp->info.oras);
		temp->info.oras = strdup(orasNou);
		temp = temp->next;
	}
	tabela.vect[hash(orasVechi, tabela.nrElem)] = NULL;
}

nodLS* creareNodListaDeep(Gradinita g)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = g;
	nou->info.nume = strdup(g.nume);
	nou->info.oras = strdup(g.oras);
	nou->next = NULL;
	nou->prev = NULL;
	return nou;
}

void inserareListaDublaDinHashT(nodLS** cap, Gradinita g)
{
	if (*cap == NULL)
		*cap = creareNodListaDeep(g);
	else
	{
		nodLS* temp = *cap;
		nodLS* nou = creareNodListaDeep(g);
		while (temp->next)
			temp = temp->next;
		nou->prev = temp;
		temp->next = nou;
	}
}

void hashT2ListaDubla(hashT tabela, nodLS** cap)
{
	*cap = NULL;
	for(unsigned char i=0; i<tabela.nrElem;i++)
		if (tabela.vect[i])
		{
			nodLS* temp = tabela.vect[i];
			while (temp)
			{
				if (!temp->info.areProgramPrelungit)
					inserareListaDublaDinHashT(cap, temp->info);
				temp = temp->next;
			}
		}
}

void spargeListe(nodLS** cap, nodLS** capPestePrag, nodLS** capSubPrag, const unsigned char prag)
{
	*capPestePrag = *capSubPrag = NULL;
	while (*cap)
	{
		nodLS* temp = *cap;
		if (temp->info.nrActualCopii > prag)
			inserareListaDublaDinHashT(capPestePrag, temp->info);
		else inserareListaDublaDinHashT(capSubPrag, temp->info);
		*cap = (*cap)->next;
		free(temp->info.nume);
		free(temp->info.oras);
		free(temp);
	}
}

void dezalocareListaDubla(nodLS** cap)
{
	while (*cap)
	{
		nodLS* aux = *cap;
		(*cap) = (*cap)->next;
		free(aux->info.nume);
		free(aux->info.oras);
		free(aux);
	}
}

void dezalocareHashT(hashT* tabela)
{
	for (unsigned char i = 0; i < tabela->nrElem; i++)
		if (tabela->vect[i])
			dezalocareListaDubla(&tabela->vect[i]);
	free(tabela->vect);
	tabela->nrElem = 0;
}

void main()
{
	hashT tabela;
	nodLS* gradiniteFaraProgramPrelungit;
	nodLS* capSubPrag, * capPestePrag;
	bool flag;
	const unsigned char* numeFisier = "date.txt";
	const char* orasVechiGradi = "Oradea";
	const char* orasNouGradi = "Ciocoiesti";
	const unsigned char prag = 50;
	citireFisier(&tabela, numeFisier);
	traversareHashT(tabela);
	schimbaOrasulGradinitelorDin(orasVechiGradi, orasNouGradi, tabela, &flag);
	if (!flag)
	{
		printf("\n\nNu exista gradinite in orasul %s\n\n", orasVechiGradi);
	}
	else
	{
		printf("\n\nGradinitile dupa modificarea din orasul %s in %s\n\n", orasVechiGradi, orasNouGradi);
		traversareHashT(tabela);
	}
	hashT2ListaDubla(tabela, &gradiniteFaraProgramPrelungit);
	if (gradiniteFaraProgramPrelungit == NULL)
	{
		printf("\n\nNu exista gradinite cu program prelungit!\n\n");
	}
	else
	{
		printf("\n\nLista gradinitelor cu program prelungit:\n\n");
		traversareListaDubla(gradiniteFaraProgramPrelungit);
	}
	spargeListe(&gradiniteFaraProgramPrelungit, &capSubPrag, &capPestePrag, prag);
	printf("\n\nLista cu gradinitile sub pragul de %hhu copii deja inscrisi:\n\n", prag);
	traversareListaDubla(capSubPrag);
	printf("\n\nLista cu gradinitile peste pragul de %hhu copii deja inscrisi:\n\n", prag);
	traversareListaDubla(capPestePrag);
	dezalocareHashT(&tabela);
	dezalocareListaDubla(&capSubPrag);
	dezalocareListaDubla(&capPestePrag);
	_CrtDumpMemoryLeaks();
}
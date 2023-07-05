#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#pragma warning(disable:4996)
#define DIM 31
#define LINE_SIZE 128
#define _CRTDBG_MAP_ALLOC

typedef struct InfoProiect Proiect;
typedef struct TabelaDispersie hashT;
typedef struct ListaSimpla nodLS;

struct InfoProiect
{
	unsigned int cod;
	char* titlu;
	char* beneficiar;
	unsigned char nrExecutanti;
	float buget;
};

struct TabelaDispersie
{
	nodLS** vect;
	unsigned char nrElem;
};

struct ListaSimpla
{
	Proiect info;
	nodLS* next;
};

unsigned char hash(const char* beneficiar, unsigned int dim)
{
	unsigned int valoare = 5381;
	char c;
	while (c = *beneficiar++)
		valoare = ((valoare << 13) + valoare) ^ c;
	return valoare % dim;
}

nodLS* creareNodLS(Proiect p)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = p;
	nou->next = NULL;
	return nou;
}

void inserareNodLista(nodLS** cap, Proiect p)
{
	if (*cap == NULL)
		*cap = creareNodLS(p);
	else
	{
		nodLS* nou = creareNodLS(p);
		nou->next = *cap;
		*cap = nou;
	}
}

void inserareHashT(hashT tabela, Proiect p)
{
	unsigned char pozitie = hash(p.beneficiar, tabela.nrElem);
	inserareNodLista(&tabela.vect[pozitie], p);
}

void citireFisier(hashT* tabela, const unsigned char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	if (!f)
		exit(-1);
	tabela->nrElem = DIM;
	tabela->vect = (nodLS**)calloc(tabela->nrElem, sizeof(nodLS*));
	unsigned char nrProiecte;
	fscanf(f, "%hhu", &nrProiecte);
	Proiect p;
	char buffer[LINE_SIZE];
	for (unsigned char i = 0; i < nrProiecte; i++)
	{
		fscanf(f, "%u %hhu %f", &p.cod, &p.nrExecutanti, &p.buget);
		fscanf(f, " %[^\n]", buffer);
		p.titlu = strdup(buffer);
		fscanf(f, " %[^\n]", buffer);
		p.beneficiar = strdup(buffer);
		inserareHashT(*tabela, p);
	}
}

void traversareLS(nodLS* cap)
{
	nodLS* temp = cap;
	while (temp)
	{
		printf("Cod: %u\nTitlu: %s\nBeneficiar: %s\nNr. executanti: %hhu\nBuget: %.2f RON\n\n", temp->info.cod, temp->info.titlu, temp->info.beneficiar, temp->info.nrExecutanti, temp->info.buget);
		temp = temp->next;
	}
}

void traversareHashT(hashT tabela)
{
	for(unsigned char i=0; i<tabela.nrElem; i++)
		if (tabela.vect[i])
		{
			printf("Sublista %hhu:\n\n", i);
			traversareLS(tabela.vect[i]);
		}
}

void bugetTotal(const char* beneficiar, hashT tabela, float* total)
{
	*total = 0;
	nodLS* aux = tabela.vect[hash(beneficiar, tabela.nrElem)];
	while (aux)
	{
		*total += aux->info.buget;
		aux = aux->next;
	}
}

void modificaBeneficiar(const char* numeVechi, const char* numeNou, hashT tabela)
{
	nodLS* aux = tabela.vect[hash(numeVechi, tabela.nrElem)];
	unsigned char pozitieNoua = hash(numeNou, tabela.nrElem);
	nodLS* temp = tabela.vect[pozitieNoua];
	if (temp == NULL)
		tabela.vect[pozitieNoua] = aux;
	else
	{
		while (temp->next)
			temp = temp->next;
		temp->next = aux;
	}
	while (aux)
	{
		free(aux->info.beneficiar);
		aux->info.beneficiar = strdup(numeNou);
		aux = aux->next;
	}
	tabela.vect[hash(numeVechi, tabela.nrElem)] = NULL;
}

void dezalocareLS(nodLS** cap)
{
	while (*cap)
	{
		nodLS* aux = (*cap);
		(*cap) = (*cap)->next;
		free(aux->info.beneficiar);
		free(aux->info.titlu);
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

nodLS* DeepCopy(nodLS* c)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info.beneficiar = strdup(c->info.beneficiar);
	nou->info.titlu = strdup(c->info.titlu);
	nou->info.buget = c->info.buget;
	nou->info.cod = c->info.cod;
	nou->info.nrExecutanti = c->info.nrExecutanti;
	nou->next = NULL;
	return nou;
}

void bugetPestePrag(const float prag, hashT tabela, nodLS** cap)
{
	*cap = NULL;
	for (unsigned char i = 0; i < tabela.nrElem; i++) 
		if (tabela.vect[i])
		{
			nodLS* aux = tabela.vect[i];
			while (aux)
			{
				if (aux->info.buget > prag)
				{
					if (*cap == NULL)
						*cap = DeepCopy(aux);
					else
					{
						nodLS* nou = DeepCopy(aux);
						nou->next = *cap;
						*cap = nou;
					}
				}
				aux = aux->next;
			}
		}
}

void main()
{
	hashT tabela;
	nodLS* cap;
	const unsigned char* numeFisier = "date.txt";
	const char* numeBeneficiar = "John Doe Ltd.";
	const char* numeBeneficiarNou = "Delta Finance Corp.";
	const float prag = 2000000.0f;
	float total;
	citireFisier(&tabela, numeFisier);
	traversareHashT(tabela);
	bugetTotal(numeBeneficiar, tabela, &total);
	if (!total)
	{
		printf("\n\nBeneficiarul %s nu exista in hashT!\n\n", numeBeneficiar);
	}
	else
	{
		printf("\n\nBeneficiarul %s are un buget total de %.2f RON\n\n", numeBeneficiar, total);
	}
	modificaBeneficiar(numeBeneficiar, numeBeneficiarNou, tabela);
	traversareHashT(tabela);
	bugetPestePrag(prag, tabela, &cap);
	if(cap)
	{
		printf("\n\n\nProiectele cu buget alocat peste %.2f RON:\n\n", prag);
		traversareLS(cap);
		dezalocareLS(&cap);
	}
	else
	{
		printf("\n\n\nNu exista proiecte cu buget alocat peste %.2f RON.\n\n", prag);
	}
	dezalocareHashT(&tabela);
	_CrtDumpMemoryLeaks();
}
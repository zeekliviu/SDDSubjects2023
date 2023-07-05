#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#pragma warning(disable:4996)
#define DIM 31
#define LINE_SIZE 128
#define _CRTDBG_MAP_ALLOC

typedef struct DosarCandidat Dosar;
typedef struct TabelaDispersie hashT;
typedef struct ListaDubla nodLS;
typedef struct ListaListe nodLP;

struct DosarCandidat
{
	char* numeCandidat;
	char* programStudiu;
	float medieBac;
	int codDosar;
};

struct TabelaDispersie
{
	nodLS** vect;
	unsigned char nrElem;
};

struct ListaDubla
{
	Dosar info;
	nodLS* next, * prev;
};

struct ListaListe
{
	nodLS* info;
	nodLP* next;
};

unsigned char hash(const char* str, unsigned char dim)
{
	unsigned int valoare = 5381;
	unsigned char c;
	while(c = *str++)
		valoare = ((valoare << 5) + valoare) ^ c;
	return valoare % dim;
}

nodLS* creareNodShallow(Dosar d)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = d;
	nou->next = NULL;
	nou->prev = NULL;
	return nou;
}

void inserareListaDubla(nodLS** cap, Dosar d)
{
	if (*cap == NULL)
		*cap = creareNodShallow(d);
	else
	{
		nodLS* nou = creareNodShallow(d);
		nodLS* temp = *cap;
		while (temp->next)
			temp = temp->next;
		nou->prev = temp;
		temp->next = nou;
	}
}

void inserareHashT(hashT tabela, Dosar d)
{
	unsigned char pozitie = hash(d.numeCandidat, tabela.nrElem);
	inserareListaDubla(&tabela.vect[pozitie], d);
}

void citireFisier(hashT* tabela, const char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	if (!f)
		exit(-1);
	tabela->nrElem = DIM;
	tabela->vect = (nodLS**)calloc(tabela->nrElem, sizeof(nodLS*));
	unsigned char nrDosare;
	Dosar d;
	char buffer[LINE_SIZE];
	fscanf(f, "%hhu", &nrDosare);
	for (unsigned char i = 0; i < nrDosare; i++)
	{
		fscanf(f, " %[^\n]", buffer);
		d.numeCandidat = strdup(buffer);
		fscanf(f, " %[^\n]", buffer);
		d.programStudiu = strdup(buffer);
		fscanf(f, "%f", &d.medieBac);
		fscanf(f, "%d", &d.codDosar);
		inserareHashT(*tabela, d);
	}
	fclose(f);
}

void traversareListaDubla(nodLS* cap)
{
	nodLS* temp = cap;
	while (temp)
	{
		printf("Cod dosar: %d\nNume candidat: %s\nProgram studiu: %s\nMedie bac: %.2f\n\n", temp->info.codDosar, temp->info.numeCandidat, temp->info.programStudiu, temp->info.medieBac);
		temp = temp->next;
	}
}

void traversareHashT(hashT tabela)
{
	for (unsigned char i = 0; i < tabela.nrElem; i++)
		if (tabela.vect[i])
			printf("\n\nSublista %hhu:\n\n", i + 1), traversareListaDubla(tabela.vect[i]);
}

void numarCandidatiLa(const char* programStudiu, hashT tabela, unsigned char* nrCandidati)
{
	*nrCandidati = 0;
	for(unsigned char i=0; i<tabela.nrElem; i++)
		if (tabela.vect[i])
		{
			nodLS* temp = tabela.vect[i];
			while (temp)
			{
				if (!strcmp(temp->info.programStudiu, programStudiu))
					(*nrCandidati)++;
				temp = temp->next;
			}
		}
}

nodLS* cautaProgramStudiu(const char* program, nodLP* cap)
{
	nodLP* temp = cap;
	while (temp)
	{
		if (!strcmp(temp->info->info.programStudiu, program))
			return temp->info;
		temp = temp->next;
	}
	return NULL;
}

nodLP* creareNodLL()
{
	nodLP* nou = (nodLP*)malloc(sizeof(nodLP));
	nou->info = NULL;
	nou->next = NULL;
	return nou;
}

nodLS* creareNodDeep(Dosar d)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = d;
	nou->info.numeCandidat = strdup(d.numeCandidat);
	nou->info.programStudiu = strdup(d.programStudiu);
	nou->next = NULL;
	nou->prev = NULL;
	return nou;
}

void inserareListaDublaDeep(nodLS** cap, Dosar d)
{
	if (*cap == NULL)
		*cap = creareNodDeep(d);
	else
	{
		nodLS* nou = creareNodDeep(d);
		nodLS* temp = *cap;
		while (temp->next)
			temp = temp->next;
		nou->prev = temp;
		temp->next = nou;
	}
}

void inserareLL(nodLP** cap, Dosar d)
{
	if (*cap == NULL)
	{
		*cap = creareNodLL();
		inserareListaDublaDeep(&(*cap)->info, d);
	}
	else
	{
		nodLP* nou = creareNodLL();
		inserareListaDublaDeep(&nou->info, d);
		nodLP* temp = *cap;
		while (temp->next)
		{
			temp = temp->next;
		}
		temp->next = nou;
	}
}

void hashT2LL(nodLP** cap, hashT tabela, const float prag)
{
	*cap = NULL;
	for(unsigned char i=0; i<tabela.nrElem; i++)
		if (tabela.vect[i])
		{
			nodLS* temp = tabela.vect[i];
			while (temp)
			{
				if (temp->info.medieBac < prag)
				{
					nodLS* rez = cautaProgramStudiu(temp->info.programStudiu, *cap);
					if (!rez)
					{
						inserareLL(cap, temp->info);
					}
					else
					{
						inserareListaDublaDeep(&rez, temp->info);
					}
				}
				temp = temp->next;
			}
		}
}

float getMin(nodLP* cap)
{
	float min = 10.0f;
	nodLP* temp = cap;
	while (temp)
	{
		nodLS* aux = temp->info;
		while (aux)
		{
			if (aux->info.medieBac < min)
				min = aux->info.medieBac;
			aux = aux->next;
		}
		temp = temp->next;
	}
	return min;
}

void stergeMin(nodLP* cap)
{
	float min = getMin(cap);
	nodLP* temp = cap;
	while (temp)
	{
		nodLS* aux = temp->info;
		while (aux)
		{
			if (aux->info.medieBac == min)
			{
				if (aux == temp->info)
				{
					temp->info = aux->next;
					free(aux->info.numeCandidat);
					free(aux->info.programStudiu);
					free(aux);
					aux = temp->info;
				}
				else if (aux->prev != NULL && aux->next != NULL)
				{
					nodLS* helper = aux->next;
					aux->prev->next = aux->next;
					aux->next->prev = aux->prev;
					free(aux->info.numeCandidat);
					free(aux->info.programStudiu);
					free(aux);
					aux = helper;
				}
				else
				{
					aux->prev->next = NULL;
					free(aux->info.numeCandidat);
					free(aux->info.programStudiu);
					free(aux);
					aux = NULL;
				}
			}
			else aux = aux->next;
		}
		temp = temp->next;
	}
}

void traversareListaListe(nodLP* cap)
{
	nodLP* temp = cap;
	unsigned char i = 0;
	while (temp)
	{
		if (temp->info)
		{
			printf("\n\nSublista %hhu\n\n", ++i);
			traversareListaDubla(temp->info);
		}
		temp = temp->next;
	}
}

void dezalocareListaDubla(nodLS** cap)
{
	while (*cap)
	{
		nodLS* aux = *cap;
		*cap = (*cap)->next;
		free(aux->info.numeCandidat);
		free(aux->info.programStudiu);
		free(aux);
	}
}

void dezalocareListaListe(nodLP** cap)
{
	while (*cap)
	{
		nodLP* aux = *cap;
		*cap = (*cap)->next;
		dezalocareListaDubla(&aux->info);
		free(aux);
	}
}

void dezalocareHashT(hashT* tabela)
{
	for(unsigned char i=0; i<tabela->nrElem; i++)
		if (tabela->vect[i])
		{
			dezalocareListaDubla(&tabela->vect[i]);
		}
	free(tabela->vect);
	tabela->nrElem = 0;
}

void main()
{
	hashT tabela;
	nodLP* listaLichele;
	const char* numeFisier = "date.txt";
	const char* programCautat = "Informatica";
	const float prag = 9.0f;
	citireFisier(&tabela, numeFisier);
	traversareHashT(tabela);
	unsigned char nrCandidati;
	numarCandidatiLa(programCautat, tabela, &nrCandidati);
	if (!nrCandidati)
		printf("\n\nNu exista programul %s in hashT!\n\n", programCautat);
	else
		printf("\n\nSunt %hhu indivizi care vor la %s\n\n", nrCandidati, programCautat);
	hashT2LL(&listaLichele, tabela, prag);
	printf("\n\nLista \"lichelelor\" care au media mai mica de %.2f\n\n", prag);
	traversareListaListe(listaLichele);
	printf("\n\nLista \"lichelelor\" dupa ce s-a sters cel/cei cu media minima\n\n");
	stergeMin(listaLichele);
	traversareListaListe(listaLichele);
	dezalocareHashT(&tabela);
	dezalocareListaListe(&listaLichele);
	_CrtDumpMemoryLeaks();
}
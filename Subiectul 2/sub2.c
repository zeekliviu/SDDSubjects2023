#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#include <stdbool.h>
#define _CRTDBG_MAP_ALLOC
#pragma warning(disable:4996)
#define DIM 31
#define LINE_LENGTH 256

typedef struct RaportAnalizaMedicale RAM;
typedef struct ListaSimpla nodLS;
typedef struct nodLista nodLista;
typedef struct TabelaDispersie hashT;
typedef struct AnalizaMedicala Analiza;

struct RaportAnalizaMedicale
{
	unsigned int cod_raport;
	unsigned int cod_pacient;
	unsigned char nrAnalize;
	char** denumiriAnalize;
	float* valoriAnalize;
	float* valoriReferinta;
	char* dataRecoltarii;
};

struct AnalizaMedicala
{
	unsigned int cod_pacient;
	float valoareAnaliza;
	char* data;
};

struct ListaSimpla
{
	RAM* info;
	nodLS* next;
};

struct nodLista
{
	Analiza info;
	nodLista* next;
};

struct TabelaDispersie
{
	nodLS** vect;
	unsigned char nrElem;
};

unsigned char hash(const char* data, hashT tabela)
{
	unsigned int valoare = 5381;
	unsigned int c;
	while (c = *data++)
	{
		valoare = ((valoare << 7) + valoare) + c;
	}
	return valoare % tabela.nrElem;
}

RAM* creareRaport(RAM ram)
{
	RAM* nou = (RAM*)malloc(sizeof(RAM));
	nou->cod_pacient = ram.cod_pacient;
	nou->cod_raport = ram.cod_raport;
	nou->nrAnalize = ram.nrAnalize;
	nou->dataRecoltarii = ram.dataRecoltarii;
	nou->denumiriAnalize = ram.denumiriAnalize;
	nou->valoriAnalize = ram.valoriAnalize;
	nou->valoriReferinta = ram.valoriReferinta;
	return nou;
}

nodLS* creareNod(RAM ram)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = creareRaport(ram);
	nou->next = NULL;
	return nou;
}

void inserareTabela(hashT tabela, RAM raport)
{
	unsigned char pozitie = hash(raport.dataRecoltarii, tabela);
	if (tabela.vect[pozitie] == NULL)
		tabela.vect[pozitie] = creareNod(raport);
	else
	{
		nodLS* nou = creareNod(raport);
		nou->next = tabela.vect[pozitie];
		tabela.vect[pozitie] = nou;
	}
}

void citireFisier(hashT tabela, const char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	unsigned char nrElem;
	fscanf(f, "%hhu", &nrElem);
	char buffer[LINE_LENGTH];
	RAM raport;
	for (unsigned char i = 0; i < nrElem; i++)
	{
		fscanf(f, "%u", &raport.cod_raport);
		fscanf(f, "%u", &raport.cod_pacient);
		fscanf(f, "%hhu", &raport.nrAnalize);
		raport.denumiriAnalize = (char**)malloc(sizeof(char*) * raport.nrAnalize);
		raport.valoriAnalize = (float*)malloc(sizeof(float) * raport.nrAnalize);
		raport.valoriReferinta = (float*)malloc(sizeof(float) * raport.nrAnalize);
		for (unsigned char j = 0; j < raport.nrAnalize; j++)
		{
			fscanf(f, " %[^\n]", buffer);
			raport.denumiriAnalize[j] = strdup(buffer);
		}
		for (unsigned char j = 0; j < raport.nrAnalize; j++)
		{
			fscanf(f, "%f", &raport.valoriAnalize[j]);
		}
		for (unsigned char j = 0; j < raport.nrAnalize; j++)
		{
			fscanf(f, "%f", &raport.valoriReferinta[j]);
		}
		fscanf(f, "%s", buffer);
		raport.dataRecoltarii = strdup(buffer);
		inserareTabela(tabela, raport);
	}
	fclose(f);
}

void traversareLista(nodLS* cap)
{
	nodLS* temp = cap;
	while (temp)
	{
		printf("\nData recoltarii: %s", temp->info->dataRecoltarii);
		printf("\nCod raport = %u, Cod pacient = %u, Nr. analize = %hhu", temp->info->cod_raport, temp->info->cod_pacient, temp->info->nrAnalize);
		for (unsigned char i = 0; i < temp->info->nrAnalize; i++)
		{
			printf("\nAnaliza: %s, Valoare gasita: %f, Valoare de referinta: %f", temp->info->denumiriAnalize[i], temp->info->valoriAnalize[i], temp->info->valoriReferinta[i]);
		}
		printf("\n");
		temp = temp->next;
	}
}

void traversareHashT(hashT tabela)
{
	for (unsigned char i = 0; i < tabela.nrElem; i++)
	{
		if (tabela.vect[i] != NULL)
		{
			printf("\nPozitia %hhu\n", i);
			traversareLista(tabela.vect[i]);
		}
	}
}

unsigned char nrAnalizeInData(hashT tabela, const unsigned char* data)
{
	unsigned char pozitie = hash(data, tabela);
	if (tabela.vect[pozitie] == NULL)
		return 0;
	else
	{
		unsigned char total = 0;
		nodLS* aux = tabela.vect[pozitie];
		while (aux)
		{
			total++;
			aux = aux->next;
		}
		return total;
	}
}

nodLista* creareNodDeep(unsigned int cod_pacient, float valoare, const char* data)
{
	nodLista* nou = (nodLista*)malloc(sizeof(nodLista));
	nou->info.cod_pacient = cod_pacient;
	nou->info.valoareAnaliza = valoare;
	nou->info.data = strdup(data);
	return nou;
}

void HashT2LinkedList(nodLista** cap, hashT tabela, const unsigned char* denumire)
{
	for (unsigned char i = 0; i < tabela.nrElem; i++)
	{
		if (tabela.vect[i] != NULL)
		{
			nodLS* aux = tabela.vect[i];
			while (aux)
			{
				for (unsigned char j = 0; j < aux->info->nrAnalize; j++)
					if (!strcmp(aux->info->denumiriAnalize[j], denumire))
					{
						nodLista* nou = creareNodDeep(aux->info->cod_pacient,aux->info->valoriAnalize[j], aux->info->dataRecoltarii);
						nou->next = *cap;
						*cap = nou;
					}
				aux = aux->next;
			}
		}
	}
}

void traversareAltaLista(nodLista* cap)
{
	nodLista* index = cap;
	while (index)
	{
		printf("\nAnaliza pacientului %u a avut valoarea %.2f in data de %s", index->info.cod_pacient, index->info.valoareAnaliza, index->info.data);
		index = index->next;
	}
}

unsigned char numarPacienti(nodLista* cap)
{
	if (cap == NULL)
		return 0;
	nodLista* index = cap;
	unsigned char nrPacienti = 0;
	unsigned char nrElem = 0;
	while (index)
	{
		nrElem++;
		index = index->next;
	}
	unsigned int* pacientiUnici = (unsigned int*)malloc(sizeof(unsigned int) * nrElem);
	for (unsigned char i = 0; i < nrElem; i++)
		pacientiUnici[i] = 0;
	index = cap;
	unsigned char aux = nrElem;
	while (index)
	{
		bool flag = false;
		for (unsigned char i = 0; i < nrElem; i++)
		{
			if(pacientiUnici[i]==index->info.cod_pacient)
				flag = true;
		}
		if (!flag)
		{
			pacientiUnici[nrElem - aux] = index->info.cod_pacient;
			aux--;
		}
		index = index->next;
	}
	for (unsigned char i = 0; i < nrElem; i++)
		if (pacientiUnici[i] != 0)
			nrPacienti++;
	free(pacientiUnici);
	return nrPacienti;
}

void dezalocareLista(nodLS** cap)
{
	while (*cap)
	{
		nodLS* aux = *cap;
		*cap = (*cap)->next;
		free(aux->info->dataRecoltarii);
		free(aux->info->valoriAnalize);
		free(aux->info->valoriReferinta);
		for (unsigned char i = 0; i < aux->info->nrAnalize; i++)
			free(aux->info->denumiriAnalize[i]);
		free(aux->info->denumiriAnalize);
		free(aux->info);
		free(aux);
	}
}

void dezalocareHashT(hashT* tabela)
{
	for (unsigned char i = 0; i < tabela->nrElem; i++)
	{
		if (tabela->vect[i] != NULL)
			dezalocareLista(&tabela->vect[i]);
	}
	free(tabela->vect);
}

void dezalocareAltaLista(nodLista** cap)
{
	while (*cap)
	{
		nodLista* aux = *cap;
		*cap = (*cap)->next;
		free(aux->info.data);
		free(aux);
	}
}

void main()
{
	hashT tabela;
	nodLS* cap = NULL;
	nodLista* altCap = NULL;
	tabela.nrElem = DIM;
	const unsigned char data[] = "01-07-2023";
	const unsigned char analiza[] = "Hematocrit";
	tabela.vect = (nodLS**)malloc(tabela.nrElem * sizeof(nodLS*));
	for (unsigned char i = 0; i < tabela.nrElem; i++)
		tabela.vect[i] = NULL;
	citireFisier(tabela, "date.txt");
	traversareHashT(tabela);
	printf("\nIn data de %s au fost %hhu analize.", data, nrAnalizeInData(tabela, data));
	printf("\n\nTraversare lista cu analiza %s\n", analiza);
	HashT2LinkedList(&altCap, tabela, analiza);
	if (altCap == NULL)
	{
		printf("Nu exista analiza %s in hashT", analiza);
	}
	else traversareAltaLista(altCap);
	printf("\nNumarul pacientilor unici din lista determinata anterior este: %hhu", numarPacienti(altCap));
	dezalocareHashT(&tabela);
	dezalocareAltaLista(&altCap);
	_CrtDumpMemoryLeaks();
}
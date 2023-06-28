#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#pragma warning(disable:4996)
#define _CRTDBG_MAP_ALLOC
#define DIM 31
#define LINE_SIZE 128

typedef struct InfoComanda Comanda;
typedef struct TabelaDispersie hashT;
typedef struct ListaSimpla nodLS;
typedef struct DataCalendaristica Data;

struct DataCalendaristica
{
	unsigned char zi, luna;
	unsigned short an;
};

struct InfoComanda
{
	unsigned int id;
	Data data_lansare;
	Data data_livrare;
	char* nume;
	float suma;
};

struct ListaSimpla
{
	Comanda info;
	nodLS* next;
};

struct TabelaDispersie
{
	nodLS** vect;
	unsigned char nrElem;
};

char comparaDate(Data d1, Data d2)
{
	if (d1.an < d2.an) return -1;
	else if (d1.an > d2.an) return 1;
	else {
		if (d1.luna < d2.luna) return -1;
		else if (d1.luna > d2.luna) return 1;
		else {
			if (d1.zi < d2.zi) return -1;
			else if (d1.zi > d2.zi) return 1;
			else return 0;
		}
	}
}

unsigned char hash(hashT tabela, const unsigned char* numeClient)
{
	unsigned int valoare = 5381;
	unsigned char c;
	while (c = *numeClient++)
	{
		valoare = ((valoare << 5) + valoare) ^ c;
	}
	return valoare % tabela.nrElem;
}

void inserareLista(nodLS** cap, Comanda c)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info.data_lansare = c.data_lansare;
	nou->info.data_livrare = c.data_livrare;
	nou->info.id = c.id;
	nou->info.suma = c.suma;
	nou->info.nume = strdup(c.nume);
	nou->next = NULL;
	if (*cap != NULL)
	{
		nodLS* temp = *cap;
		while (temp->next)
			temp = temp->next;
		temp->next = nou;
	}
	else *cap = nou;
}

void inserareListaHashT(nodLS** cap, Comanda c)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = c;
	nou->next = *cap;
	*cap = nou;
}

void inserareTabela(hashT tabela, Comanda c)
{
	unsigned char pozitie = hash(tabela, c.nume);
	inserareListaHashT(&tabela.vect[pozitie], c);
}

void citireFisier(hashT* tabela, const unsigned char* numeFisier)
{
	tabela->nrElem = DIM;
	tabela->vect = (nodLS**)malloc(sizeof(nodLS*)*tabela->nrElem);
	for (unsigned char i = 0; i < tabela->nrElem; i++)
		tabela->vect[i] = NULL;
	FILE* f = fopen(numeFisier, "r");
	unsigned char nrComenzi;
	Comanda c;
	char buffer[LINE_SIZE];
	fscanf(f, "%hhu", &nrComenzi);
	for (unsigned char i = 0; i < nrComenzi; i++)
	{
		fscanf(f, "%u", &c.id);
		fscanf(f, "%hhu %hhu %hu", &c.data_lansare.zi, &c.data_lansare.luna, &c.data_lansare.an);
		fscanf(f, "%hhu %hhu %hu", &c.data_livrare.zi, &c.data_livrare.luna, &c.data_livrare.an);
		fscanf(f, " %[^\n]", buffer);
		c.nume = strdup(buffer);
		fscanf(f, "%f", &c.suma);
		inserareTabela(*tabela, c);
	}
}

void traversareLS(nodLS* cap)
{
	nodLS* temp = cap;
	while (temp)
	{
		printf("\nId comanda: %u\nNume client: %s\nData lansare: %hhu-%hhu-%hu\nData aprox. de livrare: %hhu-%hhu-%hu\nSuma: %.2f\n", temp->info.id, temp->info.nume, temp->info.data_lansare.zi, temp->info.data_lansare.luna, temp->info.data_lansare.an, temp->info.data_livrare.zi, temp->info.data_livrare.luna, temp->info.data_livrare.an, temp->info.suma);
		temp = temp->next;
	}
}

void traversareHashT(hashT tabela)
{
	for (unsigned char i = 0; i < tabela.nrElem; i++)
	{
		if(tabela.vect[i]!=NULL)
		{
			printf("\n\nPozitia %hhu\n\n", i);
			traversareLS(tabela.vect[i]);
		}
	}
}

unsigned char nrComenziClient(hashT tabela, const unsigned char* numeClient)
{
	unsigned char rez = 0;
	nodLS* temp = tabela.vect[hash(tabela, numeClient)];
	while (temp)
	{
		rez++;
		temp = temp->next;
	}
	return rez;
}

void modificaData(hashT tabela, Data data, const unsigned int idComanda)
{
	for(unsigned char i=0; i<tabela.nrElem; i++)
		if (tabela.vect[i])
		{
			nodLS* temp = tabela.vect[i];
			while (temp)
			{
				if (temp->info.id == idComanda)
				{
					temp->info.data_livrare = data;
				}
				temp = temp->next;
			}
		}
}

void afiseazaComandaCuId(hashT tabela, const unsigned int idComanda)
{
	for(unsigned char i=0; i<tabela.nrElem; i++)
		if (tabela.vect[i])
		{
			nodLS* temp = tabela.vect[i];
			while (temp)
			{
				if (temp->info.id == idComanda)
				{
					printf("\nId comanda: %u\nNume client: %s\nData lansare: %hhu-%hhu-%hu\nData aprox. de livrare: %hhu-%hhu-%hu\nSuma: %.2f\n", temp->info.id, temp->info.nume, temp->info.data_lansare.zi, temp->info.data_lansare.luna, temp->info.data_lansare.an, temp->info.data_livrare.zi, temp->info.data_livrare.luna, temp->info.data_livrare.an, temp->info.suma);
					break;
				}
				temp = temp->next;
			}
		}
}

void stergeDinLista(nodLS** cap, Data d)
{
	nodLS* temp = *cap, * prev = NULL;
	while (temp != NULL)
		if (comparaDate(temp->info.data_livrare, d) < 0)
		{
			if (temp == *cap)
			{
				*cap = temp->next;
			}
			else prev->next = temp->next;
			free(temp->info.nume);
			nodLS* deSters = temp;
			temp = temp->next;
			free(deSters);
		}
		else
		{
			prev = temp;
			temp = temp->next;
		}
}

void stergeComenziInainteDeData(hashT tabela, Data d)
{
	for(unsigned char i=0; i<tabela.nrElem; i++)
		if (tabela.vect[i])
		{
			stergeDinLista(&tabela.vect[i], d);
		}
}

void ListaPesteSuma(nodLS** cap, hashT tabela, const float suma)
{
	for(unsigned char i=0; i<tabela.nrElem; i++)
		if (tabela.vect[i])
		{
			nodLS* temp = tabela.vect[i];
			while (temp)
			{
				if (temp->info.suma > suma)
					inserareLista(cap, temp->info);
				temp = temp->next;
			}
		}
}

void dezalocareLista(nodLS** cap)
{
	while (*cap)
	{
		nodLS* temp = *cap;
		(*cap) = (*cap)->next;
		free(temp->info.nume);
		free(temp);
	}
}

void dezalocareHashT(hashT* tabela)
{
	for (unsigned char i = 0; i < tabela->nrElem; i++)
		if (tabela->vect[i])
			dezalocareLista(&tabela->vect[i]);
	free(tabela->vect);
	tabela->nrElem = -1;
}

void main()
{
	hashT tabela;
	const unsigned char numeClient[] = "Ana Albu";
	const unsigned int idComanda = 78901;
	const Data dataNoua = { 19, 8, 2023 };
	const Data dataSters = { 31,8,2023 };
	const float suma = 1000.0f;
	nodLS* capLista = NULL;
	citireFisier(&tabela, "date.txt");
	traversareHashT(tabela);
	printf("\n\nNumarul comenzilor intermediate de %s este: ", numeClient);
	printf("%hhu", nrComenziClient(tabela, numeClient));
	printf("\n\nComenzii cu id-ul %u (daca exista) ii va fi modificata data la %hhu-%hhu-%hu.\n", idComanda, dataNoua.zi, dataNoua.luna, dataNoua.an);
	modificaData(tabela, dataNoua, idComanda);
	afiseazaComandaCuId(tabela, idComanda);
	stergeComenziInainteDeData(tabela, dataSters);
	printf("\n\nTabela de dispersie dupa stergerea comenzilor dinainte de %hhu-%hhu-%hu\n\n", dataSters.zi, dataSters.luna, dataSters.an);
	traversareHashT(tabela);
	printf("\n\nLista comenzilor care au mai mult de %.2f valoare a comenzii\n\n", suma);
	ListaPesteSuma(&capLista, tabela, suma);
	traversareLS(capLista);
	dezalocareHashT(&tabela);
	dezalocareLista(&capLista);
	_CrtDumpMemoryLeaks();
}
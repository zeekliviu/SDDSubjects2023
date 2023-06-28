#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#pragma warning(disable:4996)
#define _CRTDBG_MAP_ALLOC
#define LINE_SIZE 128

typedef struct Host Host;
typedef struct ListaSimpla nodLS;
typedef struct ListaListe nodLP;

struct Host
{
	unsigned char id;
	char* nume;
	char* adresaIP;
	char* port;
	unsigned char memorie; // in GB
	unsigned short spatiuStocare; // in TB
	float pret;
};

struct ListaSimpla
{
	nodLP* info;
	nodLS* next;
};

struct ListaListe
{
	nodLS* vecini;
	Host info;
	float greutate;
	nodLP* next;
};

nodLP* creareNodLP(Host h, float greutate)
{
	nodLP* nou = (nodLP*)malloc(sizeof(nodLP));
	nou->info.id = h.id;
	nou->info.adresaIP = strdup(h.adresaIP);
	nou->info.nume = strdup(h.nume);
	nou->info.port = strdup(h.port);
	nou->info.memorie = h.memorie;
	nou->info.pret = h.pret;
	nou->info.spatiuStocare = h.spatiuStocare;
	nou->greutate = greutate;
	nou->vecini = NULL;
	nou->next = NULL;
	return nou;
}

void inserareLP(nodLP** cap, Host h, float greutate)
{
	nodLP* nou = creareNodLP(h, greutate);
	if (*cap)
	{
		nodLP* temp = *cap;
		while (temp->next)
			temp = temp->next;
		temp->next = nou;
	}
	else *cap = nou;
}

nodLS* creareNodLS(nodLP* info)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = info;
	nou->next = NULL;
	return nou;
}

void inserareLS(nodLS** cap, nodLP* info)
{
	nodLS* nou = creareNodLS(info);
	if (*cap)
	{
		nodLS* temp = *cap;
		while (temp->next)
			temp = temp->next;
		temp->next = nou;
	}
	else *cap = nou;
}

nodLP* cautareDupaID(nodLP* cap, int id)
{
	nodLP* temp = cap;
	while (temp && temp->info.id != id)
		temp = temp->next;
	return temp;
}

void inserareArcInGraf(nodLP* graf, int idStart, int idStop)
{
	if (graf)
	{
		nodLP* nodStart = cautareDupaID(graf, idStart);
		nodLP* nodStop = cautareDupaID(graf, idStop);
		if (nodStart && nodStop)
			inserareLS(&nodStart->vecini, nodStop);
	}
}

void traversareLS(nodLS* cap)
{
	nodLS* temp = cap;
	while (temp)
	{
		printf("\nID: %hhu\nNume: %s\nAdresa IP: %s\nPort: %s\nMemorie: %hhu\nSpatiu de stocare: %hu\nPret: %.2f\nGreutate cu nodul de sus: %.2f", temp->info->info.id, temp->info->info.nume, temp->info->info.adresaIP, temp->info->info.port, temp->info->info.memorie, temp->info->info.spatiuStocare, temp->info->info.pret, temp->info->greutate);
		temp = temp->next;
	}
}

void traversareLP(nodLP* cap)
{
	nodLP* temp = cap;
	while (temp)
	{
		printf("%d.%s are urmatorii vecini: \n", temp->info.id, temp->info.nume);
		temp = temp->next;
	}
}

void citireDinFisier(nodLP** graf, const unsigned char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	char buffer[LINE_SIZE];
	Host h;
	unsigned char nrHosturi;
	fscanf(f, "%hhu", &nrHosturi);
	for (unsigned char i = 0; i < nrHosturi; i++)
	{
		h.id = i;
		fscanf(f, " %[^\n]", buffer);
		h.nume = strdup(buffer);
		fscanf(f, "%s", buffer);
		h.adresaIP = strdup(buffer);
		fscanf(f, "%s", buffer);
		h.port = strdup(buffer);
		fscanf(f, "%hhu", &h.memorie);
		fscanf(f, "%hu", &h.spatiuStocare);
		fscanf(f, "%f", &h.pret);
		inserareLP(graf, h);
	}
}

void main()
{
	nodLP* graf;
	_CrtDumpMemoryLeaks();
}
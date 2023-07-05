#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#include <stdbool.h>
#include <math.h>
#pragma warning(disable:4996)
#define _CRTDBG_MAP_ALLOC
#define DIM 31
#define LINE_SIZE 256

typedef struct Rezervare Rezervare;
typedef struct TabelaDispersie hashT;
typedef struct Vector Vector;
typedef struct DataCalendaristica Data;

struct DataCalendaristica
{
	unsigned char day, month;
	unsigned short year;
};


struct Rezervare
{
	unsigned int id;
	char* denumire_hotel;
	unsigned char nr_camere_total;
	unsigned char nr_camere_rezervate;
	char* nume_client;
	char* perioada_rezervarii;
	Data inceput;
	Data sfarsit;
	float pret;
};


struct TabelaDispersie
{
	Rezervare** vect;
	unsigned char nrElem;
};

struct Vector
{
	Rezervare* vect;
	unsigned char nrElem;
};

unsigned char hash(hashT tabela, const char* denumire)
{
	unsigned int value = 5381;
	unsigned int c;
	while (c = *denumire++)
	{
		value = ((value << 7) + value) + c;
	}
	return value % tabela.nrElem;
}

void inserareHashT(hashT tabela, Rezervare* r)
{
	unsigned char pozitie = hash(tabela, r->denumire_hotel);
	if (tabela.vect[pozitie] == NULL)
	{
		tabela.vect[pozitie] = r;
	}
	else
	{
		bool flag = false;
		while(pozitie<tabela.nrElem)
			if (tabela.vect[++pozitie] == NULL)
			{
				tabela.vect[pozitie] = r;
				flag = true;
				break;
			}
		if (!flag)
		{
			pozitie = hash(tabela, r->denumire_hotel);
			while (pozitie >= 0)
			{
				if (tabela.vect[--pozitie] == NULL)
				{
					tabela.vect[pozitie] = r;
					flag = true;
					break;
				}
			}
		}
	}
}

void convertToDate(const char* dateString, Data* data)
{
	sscanf(dateString, "%hhu-%hhu-%hu", &data->day, &data->month, &data->year);
}

bool isWithinRange(Data resStart, Data resEnd, Data startDate, Data endDate) {
	// check if reservation start is within the range
	bool startWithinRange =
		(resStart.year > startDate.year || (resStart.year == startDate.year && resStart.month > startDate.month) ||
			(resStart.year == startDate.year && resStart.month == startDate.month && resStart.day >= startDate.day)) &&
		(resStart.year < endDate.year || (resStart.year == endDate.year && resStart.month < endDate.month) ||
			(resStart.year == endDate.year && resStart.month == endDate.month && resStart.day <= endDate.day));

	// check if reservation end is within the range
	bool endWithinRange =
		(resEnd.year > startDate.year || (resEnd.year == startDate.year && resEnd.month > startDate.month) ||
			(resEnd.year == startDate.year && resEnd.month == startDate.month && resEnd.day >= startDate.day)) &&
		(resEnd.year < endDate.year || (resEnd.year == endDate.year && resEnd.month < endDate.month) ||
			(resEnd.year == endDate.year && resEnd.month == endDate.month && resEnd.day <= endDate.day));

	return startWithinRange || endWithinRange;
}


void citireFisier(const char* numeFisier, hashT tabela)
{
	FILE* f = fopen(numeFisier, "r");
	unsigned char nrRezervari;
	
	char buffer[LINE_SIZE];
	fscanf(f, "%hhu", &nrRezervari);
	for (unsigned char i = 0; i < nrRezervari; i++)
	{
		Rezervare* r = (Rezervare*)malloc(sizeof(Rezervare));
		fscanf(f, "%u", &r->id);
		fscanf(f, " %[^\n]", buffer);
		r->denumire_hotel = strdup(buffer);
		fscanf(f, "%hhu", &r->nr_camere_total);
		fscanf(f, "%hhu", &r->nr_camere_rezervate);
		fscanf(f, " %[^\n]", buffer);
		r->nume_client = strdup(buffer);
		fscanf(f, " %[^\n]", buffer);
		r->perioada_rezervarii = strdup(buffer);
		char dataInceput[11];
		char dataSfarsit[11];
		dataInceput[0] = r->perioada_rezervarii[0];
		dataInceput[1] = r->perioada_rezervarii[1];
		dataInceput[2] = r->perioada_rezervarii[2];
		dataInceput[3] = r->perioada_rezervarii[3];
		dataInceput[4] = r->perioada_rezervarii[4];
		dataInceput[5] = r->perioada_rezervarii[5];
		dataInceput[6] = r->perioada_rezervarii[6];
		dataInceput[7] = r->perioada_rezervarii[7];
		dataInceput[8] = r->perioada_rezervarii[8];
		dataInceput[9] = r->perioada_rezervarii[9];
		dataInceput[10] = '\0';
		dataSfarsit[0] = r->perioada_rezervarii[13];
		dataSfarsit[1] = r->perioada_rezervarii[14];
		dataSfarsit[2] = r->perioada_rezervarii[15];
		dataSfarsit[3] = r->perioada_rezervarii[16];
		dataSfarsit[4] = r->perioada_rezervarii[17];
		dataSfarsit[5] = r->perioada_rezervarii[18];
		dataSfarsit[6] = r->perioada_rezervarii[19];
		dataSfarsit[7] = r->perioada_rezervarii[20];
		dataSfarsit[8] = r->perioada_rezervarii[21];
		dataSfarsit[9] = r->perioada_rezervarii[22];
		dataSfarsit[10] = '\0';
		convertToDate(dataInceput, &r->inceput);
		convertToDate(dataSfarsit, &r->sfarsit);
		fscanf(f, "%f", &r->pret);
		inserareHashT(tabela, r);
	}
	fclose(f);
}

void traversareHashT(hashT tabela)
{
	for (unsigned char i = 0; i < tabela.nrElem; i++)
	{
		if (tabela.vect[i] != NULL)
		{
			printf("Pozitie: %hhu\nId rezervare: %u\nDenumire hotel: %s\nNumar camere rezervate din total: %hhu/%hhu\nNume client: %s\nPerioada rezervarii: \nCheck-In: %hhu-%hhu-%hu\nCheck-Out: %hhu-%hhu-%hu\nPret: %.2f\n\n", i, tabela.vect[i]->id, tabela.vect[i]->denumire_hotel, tabela.vect[i]->nr_camere_rezervate, tabela.vect[i]->nr_camere_total, tabela.vect[i]->nume_client, tabela.vect[i]->inceput.day, tabela.vect[i]->inceput.month, tabela.vect[i]->inceput.year, tabela.vect[i]->sfarsit.day, tabela.vect[i]->sfarsit.month, tabela.vect[i]->sfarsit.year, tabela.vect[i]->pret);
		}
	}
}

float gradOcupare(hashT tabela, const char* numeHotel, const char* perioadaInceput, const char* perioadaSfarist)
{
	unsigned short totalOcupat = 0;
	Data perioadaInceputDate, perioadaSfarsitDate;
	convertToDate(perioadaInceput, &perioadaInceputDate);
	convertToDate(perioadaSfarist, &perioadaSfarsitDate);
	unsigned char pozitie = hash(tabela, numeHotel);
	for (unsigned char i = pozitie; i < tabela.nrElem; i++)
	{
		if (!strcmp(tabela.vect[i]->denumire_hotel, numeHotel)) {
			if (isWithinRange(tabela.vect[i]->inceput, tabela.vect[i]->sfarsit, perioadaInceputDate, perioadaSfarsitDate))
			{
				totalOcupat += tabela.vect[i]->nr_camere_rezervate;
			}
		}
	}
	return (float)totalOcupat / tabela.vect[pozitie]->nr_camere_total;
}

void HashT2Vector(hashT tabela, Vector* v, const char* numeClient)
{
	v->nrElem = 0;
	v->vect = NULL;
	for (unsigned char i = 0; i < tabela.nrElem; i++)
	{
		if (tabela.vect[i] != NULL)
		{
			if (!strcmp(tabela.vect[i]->nume_client, numeClient))
			{
				v->vect = (Rezervare*)realloc(v->vect, ++v->nrElem * sizeof(Rezervare));
				v->vect[v->nrElem - 1] = *(tabela.vect[i]);
				v->vect[v->nrElem - 1].denumire_hotel = strdup(tabela.vect[i]->denumire_hotel);
				v->vect[v->nrElem - 1].perioada_rezervarii = strdup(tabela.vect[i]->perioada_rezervarii);
				v->vect[v->nrElem - 1].nume_client = strdup(tabela.vect[i]->nume_client);
			}
		}
	}
}

void traversareVector(Vector v)
{
	if (v.nrElem == 0)
		return;
	for(unsigned char i=0; i<v.nrElem; i++)
		printf("\nId rezervare: %u\nDenumire hotel: %s\nNumar camere rezervate din total: %hhu/%hhu\nNume client: %s\nPerioada rezervarii: %s\nCheck-In: %hhu-%hhu-%hu\nCheck-Out: %hhu-%hhu-%hu\nPret: %.2f\n\n", v.vect[i].id, v.vect[i].denumire_hotel, v.vect[i].nr_camere_rezervate, v.vect[i].nr_camere_total, v.vect[i].nume_client, v.vect[i].perioada_rezervarii, v.vect[i].inceput.day, v.vect[i].inceput.month, v.vect[i].inceput.year, v.vect[i].sfarsit.day, v.vect[i].sfarsit.month, v.vect[i].sfarsit.year, v.vect[i].pret);
}
int calculateDaysBetween(Data date1, Data date2) {
	// calculate total days for date1
	int total1 = date1.year * 365 + date1.month * 30 + date1.day;

	// calculate total days for date2
	int total2 = date2.year * 365 + date2.month * 30 + date2.day;

	// return difference between two dates
	return abs(total2 - total1);
}

float incasariPerioada(Vector v, const char* perioadaInceput, const char* perioadaSfarsit)
{
	float total = 0.0f;
	Data perioadaInceputData, perioadaSfarsitData;
	convertToDate(perioadaInceput, &perioadaInceputData);
	convertToDate(perioadaSfarsit, &perioadaSfarsitData);
	for(unsigned char i=0; i<v.nrElem; i++)
		if (isWithinRange(v.vect[i].inceput, v.vect[i].sfarsit, perioadaInceputData, perioadaSfarsitData))
		{
			total += calculateDaysBetween(v.vect[i].inceput, v.vect[i].sfarsit) * v.vect[i].pret;
		}
	return total;
}

unsigned char nrCamere(hashT tabela, const char* numeClient)
{
	unsigned char total = 0;
	for (unsigned char i = 0; i < tabela.nrElem; i++)
	{
		if (tabela.vect[i] != NULL)
			if (!strcmp(tabela.vect[i]->nume_client, numeClient))
				total += tabela.vect[i]->nr_camere_rezervate;
	}
	return total;
}

void dezalocareHashT(hashT* tabela)
{
	for(unsigned char i=0; i<tabela->nrElem; i++)
		if (tabela->vect[i])
		{
			free(tabela->vect[i]->denumire_hotel);
			free(tabela->vect[i]->nume_client);
			free(tabela->vect[i]->perioada_rezervarii);
			free(tabela->vect[i]);
		}
	free(tabela->vect);
	tabela->nrElem = 0;
}

void dezalocareVector(Vector* v)
{
	for (unsigned char i = 0; i < v->nrElem; i++)
	{
		free(v->vect[i].denumire_hotel);
		free(v->vect[i].nume_client);
		free(v->vect[i].perioada_rezervarii);
	}
	free(v->vect);
	v->nrElem = 0;
}

void main()
{
	hashT tabela;
	Vector v;
	tabela.nrElem = DIM;
	tabela.vect = (Rezervare**)calloc(tabela.nrElem, sizeof(Rezervare*));
	const char numeHotel[] = "Hotel Transilvania";
	const char perioadaInceput[] = "01-07-2023";
	const char perioadaSfarsit[] = "11-08-2023";
	const char Inceput[] = "01-07-2023";
	const char Sfarsit[] = "08-07-2023";
	const char numeClient[] = "Ion Popescu";
	citireFisier("date.txt", tabela);
	traversareHashT(tabela);
	printf("\nGradul de ocupare al hotelului %s intre %s si %s a fost de %.2f%%", numeHotel, perioadaInceput, perioadaSfarsit, gradOcupare(tabela, numeHotel, perioadaInceput, perioadaSfarsit)*100);
	printf("\n\nRezervarile clientului cu numele %s sunt urmatoarele:\n", numeClient);
	HashT2Vector(tabela, &v, numeClient);
	traversareVector(v);
	printf("\nIncasarile hotelului din perioada %s - %s este %.2f RON.", Inceput, Sfarsit, incasariPerioada(v, Inceput,Sfarsit));
	printf("\n\nNumarul total de camere rezervate de %s a fost de %hhu.", numeClient, nrCamere(tabela, numeClient));
	dezalocareHashT(&tabela);
	dezalocareVector(&v);
	_CrtDumpMemoryLeaks();
}
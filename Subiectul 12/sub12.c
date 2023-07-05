#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#include <stdbool.h>
#pragma warning(disable:4996)
#define _CRTDBG_MAP_ALLOC
#define LINE_SIZE 128

typedef struct Fotografie Fotografie;
typedef struct ABC nodArb;
typedef struct Vector Vector;
typedef struct DataCalendaristica Data;

struct DataCalendaristica
{
	unsigned char zi, luna;
	unsigned short an;
};

struct Fotografie
{
	unsigned int idFotografie;
	char* locatie;
	Data data;
	float rezolutie; // 120 - 600 dpi
};

struct ABC
{
	Fotografie info;
	nodArb* st, * dr;
};

struct Vector
{
	Fotografie* vect;
	unsigned char nrElem;
};

nodArb* creareNod(Fotografie f, nodArb* st, nodArb* dr)
{
	nodArb* nou = (nodArb*)malloc(sizeof(nodArb));
	nou->st = st;
	nou->dr = dr;
	nou->info = f;
	return nou;
}

void inserareArbore(nodArb** rad, Fotografie foto)
{
	if (*rad)
	{
		if (foto.idFotografie > (*rad)->info.idFotografie)
			inserareArbore(&(*rad)->dr, foto);
		else if (foto.idFotografie < (*rad)->info.idFotografie)
			inserareArbore(&(*rad)->st, foto);
	}
	else *rad = creareNod(foto, NULL, NULL);
}

void citireFisier(nodArb** rad, const unsigned char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	unsigned char nrFotografii;
	fscanf(f, "%hhu", &nrFotografii);
	Fotografie foto;
	char buffer[LINE_SIZE];
	for (unsigned char i = 0; i < nrFotografii; i++)
	{
		fscanf(f, "%u", &foto.idFotografie);
		fscanf(f, " %[^\n]", buffer);
		foto.locatie = strdup(buffer);
		fscanf(f, "%hhu %hhu %hu", &foto.data.zi, &foto.data.luna, &foto.data.an);
		fscanf(f, "%f", &foto.rezolutie);
		inserareArbore(rad, foto);
	}
	fclose(f);
}

void inordine(nodArb* rad)
{
	if (rad)
	{
		inordine(rad->st);
		printf("Id fotografie: %u\nData: %hhu/%hhu/%hu\nLocatie: %s\nRezolutie: %.2f DPI\n\n", rad->info.idFotografie, rad->info.data.zi, rad->info.data.luna, rad->info.data.an, rad->info.locatie, rad->info.rezolutie);
		inordine(rad->dr);
	}
}

void nrFotografiiLaLocatia(const unsigned char* locatie, nodArb* rad, unsigned char* nrFotografii)
{
	if (rad)
	{
		nrFotografiiLaLocatia(locatie, rad->st, nrFotografii);
		if (!strcmp(rad->info.locatie, locatie))
			(*nrFotografii)++;
		nrFotografiiLaLocatia(locatie, rad->dr, nrFotografii);
	}
}

void modificaDataFotografieiCuData(const unsigned int idFotografie, Data dataNoua, nodArb* rad)
{
	if (rad)
	{
		modificaDataFotografieiCuData(idFotografie, dataNoua, rad->st);
		if (rad->info.idFotografie == idFotografie)
			rad->info.data = dataNoua;
		modificaDataFotografieiCuData(idFotografie, dataNoua, rad->dr);
	}
}

void stergeRad(nodArb** rad)
{
	nodArb* aux = *rad;
	if (aux->st && aux->dr)
	{
		nodArb* temp = aux->st;
		while (temp->dr)
			temp = temp->dr;
		temp->dr = aux->dr;
		*rad = aux->st;
	}
	else if (aux->st)
		*rad = aux->st;
	else if (aux->dr)
		*rad = aux->dr;
	else
		*rad = NULL;
	free(aux->info.locatie);
	free(aux);
}

void dezalocareArb(nodArb** rad)
{
	if (*rad)
	{
		dezalocareArb(&(*rad)->st);
		dezalocareArb(&(*rad)->dr);
		free((*rad)->info.locatie);
		free(*rad);
	}
}

Fotografie creareFotografie(Fotografie f)
{
	Fotografie noua = f;
	noua.locatie = strdup(f.locatie);
	return noua;
}

void populareVector(nodArb* rad, Vector* v, const float rezolutie)
{
	if (rad)
	{
		populareVector(rad->st, v, rezolutie);
		if (rad->info.rezolutie == rezolutie)
			v->vect = (Fotografie*)realloc(v->vect, sizeof(Fotografie) * (++v->nrElem)), v->vect[v->nrElem - 1] = creareFotografie(rad->info);
		populareVector(rad->dr, v, rezolutie);
	}
}

void traversareVector(Vector v)
{
	for (unsigned char i = 0; i < v.nrElem; i++)
		printf("Id fotografie: %u\nData: %hhu/%hhu/%hu\nLocatie: %s\nRezolutie: %.2f DPI\n\n", v.vect[i].idFotografie, v.vect[i].data.zi, v.vect[i].data.luna, v.vect[i].data.an, v.vect[i].locatie, v.vect[i].rezolutie);
}

void dezalocareVector(Vector* v)
{
	for (unsigned i = 0; i < v->nrElem; i++)
		free(v->vect[i].locatie);
	free(v->vect);
	v->nrElem = 0;
}

void main()
{
	nodArb* rad = NULL;
	Vector v;
	v.nrElem = 0;
	v.vect = NULL;
	const unsigned char locatie[] = "Eiffel Tower, Paris";
	const Data dataNoua = { 7,7,2007 };
	const unsigned int idPoza = 10112;
	const float rezolutie = 200.0f;
	unsigned char nrFotografii = 0;
	citireFisier(&rad, "date.txt");
	inordine(rad);
	nrFotografiiLaLocatia(locatie, rad, &nrFotografii);
	if (!nrFotografii)
	{
		printf("\n\nNu s-au facut fotografii la %s.", locatie);
	}
	else
	{
		printf("\n\nNr. de fotografii facute la %s este %hhu.", locatie, nrFotografii);
	}
	printf("\n\nModificarea datei pozei cu id-ul %u cu noua data: %hhu/%hhu/%hu\n\n", idPoza, dataNoua.zi, dataNoua.luna, dataNoua.an);
	modificaDataFotografieiCuData(idPoza, dataNoua, rad);
	inordine(rad);
	printf("\nS-a sters radacina! BST-ul modificat\n\n");
	stergeRad(&rad);
	inordine(rad);
	printf("\n\nVectorasul care are fotografiile cu rezolutia egala cu %.2f DPI\n\n", rezolutie);
	populareVector(rad, &v, rezolutie);
	traversareVector(v);
	dezalocareVector(&v);
	dezalocareArb(&rad);
	_CrtDumpMemoryLeaks();
}
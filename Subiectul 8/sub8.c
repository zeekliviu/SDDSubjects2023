#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <crtdbg.h>
#include <stdbool.h>
#pragma warning(disable:4996)
#define _CRTDBG_MAP_ALLOC
#define LINE_SIZE 128

typedef struct InfoHost Host;
typedef struct ListaListe nodLP;
typedef struct ListaSimpla nodLS;
typedef struct Vector Vector;
typedef struct Vector2 Vector2;

struct InfoHost
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
	Host info;
	float greutate;
	nodLS* next;
};

struct ListaListe
{
	Host nod;
	nodLS* info;
	nodLP* next;
};

struct Vector
{
	unsigned char nodSrc, nodDst;
};

struct Vector2
{
	Host* vect;
	unsigned char nrElem;
};

nodLP* creareNodLP(Host h)
{
	nodLP* nou = (nodLP*)malloc(sizeof(nodLP));
	nou->info = NULL;
	nou->next = NULL;
	nou->nod = h;
	return nou;
}

void inserareListaAdiacenta(nodLP** cap, Host h)
{
	nodLP* nou = creareNodLP(h);
	if (*cap == NULL)
		*cap = nou;
	else
	{
		nodLP* temp = *cap;
		while (temp->next)
			temp = temp->next;
		temp->next = nou;
	}
}

nodLP* gasesteNodPrincipal(nodLP* cap, unsigned char id)
{
	nodLP* temp = cap;
	while (temp)
	{
		if (temp->nod.id == id)
			return temp;
		temp = temp->next;
	}
}

Host gasesteInfoNod(nodLP* cap, unsigned char id)
{
	Host h;
	nodLP* temp = cap;
	while (temp)
	{
		if (temp->nod.id == id)
		{
			h.adresaIP = strdup(temp->nod.adresaIP);
			h.id = id;
			h.memorie = temp->nod.memorie;
			h.spatiuStocare = temp->nod.spatiuStocare;
			h.nume = strdup(temp->nod.nume);
			h.port = strdup(temp->nod.port);
			h.pret = temp->nod.pret;
			break;
		}
		temp = temp->next;
	}
	return h;
}

nodLS* creareNodLS(Host h, float greutate)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->info = h;
	nou->greutate = greutate;
	nou->next = NULL;
	return nou;
}

void insereazaListaSimpla(nodLS** cap, Host h, float greutate)
{
	nodLS* nou = creareNodLS(h, greutate);
	if (*cap == NULL)
	{
		*cap = nou;
	}
	else
	{
		nodLS* temp = *cap;
		while (temp->next)
			temp = temp->next;
		temp->next = nou;
	}
}

void insereazaVecin(nodLP* cap, unsigned char nodStart, unsigned char nodStop, float greutate)
{
	nodLP* nodPrincipal = gasesteNodPrincipal(cap, nodStart);
	Host infoNod = gasesteInfoNod(cap, nodStop);
	insereazaListaSimpla(&nodPrincipal->info, infoNod, greutate);
}

void citireFisier(nodLP** cap, const char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	if (!f)
	{
		printf("Fisier inexistent!");
		exit(-1);
	}
	*cap = NULL;
	char buffer[LINE_SIZE];
	Host h;
	unsigned char nrNoduri;
	fscanf(f, "%hhu", &nrNoduri);
	for (unsigned char i = 0; i < nrNoduri; i++)
	{
		h.id = i+1;
		fscanf(f, " %[^\n]", buffer);
		h.nume = strdup(buffer);
		fscanf(f, "%s", buffer);
		h.adresaIP = strdup(buffer);
		fscanf(f, "%s", buffer);
		h.port = strdup(buffer);
		fscanf(f, "%hhu", &h.memorie);
		fscanf(f, "%hu", &h.spatiuStocare);
		fscanf(f, "%f", &h.pret);
		inserareListaAdiacenta(cap, h);
	}
	unsigned char nrMuchii;
	unsigned char nodStart, nodStop;
	float greutate;
	fscanf(f, "%hhu", &nrMuchii);
	for (unsigned char i = 0; i < nrMuchii; i++)
	{
		fscanf(f, "%hhu %hhu %f", &nodStart, &nodStop, &greutate);
		insereazaVecin(*cap, nodStart, nodStop, greutate);
	}
	fclose(f);
}

void traversareListaAdiacenta(nodLP* cap)
{
	nodLP* temp = cap;
	while (temp)
	{
		printf("\n\nVecinii nodului %hhu sunt: \n", temp->nod.id);
		nodLS* aux = temp->info;
		while (aux)
		{
			printf("\n%hhu avand greutatea %.2f", aux->info.id, aux->greutate);
			aux = aux->next;
		}
		temp = temp->next;
	}
}

void LL2Vector(nodLP* cap, Vector** v, unsigned char* dim, const float prag)
{
	nodLP* temp = cap;
	*v = NULL;
	while (temp)
	{
		nodLS* aux = temp->info;
		while (aux)
		{
			if (aux->greutate > prag) {
				*v = (Vector*)realloc(*v, sizeof(Vector) * (++(*dim)));
				(*v)[(*dim) - 1].nodSrc = temp->nod.id, (*v)[(*dim)-1].nodDst = aux->info.id;
			}
			aux = aux->next;
		}
		temp = temp->next;
	}
}

void traversareVectorArce(Vector* v, const unsigned char dim)
{
	for (unsigned char i = 0; i < dim; i++)
		printf("\n[%hhu - %hhu]", v[i].nodSrc, v[i].nodDst);
}

Host DeepCopy(Host h)
{
	Host nou;
	nou.adresaIP = strdup(h.adresaIP);
	nou.id = h.id;
	nou.memorie = h.memorie;
	nou.pret = h.pret;
	nou.port = strdup(h.port);
	nou.nume = strdup(h.nume);
	nou.spatiuStocare = h.spatiuStocare;
	return nou;
}

void LL2Vector2(nodLP* cap, Vector2* w, const char* substr)
{
	w->vect = NULL;
	w->nrElem = 0;
	nodLP* temp = cap;
	while (temp)
	{
		if (strstr(temp->nod.nume, substr))
		{
			w->vect = (Host*)realloc(w->vect, sizeof(Host) * (++w->nrElem));
			w->vect[w->nrElem - 1] = DeepCopy(temp->nod);
		}
		temp = temp->next;
	}
}

void traversareVector2(Vector2 w)
{
	for (unsigned char i = 0; i < w.nrElem; i++)
		printf("\nHost id: %hhu\nNume: %s\nAdresa: %s:%s\nMemorie: %hhu GB\nSpatiu de stocare %hu TB\nPret: %.2f RON\n\n", w.vect[i].id, w.vect[i].nume, w.vect[i].adresaIP, w.vect[i].port, w.vect[i].memorie, w.vect[i].spatiuStocare, w.vect[i].pret);
}

float greutateMinima(nodLP* cap)
{
	nodLP* temp = cap;
	float min = temp->info->greutate;
	while (temp)
	{
		nodLS* aux = temp->info;
		while (aux)
		{
			if (aux->greutate < min)
				min = aux->greutate;
			aux = aux->next;
		}
		temp = temp->next;
	}
	return min;
}

void Cerinta4(nodLP* cap, Vector2* w)
{
	float min = greutateMinima(cap);
	for (unsigned char i = 0; i < w->nrElem; i++)
	{
		nodLP* aux = gasesteNodPrincipal(cap, w->vect[i].id);
		nodLS* temp = aux->info;
		bool flag = false;
		while (temp)
		{
			if (temp->greutate == min)
			{
				flag = true;
				break;
			}
			temp = temp->next;
		}
		if (flag)
		{
			Host aux = w->vect[i];
			w->vect[i] = w->vect[w->nrElem - 1];
			w->vect[w->nrElem - 1] = aux;
			free(w->vect[w->nrElem - 1].adresaIP);
			free(w->vect[w->nrElem - 1].nume);
			free(w->vect[w->nrElem - 1].port);
			w->vect = (Host*)realloc(w->vect, sizeof(Host) * (--w->nrElem));
			i--;
		}
	}
}

void dezalocareVector(Vector** v, unsigned char* dim)
{
	free((*v));
	*v = NULL;
	*dim = 0;
}

void dezalocareVector2(Vector2* v)
{
	for (unsigned char i = 0; i < v->nrElem; i++)
		free(v->vect[i].adresaIP), free(v->vect[i].port), free(v->vect[i].nume);
	free(v->vect);
	v->nrElem = 0;
}

void dezalocareListaSimpla(nodLS** cap)
{
	while (*cap)
	{
		nodLS* temp = *cap;
		*cap = (*cap)->next;
		free(temp->info.adresaIP);
		free(temp->info.nume);
		free(temp->info.port);
		free(temp);
		temp = NULL;
	}
}

void dezalocareListaAdiacente(nodLP** cap)
{
	while (*cap)
	{
		nodLP* temp = *cap;
		*cap = (*cap)->next;
		dezalocareListaSimpla(&temp->info);
		free(temp->nod.adresaIP);
		free(temp->nod.nume);
		free(temp->nod.port);
		free(temp);
		temp = NULL;
	}
}

void main()
{
	nodLP* listaAdiacenta;
	Vector* v;
	Vector2 w;
	unsigned char dim = 0;
	const char* numeFisier = "date.txt";
	const float prag = 13.69f;
	const char* contine = "AI";
	citireFisier(&listaAdiacenta, numeFisier);
	traversareListaAdiacenta(listaAdiacenta);
	LL2Vector(listaAdiacenta, &v, &dim, prag);
	if (dim)
	{
		printf("\n\nArcele care au ponderea peste %.2f sunt:", prag);
		traversareVectorArce(v, dim);
	}
	else
	{
		printf("\n\nNu exista arce cu ponderea peste %.2f!\n", prag);
	}
	LL2Vector2(listaAdiacenta, &w, contine);
	if (!w.nrElem)
	{
		printf("\n\nNu exista hosturi care sa contina %s\n\n", contine);
	}
	else
	{
		printf("\n\nHosturile care contin substringul %s\n\n", contine);
		traversareVector2(w);
	}
	printf("\n\nVector modificat:\n\n");
	Cerinta4(listaAdiacenta, &w);
	traversareVector2(w);
	dezalocareListaAdiacente(&listaAdiacenta);
	dezalocareVector(&v, &dim);
	dezalocareVector2(&w);
	_CrtDumpMemoryLeaks();
}
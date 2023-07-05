#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#include <stdbool.h>
#pragma warning(disable:4996)
#define _CRTDBG_MAP_ALLOC
#define LINE_SIZE 128

typedef struct InfoConsultatie Consultatie;
typedef struct ListaSimpla nodLS;
typedef struct ListaListe nodLP;
typedef struct Vector Vector;

struct InfoConsultatie
{
	char* data;
	char* nume;
	char* specialitate;
	char* diagnostic;
	int pret;
};

struct ListaSimpla
{
	Consultatie c;
	nodLS* next;
};

struct ListaListe
{
	nodLS* info;
	nodLP* next;
};

struct Vector
{
	Consultatie* vect;
	unsigned char nrElem;
};


nodLP* creareNodLP()
{
	nodLP* nou = (nodLP*)malloc(sizeof(nodLP));
	nou->info = NULL;
	nou->next = NULL;
	return nou;
}

nodLS* cautaInListaListe(nodLP* cap, const unsigned char* specialitate)
{
	nodLP* aux = cap;
	while (aux)
	{
		if (!strcmp(aux->info->c.specialitate, specialitate))
				return aux->info;
		aux = aux->next;
	}
	return NULL;
}

nodLS* creareNodListaSimpla(Consultatie c)
{
	nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
	nou->c = c;
	nou->next = NULL;
	return nou;
}

void inserareNodListaSimpla(nodLS** cap, Consultatie c)
{
	if (*cap == NULL)
		*cap = creareNodListaSimpla(c);
	else
	{
		nodLS* temp = *cap;
		while (temp->next)
			temp = temp->next;
		temp->next = creareNodListaSimpla(c);
	}
}

void inserareListaListe(nodLP** cap, Consultatie c)
{
	if (*cap == NULL)
	{
		*cap = creareNodLP();
		inserareNodListaSimpla(&(*cap)->info, c);
	}
	else
	{
		nodLS* gasit = cautaInListaListe(*cap, c.specialitate);
		if (!gasit)
		{
			nodLP* temp = *cap;
			while (temp->next)
				temp = temp->next;
			temp->next = creareNodLP();
			inserareNodListaSimpla(&temp->next->info, c);
		}
		else
		{
			inserareNodListaSimpla(&gasit, c);
		}
	}
}


unsigned int valoareTotala(nodLP* cap, const char* numeSpecialitate, bool* flag)
{
	unsigned int total = 0;
	nodLS* gasit = cautaInListaListe(cap, numeSpecialitate);
	if (!gasit)
	{
		*flag = false;
		return total;
	}
	while (gasit)
	{
		total += gasit->c.pret;
		gasit = gasit->next;
	}
	*flag = true;
	return total;
}

void citireFisier(nodLP** cap, const unsigned char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	if (!f)
		exit(-1);
	unsigned char nrConsultatii;
	char buffer[LINE_SIZE];
	Consultatie c;
	fscanf(f, "%hhu", &nrConsultatii);
	for (unsigned char i = 0; i < nrConsultatii; i++)
	{
		fscanf(f, "%s", buffer);
		c.data = strdup(buffer);
		fscanf(f, " %[^\n]", buffer);
		c.nume = strdup(buffer);
		fscanf(f, " %[^\n]", buffer);
		c.specialitate = strdup(buffer);
		fscanf(f, " %[^\n]", buffer);
		c.diagnostic = strdup(buffer);
		fscanf(f, "%d", &c.pret);
		inserareListaListe(cap, c);
	}
	fclose(f);
}

void traversareLS(nodLS* cap)
{
	nodLS* temp = cap;
	while (temp)
	{
		printf("Data: %s\nNume: %s\nSpecialitate: %s\nDiagnostic: %s\nPret: %d RON\n\n", temp->c.data, temp->c.nume, temp->c.specialitate, temp->c.diagnostic, temp->c.pret);
		temp = temp->next;
	}
}

void traversareLP(nodLP* cap)
{
	nodLP* temp = cap;
	unsigned char i = 0;
	while (temp)
	{
		if(temp->info)
		{
			printf("Sublista %hhu\n\n", ++i);
			traversareLS(temp->info);
		}
		temp = temp->next;
	}
}

nodLS* modificaPret(nodLP* cap, const char* dataConsult, const int pretNou)
{
	nodLP* aux = cap;
	while (aux)
	{
		nodLS* temp = aux->info;
		while (temp)
		{
			if (!strcmp(temp->c.data, dataConsult))
			{
				temp->c.pret = pretNou;
				return temp;
			}
			temp = temp->next;
		}
		aux = aux->next;
	}
	return NULL;
}

void stergeConsultatiiCuDiagnostic(nodLP* cap, const char* diagnostic)
{
	nodLP* temp = cap;
	while (temp)
	{
		nodLS* aux = temp->info, * prev = NULL;
		while (aux != NULL)
		{
			if (!strcmp(aux->c.diagnostic, diagnostic))
			{
				if (aux == temp->info)
				{
					temp->info = aux->next;
				}
				else
				{
					prev->next = aux->next;
				}
				free(aux->c.data);
				free(aux->c.diagnostic);
				free(aux->c.nume);
				free(aux->c.specialitate);
				free(aux);
				if (prev != NULL)
				{
					aux = prev->next;
				}
				else
				{
					aux = temp->info;
				}
			}
			else
			{
				prev = aux;
				aux = aux->next;
			}
		}
		temp = temp->next;
	}
}

Consultatie DeepCopy(Consultatie c)
{
	Consultatie copie = c;
	copie.diagnostic = strdup(c.diagnostic);
	copie.nume = strdup(c.nume);
	copie.specialitate = strdup(c.specialitate);
	copie.data = strdup(c.data);
	return copie;
}

void populareVector(nodLP* cap, Vector* v, const int prag)
{
	nodLP* temp = cap;
	while (temp)
	{
		nodLS* aux = temp->info;
		while (aux)
		{
			if (aux->c.pret > prag) {
				v->vect = (Consultatie*)realloc(v->vect, sizeof(Consultatie) * (++v->nrElem));
				v->vect[v->nrElem-1] = DeepCopy(aux->c);
			}
			aux = aux->next;
		}
		temp = temp->next;
	}
}

void traversareVector(Vector v)
{
	for (unsigned char i = 0; i < v.nrElem; i++)
		printf("Data: %s\nNume: %s\nSpecialitate: %s\nDiagnostic: %s\nPret: %d RON\n\n", v.vect[i].data, v.vect[i].nume, v.vect[i].specialitate, v.vect[i].diagnostic, v.vect[i].pret);
}

void dezalocare(Vector* v)
{
	for (unsigned char i = 0; i < v->nrElem; i++)
	{
		free(v->vect[i].data);
		free(v->vect[i].diagnostic);
		free(v->vect[i].nume);
		free(v->vect[i].specialitate);
	}
	free(v->vect);
	v->nrElem = 0;
}

void dezalocareListaSimpla(nodLS** cap)
{
	while (*cap)
	{
		nodLS* aux = *cap;
		*cap = (*cap)->next;
		free(aux->c.data);
		free(aux->c.diagnostic);
		free(aux->c.specialitate);
		free(aux->c.nume);
		free(aux);
	}
}

void dezalocareListaListe(nodLP** cap)
{
	while (*cap)
	{
		nodLP* aux = *cap;
		*cap = (*cap)->next;
		dezalocareListaSimpla(&aux->info);
		free(aux);
	}
}

void main()
{
	nodLP* cap = NULL;
	Vector v;
	v.nrElem = 0;
	v.vect = NULL;
	const unsigned char numeFisier[] = "date.txt";
	const char numeSpecialitate[] = "Ortopedie si Traumatologie";
	const char diagnostic[] = "Tensiune arteriala marita";
	const int pretNou = 699;
	const char dataConsultatie[] = "21/09/2023";
	const int prag = 250;
	bool flag;
	int valoareTotalaSpecialitate;
	citireFisier(&cap, numeFisier);
	traversareLP(cap);
	valoareTotalaSpecialitate = valoareTotala(cap, numeSpecialitate, &flag);
	if (!flag)
	{
		printf("\n\nSpecialitatea %s nu a fost gasita in lista de liste\n", numeSpecialitate);
	}
	else
	{
		printf("\n\nSpecialitatea %s insumeaza %d RON.\n\n", numeSpecialitate, valoareTotalaSpecialitate);
	}
	nodLS* rez = modificaPret(cap, dataConsultatie, pretNou);
	if (!rez)
	{
		printf("\n\nNicio consultatie nu a avut loc la data de %s\n", dataConsultatie);
	}
	else
	{
		printf("\n\nConsultatia din data de %s a fost actualizata cu noul pret %d RON.\n", dataConsultatie, pretNou);
		printf("\n\nConsultatia afectata:\n\n");
		printf("Data: %s\nNume: %s\nSpecialitate: %s\nDiagnostic: %s\nPret: %d RON\n\n", rez->c.data, rez->c.nume, rez->c.specialitate, rez->c.diagnostic, rez->c.pret);
	}
	stergeConsultatiiCuDiagnostic(cap, diagnostic);
	printf("\n\n\n");
	traversareLP(cap);
	populareVector(cap, &v, prag);
	printf("\n\n\nVectorul cu valori peste %d RON:\n\n\n", prag);
	traversareVector(v);
	dezalocareListaListe(&cap);
	dezalocare(&v);
	_CrtDumpMemoryLeaks();
}
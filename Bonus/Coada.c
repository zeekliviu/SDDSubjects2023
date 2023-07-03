#include <stdio.h>
#include <crtdbg.h>
#include <string.h>
#include <malloc.h>
#pragma warning(disable:4996)

typedef struct InfoStudent Student;
typedef struct Coada nodC;

struct InfoStudent
{
	char* nume;
	unsigned char varsta;
	float medie;
};

struct Coada
{
	Student info;
	nodC* next, * prev;
};

nodC* creareNodCoada(Student s)
{
	nodC* nou = (nodC*)malloc(sizeof(nodC));
	nou->info = s;
	nou->next = NULL;
	nou->prev = NULL;
	return nou;
}

void put(nodC** cap, nodC** coada, Student s)
{
	nodC* nou = creareNodCoada(s);
	if (*coada == NULL)
	{
		*cap = *coada = nou;
	}
	else
	{
		nou->next = *cap;
		(*cap)->prev = nou;
		*cap = nou;
	}
}

Student DeepCopy(Student s)
{
	Student nou;
	nou = s;
	nou.nume = strdup(s.nume);
	return nou;
}

Student* get(nodC** cap, nodC** coada)
{
	if (*cap == NULL || *coada == NULL)
	{
		return NULL;
	}
	Student* s = (Student*)malloc(sizeof(Student));
	if (*cap == *coada)
	{
		*s = DeepCopy((*coada)->info);
		free(*coada);
		*cap = *coada = NULL;
	}
	else
	{
		nodC* aux = *coada;
		*coada = (*coada)->prev;
		*s = DeepCopy(aux->info);
		free(aux);
	}
	return s;
}

void dezalocareCoada(nodC** cap, nodC** coada)
{
	Student* s;
	while (s = get(cap, coada))
	{
		printf("Studentul %s avand media %.2f si varsta %hhu a iesit din coada\n\n", s->nume, s->medie, s->varsta);
		free(s->nume);
		free(s);
	}
}

void main()
{
	Student s1 = { "Eliza", 21, 9.35f };
	Student s2 = { "Liviu", 22, 9.37f };
	Student s3 = { "Gabi", 20, 8.93f };
	Student s4 = { "Ardeu", 20, 7.84f };
	Student studenti[] = { s1,s2,s3,s4 };
	nodC* cap = NULL, * coada = NULL;
	for(unsigned char i=0; i<4; i++)
		put(&cap, &coada, studenti[i]);
	dezalocareCoada(&cap, &coada);
	_CrtDumpMemoryLeaks();
}
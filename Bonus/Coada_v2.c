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
	nodC* next;
};

nodC* creareNodCoada(Student s)
{
	nodC* nou = (nodC*)malloc(sizeof(nodC));
	nou->info = s;
	nou->next = NULL;
	return nou;
}

Student DeepCopy(Student s)
{
	Student nou;
	nou = s;
	nou.nume = strdup(s.nume);
	return nou;
}

void put(nodC** prim, nodC** ultim, Student s)
{
	if (*prim == NULL && *ultim == NULL)
	{
		*prim = *ultim = creareNodCoada(s);
	}
	else
	{
		nodC* nou = creareNodCoada(s);
		(*ultim)->next = nou;
		*ultim = nou;
	}
}

Student* get(nodC** prim, nodC** ultim)
{
	if (*prim == NULL)
	{
		*ultim = NULL;
		return NULL;
	}
	else
	{
		Student* s = (Student*)malloc(sizeof(Student));
		*s = DeepCopy((*prim)->info);
		nodC* aux = *prim;
		*prim = (*prim)->next;
		free(aux);
		return s;
	}
}

void dezalocareCoada(nodC** prim, nodC** ultim)
{
	Student* s;
	while (s = get(prim, ultim))
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
	nodC* prim = NULL, * ultim = NULL;
	for (unsigned char i = 0; i < 4; i++)
		put(&prim, &ultim, studenti[i]);
	dezalocareCoada(&prim, &ultim);
	_CrtDumpMemoryLeaks();
}
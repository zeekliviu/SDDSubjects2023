#include <stdio.h>
#include <malloc.h>
#include <crtdbg.h>
#include <string.h>
#pragma warning(disable:4996)

typedef struct InfoStudent Student;
typedef struct Stiva nodS;

struct InfoStudent
{
	char* nume;
	unsigned char varsta;
	float medie;
};

struct Stiva
{
	Student info;
	nodS* next;
};

nodS* creareNodStiva(Student s)
{
	nodS* nou = (nodS*)malloc(sizeof(nodS));
	nou->info = s;
	nou->next = NULL;
	return nou;
}

void push(nodS** cap, Student s)
{
	if (*cap == NULL)
	{
		*cap = creareNodStiva(s);
	}
	else
	{
		nodS* nou = creareNodStiva(s);
		nou->next = *cap;
		*cap = nou;
	}
}

Student DeepCopy(Student s)
{
	Student nou = s;
	nou.nume = strdup(s.nume);
	return nou;
}

Student* pop(nodS** cap)
{
	if (*cap == NULL)
		return NULL;
	else
	{
		Student* s = (Student*)malloc(sizeof(Student));
		*s = DeepCopy((*cap)->info);
		nodS* aux = *cap;
		*cap = (*cap)->next;
		free(aux);
		return s;
	}
}

void dezalocareStiva(nodS** cap)
{
	Student* s;
	while (s = pop(cap))
	{
		printf("Studentul %s avand media %.2f si varsta %hhu a iesit din stiva\n\n", s->nume, s->medie, s->varsta);
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
	nodS* cap = NULL;
	for (unsigned char i = 0; i < 4; i++)
		push(&cap, studenti[i]);
	dezalocareStiva(&cap);
	_CrtDumpMemoryLeaks();
}
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#pragma warning(disable:4996)
#define LINE_SIZE 128
#define _CRTDBG_MAP_ALLOC

typedef struct Data Data;
typedef struct InfoTask Task;
typedef struct ArboreBinar nodArb;
typedef struct ListaDubla nodLS;
typedef struct Vector Vector;

struct Data
{
    unsigned char zi;
    unsigned char luna;
    unsigned short an;
};

struct InfoTask
{
    unsigned int idTask;
    char* numeTask;
    char* descriereTask;
    char* responsabilTask;
    Data dataStart;
    Data dataFinalizare;
    float timpEstimat; // in ore
};

struct ArboreBinar
{
    Task info;
    nodArb* st, * dr;
    char BF;
};

struct ListaDubla
{
    Task info;
    nodLS* next, * prev;
};

struct Vector
{
    Task* vect;
    unsigned char nrElem;
};

nodArb* creareNod(Task t, nodArb* st, nodArb* dr)
{
    nodArb* nou = (nodArb*)malloc(sizeof(nodArb));
    nou->info = t;
    nou->st = st;
    nou->dr = dr;
    return nou;
}

char maxim(char a, char b)
{
    return a > b ? a : b;
}

unsigned char nrNiveluri(nodArb* rad)
{
    if (!rad)
        return 0;
    return 1 + maxim(nrNiveluri(rad->st), nrNiveluri(rad->dr));
}

void calculBF(nodArb* rad)
{
    if (rad)
    {
        rad->BF = nrNiveluri(rad->dr) - nrNiveluri(rad->st);
        calculBF(rad->st);
        calculBF(rad->dr);
    }
}

void rotatie_dreapta(nodArb** rad)
{
    printf("\n\nRotatie dreapta la %f\n\n", (*rad)->info.timpEstimat);
    nodArb* aux = (*rad)->st;
    (*rad)->st = (*rad)->st->dr;
    (*rad)->st->dr = *rad;
    *rad = aux;
    calculBF(*rad);
}

void rotatie_stanga(nodArb** rad)
{
    printf("\n\nRotatie stanga la %f\n\n", (*rad)->info.timpEstimat);
    nodArb* aux = (*rad)->dr;
    (*rad)->dr = (*rad)->dr->st;
    (*rad)->dr->st = *rad;
    *rad = aux;
    calculBF(*rad);
}

void rotatie_dreapta_stanga(nodArb** rad)
{
    printf("\n\nRotatie dreapta-stanga la %f\n\n", (*rad)->info.timpEstimat);
    rotatie_dreapta(&((*rad)->dr));
    rotatie_stanga(rad);
}

void rotatie_stanga_dreapta(nodArb** rad)
{
    printf("\n\nRotatie stanga-dreapta la %f\n\n", (*rad)->info.timpEstimat);
    rotatie_stanga(&((*rad)->st));
    rotatie_dreapta(rad);
}

void reechilibrare(nodArb** rad)
{
    if (!(*rad)) return;

    calculBF(*rad);
    if ((*rad)->BF == -2)  // left heavy
    {
        if ((*rad)->st->BF <= 0) // left child is left heavy or balanced
            rotatie_dreapta(rad);
        else if ((*rad)->st->BF == 1) // left child is right heavy
            rotatie_stanga_dreapta(rad);
    }
    else if ((*rad)->BF == 2) // right heavy
    {
        if ((*rad)->dr->BF >= 0) // right child is right heavy or balanced
            rotatie_stanga(rad);
        else if ((*rad)->dr->BF == -1) // right child is left heavy
            rotatie_dreapta_stanga(rad);
    }
}

void afisareNivel(nodArb* rad, int level)
{
    if (rad == NULL)
        return;
    if (level == 1)
        printf("Timp estimat: %.2f\nID Task: %u\nNume task: %s\nDescriere task: %s\nResponsabil task: %s\nData start: %hhu/%hhu/%hu\nData stop: %hhu/%hhu/%hu\n\n", rad->info.timpEstimat, rad->info.idTask, rad->info.numeTask, rad->info.descriereTask, rad->info.responsabilTask, rad->info.dataStart.zi, rad->info.dataStart.luna, rad->info.dataStart.an, rad->info.dataFinalizare.zi, rad->info.dataFinalizare.luna, rad->info.dataFinalizare.an);
    else if (level > 1)
    {
        afisareNivel(rad->st, level - 1);
        afisareNivel(rad->dr, level - 1);
    }
}

/* traversare arbore pe niveluri */
void traversareNiveluri(nodArb* rad)
{
    int h = nrNiveluri(rad);
    for (int i = 1; i <= h; i++)
        printf("\n\nNivel %i\n\n", i), afisareNivel(rad, i);
}

void inserareArb(nodArb** rad, Task t)
{
    if (*rad)
    {
        if (t.timpEstimat > (*rad)->info.timpEstimat)
            inserareArb(&(*rad)->dr, t);
        else if (t.timpEstimat < (*rad)->info.timpEstimat)
            inserareArb(&(*rad)->st, t);
        reechilibrare(rad);
    }
    else *rad = creareNod(t, NULL, NULL);
}


void inordine(nodArb* rad)
{
    if (rad)
    {
        inordine(rad->st);
        printf("Timp estimat: %.2f\nID Task: %u\nNume task: %s\nDescriere task: %s\nResponsabil task: %s\nData start: %hhu/%hhu/%hu\nData stop: %hhu/%hhu/%hu\n\n", rad->info.timpEstimat, rad->info.idTask, rad->info.numeTask, rad->info.descriereTask, rad->info.responsabilTask, rad->info.dataStart.zi, rad->info.dataStart.luna, rad->info.dataStart.an, rad->info.dataFinalizare.zi, rad->info.dataFinalizare.luna, rad->info.dataFinalizare.an);
        inordine(rad->dr);
    }
}

void citireFisier(nodArb** rad, const char* numeFisier)
{
    FILE* f = fopen(numeFisier, "r");
    if (!f)
    {
        printf("\nFisierul nu a fost gasit!");
        exit(-1);
    }
    unsigned char nrTaskuri;
    fscanf(f, "%hhu", &nrTaskuri);
    char buffer[LINE_SIZE];
    Task t;
    *rad = NULL;
    for (unsigned char i = 0; i < nrTaskuri; i++)
    {
        fscanf(f, "%u", &t.idTask);
        fscanf(f, " %[^\n]", buffer);
        t.numeTask = strdup(buffer);
        fscanf(f, " %[^\n]", buffer);
        t.descriereTask = strdup(buffer);
        fscanf(f, "%s", buffer);
        t.responsabilTask = strdup(buffer);
        fscanf(f, "%hhu %hhu %hu", &t.dataStart.zi, &t.dataStart.luna, &t.dataStart.an);
        fscanf(f, "%hhu %hhu %hu", &t.dataFinalizare.zi, &t.dataFinalizare.luna, &t.dataFinalizare.an);
        fscanf(f, "%f", &t.timpEstimat);
        inserareArb(rad, t);
    }
    fclose(f);
}

void main()
{
    nodArb* rad;
    const char* numeFisier = "date.txt";
    citireFisier(&rad, numeFisier);
    inordine(rad);
    printf("\n\n");
    traversareNiveluri(rad);
    _CrtDumpMemoryLeaks();
}
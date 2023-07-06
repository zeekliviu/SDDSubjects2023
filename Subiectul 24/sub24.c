#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <crtdbg.h>
#pragma warning(disable : 4996)
#define LINE_SIZE 128
#define _CRTDBG_MAP_ALLOC

typedef struct Data Data;
typedef struct InfoTask Task;
typedef struct ArboreBinar nodArb;

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
    (*rad)->st = aux->dr;
    aux->dr = *rad;
    *rad = aux;
    calculBF(*rad);
}

void rotatie_stanga(nodArb** rad)
{
    printf("\n\nRotatie stanga la %f\n\n", (*rad)->info.timpEstimat);
    nodArb* aux = (*rad)->dr;
    (*rad)->dr = aux->st;
    aux->st = *rad;
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
    if (!(*rad))
        return;

    calculBF(*rad);
    if ((*rad)->BF == -2) // left heavy
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
    else
        *rad = creareNod(t, NULL, NULL);
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

void spargeArb(nodArb* r, nodArb** s, unsigned t, nodArb* prev)
{
    if (!r)
        return;
    if ((r)->info.idTask == t)
    {
        *s = r;
        if (prev)
        {
            if (prev->st == r)
            {
                prev->st = NULL;
            }
            else
            {
                prev->dr = NULL;
            }
        }
        reechilibrare(s);
        return;
    }
    spargeArb((r)->st, s, t, (r));
    spargeArb((r)->dr, s, t, (r));
}

typedef struct NodeLD
{
    Task info;
    struct NodeLD* prev, * next;
} NodeLD;

NodeLD* createNodeLD(Task t)
{
    NodeLD* n = (NodeLD*)malloc(sizeof(NodeLD));
    if (n)
    {
        n->info = t;
        n->prev = n->next = NULL;
    }
    return n;
}

void insertListD(NodeLD** h, NodeLD* n)
{
    if (!*h)
    {
        *h = n;
        return;
    }
    NodeLD* aux = *h;
    while (aux->next)
    {
        aux = aux->next;
    }
    n->prev = aux;
    aux->next = n;
}

Task deepClone(Task s)
{
    Task t;
    t.idTask = s.idTask;
    t.numeTask = strdup(s.numeTask);
    t.responsabilTask = strdup(s.responsabilTask);
    t.descriereTask = strdup(s.descriereTask);
    t.dataFinalizare = s.dataFinalizare;
    t.dataStart = s.dataStart;
    t.timpEstimat = s.timpEstimat;
    return t;
}

void convertBSTLD(nodArb* r, NodeLD** h)
{
    if (!r)
        return;
    convertBSTLD(r->st, h);
    insertListD(h, createNodeLD(deepClone(r->info)));
    convertBSTLD(r->dr, h);
}

void printList(NodeLD* h)
{
    if (!h)
        return;
    printf("Timp estimat: %.2f\nID Task: %u\nNume task: %s\nDescriere task: %s\nResponsabil task: %s\nData start: %hhu/%hhu/%hu\nData stop: %hhu/%hhu/%hu\n\n", h->info.timpEstimat, h->info.idTask, h->info.numeTask, h->info.descriereTask, h->info.responsabilTask, h->info.dataStart.zi, h->info.dataStart.luna, h->info.dataStart.an, h->info.dataFinalizare.zi, h->info.dataFinalizare.luna, h->info.dataFinalizare.an);
    printList(h->next);
}

typedef struct Vector
{
    Task* el;
    int length;
} Vector;

void insertVector(Vector* v, Task t) {
    (*v).el = (Task*)realloc((*v).el, sizeof(Task) * ((*v).length + 1));
    (*v).el[(*v).length++] = t;
}

void convertLDV(NodeLD* h, Vector* v, float t) {
    while (h)
    {
        if (h->info.timpEstimat > t) {
            insertVector(v, h->info);
        }
        h = h->next;
    }

}

void printVector(Vector v) {
    for (int i = 0; i < v.length; i++)
    {
        printf("Timp estimat: %.2f\nID Task: %u\nNume task: %s\nDescriere task: %s\nResponsabil task: %s\nData start: %hhu/%hhu/%hu\nData stop: %hhu/%hhu/%hu\n\n", v.el[i].timpEstimat, v.el[i].idTask, v.el[i].numeTask, v.el[i].descriereTask, v.el[i].responsabilTask, v.el[i].dataStart.zi, v.el[i].dataStart.luna, v.el[i].dataStart.an, v.el[i].dataFinalizare.zi, v.el[i].dataFinalizare.luna, v.el[i].dataFinalizare.an);
    }
}

void deallocateTree(nodArb** r) {
    if (!*r) return;
    deallocateTree(&((*r)->st));
    deallocateTree(&((*r)->dr));
    free((*r)->info.descriereTask);
    free((*r)->info.numeTask);
    free((*r)->info.responsabilTask);
    free(*r);
    *r = NULL;
}

void deallocateLD(NodeLD** h) {
    if (!*h) return;
    deallocateLD(&((*h)->next));
    free((*h)->info.descriereTask);
    free((*h)->info.numeTask);
    free((*h)->info.responsabilTask);
    free((*h));
    *h = NULL;
}

void deallocateVector(Vector* v) {
    free(v->el);
    v->el = NULL;
    v->length = 0;
}

void main()
{
    nodArb* rad;
    const char* numeFisier = "date.txt";
    citireFisier(&rad, numeFisier);
    inordine(rad);
    printf("\n\n");
    traversareNiveluri(rad);
    nodArb* s = NULL;
    spargeArb(rad, &s, 4, NULL);
    printf("l-am spart?\n");
    reechilibrare(&rad);
    inordine(rad);
    printf("arbore secundar\n");
    inordine(s);
    NodeLD* h = NULL;
    convertBSTLD(s, &h);
    printf("Lista dubla\n");
    printList(h);
    Vector v = { NULL, 0 };
    convertLDV(h, &v, 40);
    printf("Vectorul \n\n");
    printVector(v);
    deallocateTree(&rad);
    deallocateTree(&s);
    deallocateLD(&h);
    deallocateVector(&v);
    _CrtDumpMemoryLeaks();
}
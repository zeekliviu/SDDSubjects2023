#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <crtdbg.h>
#pragma warning(disable:4996)
#define LINE_SIZE 128
#define _CRTDBG_MAP_ALLOC

typedef struct InfoLocalitate Localitate;
typedef struct ListaAdiacenta nodLP;
typedef struct ListaSimpla nodLS;
typedef struct ABC nodArb;
typedef struct VectorLocalitati Vector1;
typedef struct VectorDenumiri Vector2;

struct InfoLocalitate
{
    unsigned char id;
    char* nume;          
    unsigned int populatie;
    float suprafata;
    char* judet;
};

struct ListaSimpla
{
    Localitate info;
    nodLS* next;
};

struct ListaAdiacenta
{
    Localitate nod;
    nodLS* vecini;
    nodLP* next;
};

struct ABC
{
    Localitate info;
    nodArb* st, * dr;
};

struct VectorLocalitati
{
    Localitate* vect;
    unsigned char nrElem;
};

struct VectorDenumiri
{
    char* denumire;
};

nodLP* creareNodLP(Localitate l)
{
    nodLP* nou = (nodLP*)malloc(sizeof(nodLP));
    nou->nod = l;
    nou->vecini = NULL;
    nou->next = NULL;
    return nou;
}

void inserareListaAdiacenta(nodLP** cap, Localitate l)
{
    nodLP* nou = creareNodLP(l);
    if (*cap == NULL)
        *cap = nou;
    else
    {
        nodLP* temp = *cap;
        while (temp->next)
        {
            temp = temp->next;
        }
        temp->next = nou;
    }
}

nodLS** gasesteVecini(nodLP* cap, const unsigned char id)
{
    nodLP* index = cap;
    while (index)
    {
        if (index->nod.id == id)
            return &(index->vecini);
        index = index->next;
    }
    return NULL;
}

Localitate DeepCopy(Localitate l)
{
    Localitate noua = l;
    noua.judet = strdup(l.judet);
    noua.nume = strdup(l.nume);
    return noua;
}

Localitate gasesteNod(nodLP* cap, const unsigned char id)
{
    nodLP* index = cap;
    while (index)
    {
        if (index->nod.id == id)
            return DeepCopy(index->nod);
        index = index->next;
    }
}

nodLS* creareNodLS(Localitate l)
{
    nodLS* nou = (nodLS*)malloc(sizeof(nodLS));
    nou->info = l;
    nou->next = NULL;
    return nou;
}

void insereazaListaSimpla(nodLS** cap, Localitate l)
{
    nodLS* nou = creareNodLS(l);
    if (*cap == NULL)
        *cap = nou;
    else
    {
        nodLS* index = *cap;
        while (index->next)
        {
            index = index->next;
        }
        index->next = nou;
    }
}

void inserareVecini(nodLP* cap, const unsigned char nodStart, const unsigned char nodStop)
{
    nodLS** capNodStart = gasesteVecini(cap, nodStart);
    nodLS** capNodStop = gasesteVecini(cap, nodStop);
    Localitate locNodStart = gasesteNod(cap, nodStart);
    Localitate locNodStop = gasesteNod(cap, nodStop);
    insereazaListaSimpla(capNodStart, locNodStop);
    insereazaListaSimpla(capNodStop, locNodStart);
}

void citireFisier(nodLP** cap, const char* numeFisier)
{
    FILE* f = fopen(numeFisier, "r");
    if (!f)
    {
        printf("Fisierul nu a fost gasit!");
        exit(-1);
    }
    *cap = NULL;
    char buffer[LINE_SIZE];
    Localitate l;
    unsigned char nrLocalitati;
    fscanf(f, "%hhu", &nrLocalitati);
    for (unsigned char i = 0; i < nrLocalitati; i++)
    {
        l.id = i + 1;
        fscanf(f, "%s", buffer);
        l.nume = strdup(buffer);
        fscanf(f, "%u", &l.populatie);
        fscanf(f, "%f", &l.suprafata);
        fscanf(f, "%s", buffer);
        l.judet = strdup(buffer);
        inserareListaAdiacenta(cap, l);
    }
    unsigned char nrMuchii;
    fscanf(f, "%hhu", &nrMuchii);
    unsigned char nodStart, nodStop;
    for (unsigned char i = 0; i < nrMuchii; i++)
    {
        fscanf(f, "%hhu %hhu", &nodStart, &nodStop);
        inserareVecini(*cap, nodStart, nodStop);
    }
    fclose(f);
}

unsigned char nrMaximVecini(nodLP* cap)
{
    nodLP* index = cap;
    unsigned char max=0;
    nodLS* aux = index->vecini;
    while (aux)
    {
        max++;
        aux = aux->next;
    }
    index = index->next;
    while (index)
    {
        unsigned char nrConexiuni = 0;
        aux = index->vecini;
        while (aux)
        {
            nrConexiuni++;
            aux = aux->next;
        }
        if (nrConexiuni > max)
            max = nrConexiuni;
        index = index->next;
    }
    return max;
}

unsigned char lungimeLista(nodLS* cap)
{
    nodLS* index = cap;
    unsigned char i = 0;
    while (index)
    {
        i++;
        index = index->next;
    }
    return i;
}

Vector2* localitatiCuNrMaxim(nodLP* cap, unsigned char* dim)
{
    Vector2* rez = NULL;
    *dim = 0;
    nodLP* temp = cap;
    unsigned char maxim = nrMaximVecini(cap);
    while (temp)
    {
        if (lungimeLista(temp->vecini) == maxim)
        {
            rez = (Vector2*)realloc(rez, ++(*dim)*sizeof(Vector2));
            rez[*dim - 1].denumire = strdup(temp->nod.nume);
        }
        temp = temp->next;
    }
    return rez;
}

void traversareListaAdiacenta(nodLP* cap)
{
    nodLP* index = cap;
    while (index)
    {
        printf("Nodul %u are urmatorii vecini:\n\n", index->nod.id);
        nodLS* aux = index->vecini;
        while (aux)
        {
            printf("Nodul %u\n", aux->info.id);
            aux = aux->next;
        }
        printf("\n\n");
        index = index->next;
    }
}

void traversareVector2(Vector2* v, const unsigned char dim)
{
    for (unsigned char i = 0; i < dim; i++)
        printf("\nNodul %s", v[i].denumire);
}

nodArb* creareNodArb(Localitate l)
{
    nodArb* nou = (nodArb*)malloc(sizeof(nodArb));
    nou->info = l;
    nou->st = NULL;
    nou->dr = NULL;
    return nou;
}

void inserareArb(nodArb** rad, Localitate l)
{
    if (*rad)
    {
        if (l.suprafata > (*rad)->info.suprafata)
            inserareArb(&(*rad)->dr, l);
        else if (l.suprafata < (*rad)->info.suprafata)
            inserareArb(&(*rad)->st, l);
    }
    else *rad = creareNodArb(l);
}

void LA2Arb(nodLP* cap, nodArb** rad, const unsigned char inceput)
{
    *rad = NULL;
    nodLP* aux = cap;
    while (aux)
    {
        if (aux->nod.nume[0] == inceput)
            inserareArb(rad, DeepCopy(aux->nod));
        aux = aux->next;
    }
}

void inordine(nodArb* rad)
{
    if (rad)
    {
        inordine(rad->st);
        printf("Id localitate: %hhu\nNume: %s\nPopulatie: %u locuitori\nSuprafata: %.2f km^2\nJudet: %s\n\n", rad->info.id, rad->info.nume, rad->info.populatie, rad->info.suprafata, rad->info.judet);
        inordine(rad->dr);
    }
}

void Arb2Vect(nodArb* rad, Vector1* v, nodArb* prev)
{
    if (rad)
    {
        if (rad->st == NULL && rad->dr == NULL)
        {
            v->vect = (Localitate*)realloc(v->vect, sizeof(Localitate) * (++v->nrElem));
            v->vect[v->nrElem - 1] = DeepCopy(rad->info);
            if (prev) {
                if (rad == prev->st) {
                    prev->st = NULL;
                }
                else if (rad == prev->dr) {
                    prev->dr = NULL;
                }
            }
            free(rad->info.nume);
            free(rad->info.judet);
            free(rad);
            return;
        }
        Arb2Vect(rad->st, v, rad);
        Arb2Vect(rad->dr, v, rad);
    }
}

void traversareVector1(Vector1 w)
{
    for (unsigned char i = 0; i < w.nrElem; i++)
        printf("Id localitate: %hhu\nNume: %s\nPopulatie: %u locuitori\nSuprafata: %.2f km^2\nJudet: %s\n\n", w.vect[i].id, w.vect[i].nume, w.vect[i].populatie, w.vect[i].suprafata, w.vect[i].judet);
}

void dezalocareArb(nodArb** rad)
{
    if (*rad)
    {
        dezalocareArb(&(*rad)->st);
        dezalocareArb(&(*rad)->dr);
        free((*rad)->info.judet);
        free((*rad)->info.nume);
        free(*rad);
    }
}

void dezalocareListaSimpla(nodLS** cap)
{
    while (*cap)
    {
        nodLS* aux = *cap;
        *cap = (*cap)->next;
        free(aux->info.judet);
        free(aux->info.nume);
        free(aux);
    }
}

void dezalocareListaAdiacenta(nodLP** cap)
{
    while (*cap)
    {
        nodLP* aux = *cap;
        *cap = (*cap)->next;
        free(aux->nod.judet);
        free(aux->nod.nume);
        dezalocareListaSimpla(&aux->vecini);
        free(aux);
    }
}

void dezalocareVector1(Vector1* v)
{
    for (unsigned char i = 0; i < v->nrElem; i++)
        free(v->vect[i].judet), free(v->vect[i].nume);
    free(v->vect);
    v->nrElem = 0;
}

void dezalocareVector2(Vector2** v, unsigned char* dim)
{
    for (unsigned char i = 0; i < *dim; i++)
        free((*v)[i].denumire);
    free(*v);
    *v = NULL;
    *dim = 0;
}

void main()
{
    nodLP* listaAdiacenta;
    nodArb* rad;
    const char* numeFisier = "date.txt";
    const char inceput = 'C';
    citireFisier(&listaAdiacenta, numeFisier);
    traversareListaAdiacenta(listaAdiacenta);
    unsigned char dim;
    Vector2* v = localitatiCuNrMaxim(listaAdiacenta, &dim);
    printf("\nNodurile care au nr. maxim de conexiuni:\n\n");
    traversareVector2(v, dim);
    printf("\n\n");
    LA2Arb(listaAdiacenta, &rad, inceput);
    printf("\n\nArborele:\n\n");
    inordine(rad);
    Vector1 w;
    w.nrElem = 0;
    w.vect = NULL;
    Arb2Vect(rad, &w, NULL);
    printf("\n\nVectorul\n\n");
    traversareVector1(w);
    printf("\n\nArborele nou:\n\n");
    inordine(rad);
    dezalocareArb(&rad);
    dezalocareListaAdiacenta(&listaAdiacenta);
    dezalocareVector1(&w);
    dezalocareVector2(&v, &dim);
    _CrtDumpMemoryLeaks();
}
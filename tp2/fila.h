#ifndef FILA_H
#define FILA_H

typedef struct Item {
    unsigned pos_pag;
    struct Item* prox; 
} Item;

typedef struct Fila {
    Item* inicio;
    Item* fim;
} Fila;

Fila* cria_fila();
void free_fila(Fila* fila);
int fila_vazia(Fila* fila);
void insere_fila(Fila* fila, unsigned pos_pag);
unsigned retira_fila(Fila* fila);

#endif

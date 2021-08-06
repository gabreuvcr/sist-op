#include <stdlib.h>
#include <stdio.h>
#include "fila.h"

Fila* cria_fila() {
    Fila* fila = (Fila*) malloc(sizeof(Fila));
    fila->inicio = NULL;
    fila->fim = NULL;
    return fila;
}

void free_fila(Fila* fila) {
    Item* atual = fila->inicio;
    while (atual != NULL) {
        Item* aux = atual->prox;
        free(atual);
        atual = aux;
    }
    free(fila);
}

int fila_vazia(Fila* fila) {
    return fila->inicio == NULL;
}

void insere_fila(Fila* fila, unsigned pos_pag) {
    Item* novo_item = (Item*) malloc(sizeof(Item));
    novo_item->pos_pag = pos_pag;
    novo_item->prox = NULL;

    if (fila_vazia(fila) == 1) {
        fila->inicio = novo_item;
        fila->fim = novo_item;
    } else {
        fila->fim->prox = novo_item;
        fila->fim = novo_item;
    }
    
}

unsigned retira_fila(Fila* fila) {
    if (fila_vazia(fila) == 1) {
        printf("fila vazia\n");
        return 0;
    }

    Item* retirado = fila->inicio;
    fila->inicio = fila->inicio->prox;
    unsigned pos_pag = retirado->pos_pag;
    free(retirado);

    return pos_pag;
}

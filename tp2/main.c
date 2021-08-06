#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "fila.h"

typedef struct Pagina {
    bool valida;
    bool suja;
    char ultima_op;
    unsigned pos_mem;
    unsigned contador;
} Pagina;

unsigned max_paginas_memoria;
unsigned max_paginas_tabela;
unsigned offset;

void checa_args(int argc) {
    if (argc < 5) {
        printf("numero de argumentos invalido\n");
        exit(EXIT_FAILURE);
    }
}

void checa_alg(char* alg) {
    if (strcmp(alg, "fifo") != 0 && strcmp(alg, "lru") != 0 &&
        strcmp(alg, "2a") != 0 && strcmp(alg, "ale") != 0) {
            printf("algoritmo invalido\n");
            exit(EXIT_FAILURE);
    }
}

void checa_tamanhos(unsigned tamanho_paginas, unsigned tamanho_memoria) {
    if (tamanho_paginas <= 0 || tamanho_memoria <= 0) {
        printf("o tamanho precisa ser um inteiro maior do que 0\n");
        exit(EXIT_FAILURE);
    } else if ((tamanho_paginas & (tamanho_paginas - 1)) != 0 || 
               (tamanho_memoria & (tamanho_memoria - 1)) != 0) {
        printf("o tamanho precisa ser potencia de 2\n");
        exit(EXIT_FAILURE);
    }
}

unsigned calcula_offset(unsigned tamanho_paginas) {
    unsigned tmp = tamanho_paginas;
    unsigned offset = 0;

    while (tmp > 1) {
        tmp = tmp >> 1;
        offset++;
    }

    return offset;
}

FILE* abre_arquivo(char* alg, char* nome_arquivo) {
    FILE* arquivo = fopen(nome_arquivo, "r");
    
    if (arquivo == NULL) {
        printf("arquivo invalido\n");
        exit(EXIT_FAILURE);
    }

    return arquivo;
}

void zera_tabela(Pagina* tabela_paginas) {
    for (int i = 0; i < max_paginas_tabela; i++) {
        tabela_paginas[i].valida = false;
        tabela_paginas[i].suja = false;
        tabela_paginas[i].contador = 0;
        tabela_paginas[i].pos_mem = 0;
    }
}

void zera_memoria(unsigned* mem_fisica) {
    for (unsigned i = 0; i < max_paginas_memoria; i++) {
        mem_fisica[i] = 0;
    }   
}

void zera_seg_chance(bool* vetor_seg_chance) {
    for (unsigned i = 0; i < max_paginas_memoria; i++) {
        vetor_seg_chance[i] = false;
    }   
}

unsigned pos_pag_menos_recente(Pagina* tabela_paginas, unsigned* mem_fisica) {
    unsigned menor_contador = tabela_paginas[mem_fisica[0]].contador;
    unsigned pos_desalocada = 0;
    for (unsigned i = 0; i < max_paginas_memoria; i++) {
        if (tabela_paginas[mem_fisica[i]].contador < menor_contador) {
            menor_contador = tabela_paginas[mem_fisica[i]].contador;
            pos_desalocada = i; 
        }
    }
    return pos_desalocada;
}

unsigned pos_pag_aleatoria() {
    unsigned pos_aleatoria = random() % max_paginas_memoria;
    return pos_aleatoria;
}

unsigned pos_pag_seg_chance(bool* vetor_seg_chance, unsigned* ponteiro_seg_chance) {
    unsigned pos_atual;
    while (true) {
        pos_atual = *ponteiro_seg_chance;
        *ponteiro_seg_chance += 1;
        *ponteiro_seg_chance %= max_paginas_memoria;
        if (vetor_seg_chance[pos_atual] == false) {
            return pos_atual;
        }
        else if (vetor_seg_chance[pos_atual] == true) {
            vetor_seg_chance[pos_atual] = false;
        }
    }
}

void tabela(char* alg, Pagina* tabela_paginas, unsigned* mem_fisica, bool* vetor_seg_chance) {
    if (strcmp(alg, "lru") == 0){
        printf("End virtual | Suja | Contador | Ultima op\n");
    }
    else if (strcmp(alg, "2a") == 0){
        printf("End virtual | Suja | Seg chance | Ultima op\n");
    }
    else {
        printf("End virtual | Suja | Ultima op\n");
    }
    
    for (unsigned i = 0; i < max_paginas_memoria; i++) {
        printf("%11x |  %3u", mem_fisica[i], tabela_paginas[mem_fisica[i]].suja);
        if (strcmp(alg, "lru") == 0) {
            printf(" | %8u", tabela_paginas[mem_fisica[i]].contador);
        }
        else if (strcmp(alg, "2a") == 0) {
            printf(" | %10d", vetor_seg_chance[i]);
        }
        printf(" | %9c\n", tabela_paginas[mem_fisica[i]].ultima_op);
    }
}

void leitura_acessos(FILE* arquivo, char* alg) {
    clock_t inicio = clock();

    Pagina* tabela_paginas = (Pagina*) malloc(sizeof(Pagina) * max_paginas_tabela);
    zera_tabela(tabela_paginas);
    
    Fila* fila = cria_fila();

    unsigned* mem_fisica = (unsigned*) malloc(sizeof(unsigned) * max_paginas_memoria);
    zera_memoria(mem_fisica);

    bool* vetor_seg_chance = (bool*) malloc(sizeof(bool) * max_paginas_memoria);
    zera_seg_chance(vetor_seg_chance);

    unsigned mem_disponivel = max_paginas_memoria;
    unsigned addr, pag_desalocada, pos_desalocada;
    unsigned ponteiro_seg_chance = 0;
    unsigned pag_lidas = 0;
    unsigned pag_escritas = 0;
    unsigned contador = 1;
    char rw;

    while (fscanf(arquivo, "%x %c", &addr, &rw) != EOF) {
        unsigned pag_atual = addr >> offset;
        if (rw != 'R' && rw != 'W') {
            continue;
        }
        if (rw == 'W') {
            tabela_paginas[pag_atual].suja = true;
        }
        tabela_paginas[pag_atual].contador = contador++;
        tabela_paginas[pag_atual].ultima_op = rw;

        if (tabela_paginas[pag_atual].valida == true) { 
            if (strcmp(alg, "2a") == 0) {
                vetor_seg_chance[tabela_paginas[pag_atual].pos_mem] = true;
            }
        }
        else if (mem_disponivel > 0) { 
            unsigned pos_mem = max_paginas_memoria - mem_disponivel;

            tabela_paginas[pag_atual].valida = true;
            tabela_paginas[pag_atual].pos_mem = pos_mem;
            mem_fisica[pos_mem] = pag_atual;

            if (strcmp(alg, "fifo") == 0) {
                insere_fila(fila, pos_mem);
            }

            mem_disponivel--;
            pag_lidas++;
        } 
        else if (mem_disponivel == 0) { 
            if (strcmp(alg, "fifo") == 0) {
                pos_desalocada = retira_fila(fila);
            } 
            else if (strcmp(alg, "lru") == 0) {
                pos_desalocada = pos_pag_menos_recente(tabela_paginas, mem_fisica);
            } 
            else if (strcmp(alg, "2a") == 0) {
                pos_desalocada = pos_pag_seg_chance(vetor_seg_chance, &ponteiro_seg_chance);
            }
            else if (strcmp(alg, "ale") == 0) {
                pos_desalocada = pos_pag_aleatoria();
            } 

            pag_desalocada = mem_fisica[pos_desalocada];

            tabela_paginas[pag_desalocada].valida = false;
            if (tabela_paginas[pag_desalocada].suja == true) {
                tabela_paginas[pag_desalocada].suja = false;
                pag_escritas++;
            }

            tabela_paginas[pag_atual].valida = true;
            tabela_paginas[pag_atual].pos_mem = pos_desalocada;
            mem_fisica[pos_desalocada] = pag_atual;

            if (strcmp(alg, "fifo") == 0) {
                insere_fila(fila, pos_desalocada);
            }

            pag_lidas++;
        }
    }
    
    clock_t final = clock();    
    double tempo_total = (double) (final - inicio) / CLOCKS_PER_SEC;

    printf("Paginas lidas: %d\n", pag_lidas);
    printf("Paginas escritas: %d\n", pag_escritas);
    printf("Acessos a memoria: %d\n", contador - 1);
    printf("Tempo de execucao: %.5lfs\n", tempo_total);
    printf("Tabela:\n");
    tabela(alg, tabela_paginas, mem_fisica, vetor_seg_chance);

    free_fila(fila);
    free(mem_fisica);
    free(tabela_paginas);
    free(vetor_seg_chance);
}

int main(int argc, char* argv[]) {
    checa_args(argc);

    char* alg = argv[1];
    char* nome_arquivo = argv[2];
    unsigned tamanho_paginas = atoi(argv[3]);
    unsigned tamanho_memoria = atoi(argv[4]);

    checa_alg(alg);
    checa_tamanhos(tamanho_paginas, tamanho_memoria);

    offset = calcula_offset(tamanho_paginas * 1024);
    max_paginas_memoria = (unsigned) tamanho_memoria / tamanho_paginas;
    max_paginas_tabela = (unsigned) pow(2.0, 32.0 - offset); 
    FILE* arquivo = abre_arquivo(alg, nome_arquivo);

    srand(time(NULL));
    printf("Executando o simulador...\n");
    printf("Arquivo de entrada: %s\n", nome_arquivo);
    printf("Tamanho da memoria: %d KB\n", tamanho_memoria);
    printf("Tamanho das paginas: %d KB\n", tamanho_paginas);
    printf("Tecnica de reposicao: %s\n", alg);

    leitura_acessos(arquivo, alg);

    fclose(arquivo);
    return 0;
}

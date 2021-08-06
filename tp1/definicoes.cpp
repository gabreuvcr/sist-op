#include "definicoes.h"

int num_uso;
int prox = FIRST;
int prox_casal = EMPTY;
int threads_fechadas = 0;
pthread_mutex_t forno,fila;
pthread_cond_t cond_forno, cond_fila;

vector<int> personagens;

map<int, set<int>> pref = {
    {0, {2, 3, 4}},
    {1, {0, 3, 4}},
    {2, {1, 3, 4}},
    {3, {4}},
    {4, {EMPTY}}
};

const char* nome(long id) {
    string nome = "";
    switch (id) {
        case 0:
            nome = "Sheldon";
            break;
        case 1:
            nome = "Leonard";
            break;
        case 2:
            nome = "Howard";
            break;
        case 3:
            nome = "Stuart";
            break;
        case 4:
            nome = "Kripke";
            break;
        case 5:
            nome = "Amy";
            break;
        case 6:
            nome = "Penny";
            break;
        case 7:
            nome = "Bernadette";
            break;
    }
    return nome.c_str();
}

void quer_usar_forno(long id) {
    printf("%s quer usar o forno\n", nome(id));
}

void esquenta_algo(long id) {
    printf("%s começa a esquentar algo\n", nome(id));
    sleep(1);
}

void vai_comer(long id) {
    printf("%s vai comer\n", nome(id));
    sleep((int) (drand48() * 3.0) + 3);
}

void voltou_trabalho(long id) {
    printf("%s voltou para o trabalho\n", nome(id));
    sleep((int) (drand48() * 3.0) + 3);
}

int parceiro(long id) {
    if (id >= 5) {
        return id - 5;
    } else if (id <= 2) {
        return id + 5;
    } else {
        return EMPTY;
    }
}

int num_casais() {
    int num = 0;
    for (int i = 0; i < CASAIS; i++) {
        if (personagens[i] > 0 && personagens[parceiro(i)] > 0) {
            num++;
        }
    }
    return num;
}

int valor_personagem(long id) {
    if (parceiro(id) != EMPTY && personagens[parceiro(id)] > 0) {
        prox_casal = EMPTY; //formou outro casal, checar o proximo dnv
        return (personagens[parceiro(id)] + 1);
    } else {
        return 1;
    }
}

void trata_deadlock() {
    int novo_prox;
    novo_prox = (int) (drand48() * 2.0);
    if (personagens[novo_prox] > 0 && personagens[parceiro(novo_prox)] > 0) {
        prox = novo_prox;
        prox_casal = parceiro(novo_prox);
    } else if (personagens[novo_prox] > 0) {
        prox = novo_prox;
    } else {
        prox = parceiro(novo_prox);
    }
    printf("Raj detectou um deadlock, liberando %s\n", nome(prox));
    pthread_cond_broadcast(&cond_forno);
    pthread_mutex_unlock(&fila);
    pthread_cond_broadcast(&cond_fila);
}

void verificacao_raj() {
    while(threads_fechadas != 8) {
        sleep(5);
        if (prox == DEADLOCK) {
            trata_deadlock();
        }
    }
    pthread_exit(EXIT_SUCCESS);
}

int verifica_deadlock() {
    int qnt_sem_casal = 0;
    int qnt_casal = 0;
    for (int i = 0; i < CASAIS; i++) {
        qnt_sem_casal += (personagens[i] > 0) ^ (personagens[parceiro(i)] > 0);
        qnt_casal += (personagens[i] > 0) && (personagens[parceiro(i)] > 0);
    }
    return (qnt_sem_casal == CASAIS || qnt_casal == CASAIS);
}

void prioridade_casais() {
    prox = EMPTY;
    prox_casal = EMPTY;
    for (int i = 0; i < CASAIS; i++) {
        if (personagens[i] > 0 && personagens[parceiro(i)] > 0
            && (prox == EMPTY || pref[i].count(prox % 5))) {
            if (personagens[i] > personagens[parceiro(i)]) {
                prox = parceiro(i);
                prox_casal = i;
            } else {
                prox = i;
                prox_casal = parceiro(i);
            }
        }
    }
}

void prioridade_sem_casal() {
    prox = EMPTY;
    for (int i = 0; i < PERSONAGENS - 1; i++) {
        if (personagens[i] > 0 && (prox == EMPTY || pref[i % 5].count(prox % 5))) {
            prox = i;
        } 
    }
}

void encontra_proximo() {
    if (prox_casal != EMPTY) { //prox é o(a) namorado(a)
        prox = prox_casal;
        prox_casal = EMPTY;
    } else if (verifica_deadlock()) {
        prox = DEADLOCK;
        pthread_mutex_lock(&fila);
    } else if (num_casais() > 0) { 
        prioridade_casais();
    } else {
        prioridade_sem_casal();
    }
}

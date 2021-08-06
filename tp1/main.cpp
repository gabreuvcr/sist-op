#include "definicoes.h"

void* thread_personagem(void* arg) {
    long id = (long) arg;
    if (id == 8) {
        verificacao_raj();
    }
    for (int i = 0; i < num_uso; i++) {     
        sleep((int) (drand48() * 2.0) + 1);
        pthread_mutex_lock(&fila);
        while(prox == DEADLOCK) {
            pthread_cond_wait(&cond_fila, &fila);
        }
        personagens[id] = valor_personagem(id);
        quer_usar_forno(id);
        pthread_mutex_unlock(&fila);
        pthread_mutex_lock(&forno);
        if (prox == EMPTY) {
            encontra_proximo();
        }
        while(prox != id && prox != FIRST) {
            pthread_cond_wait(&cond_forno, &forno);
        }
        personagens[id] = 0;
        esquenta_algo(id);
        encontra_proximo();
        pthread_mutex_unlock(&forno);
        pthread_cond_broadcast(&cond_forno);
        vai_comer(id);
        voltou_trabalho(id);
    }
    threads_fechadas++;
    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        exit(EXIT_FAILURE);
    }

    srand48(time(NULL));
    num_uso = atoi(argv[1]);

    if (num_uso <= 0) {
        exit(EXIT_FAILURE);
    }
    
    pthread_t *thread_ids = new pthread_t[9];
    pthread_mutex_init(&forno, NULL);
    pthread_mutex_init(&fila, NULL);
    pthread_cond_init(&cond_forno, NULL);
    pthread_cond_init(&cond_fila, NULL);

    for (long i = 0; i < PERSONAGENS; i++) {
        personagens.push_back(0);
        pthread_create(&thread_ids[i], NULL, thread_personagem, (void*) i);
    }
    for (int i = 0; i < PERSONAGENS; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    delete [] thread_ids;
    pthread_mutex_destroy(&forno);
    pthread_mutex_destroy(&fila);
    pthread_cond_destroy(&cond_forno);
    pthread_cond_destroy(&cond_fila);

    return 0;
}

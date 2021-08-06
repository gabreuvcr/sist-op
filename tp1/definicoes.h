#ifndef DEFINICOES_H
#define DEFINICOES_H
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#define EMPTY -1
#define FIRST -2
#define DEADLOCK -3
#define PERSONAGENS 9
#define CASAIS 3
using namespace std;

extern int num_uso;
extern int prox;
extern int prox_casa;
extern int threads_fechadas;
extern pthread_mutex_t forno,fila;
extern pthread_cond_t cond_forno, cond_fila;
extern vector<int> personagens;
extern map<int, set<int>> pref;

int valor_personagem(long id);
void quer_usar_forno(long id);
void esquenta_algo(long id);
void encontra_proximo();
void vai_comer(long id);
void voltou_trabalho(long id);
void verificacao_raj();

#endif

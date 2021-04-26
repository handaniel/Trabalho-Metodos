#ifndef PMM_HPP_INCLUDED
#define PMM_HPP_INCLUDED

#include <string>
#include <memory.h>
#include <time.h>
#include <iostream>

#define MAX_PON 13206
#define MAX_POS 8

#define MAX(X, Y) ((X > Y) ? X : Y)

int numPon;
int numPosCan;
int matPosCan[MAX_PON * MAX_POS][100];
int qtdConPos[MAX_PON * MAX_POS];

typedef struct solucao
{
    int FO;
    int con;
    int vetPosPon[MAX_PON];
} Solucao;

bool lerDados(std::string arq);
void testarDados(std::string arq);

void heuConAle(Solucao &s);
void calcFO(Solucao &s);
void calcCon(Solucao &s);

void clonaSol(Solucao &s1, Solucao &s2);
void imprimeSol(std::string arq, Solucao &s, bool t);
void lerSol(std::string arq, Solucao &s);

void testeHeu(Solucao &s, int qtd);
void testeCalcFO(Solucao &s, int qtd);

Solucao populacao[10000];
void algoritmoGenetico(const int pop,const int cro, const double mut, const double eli, double tempo_max,
                       Solucao &s, double &tempo_melhor, double &tempo_total);
void crossover(int p1, int p2, int f1, int f2);
void ordena_populacao(int limite);
void mutacao(Solucao &s);

#endif // PMM_HPP_INCLUDED

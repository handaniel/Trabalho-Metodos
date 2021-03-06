#include "prcp.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    Solucao s;

    int inst = 1;
    int seed = 2;
    int tempo = 1; // 1 = 1 min, 2 = 2 min, 3 = 30 min

    double tempo_max;
    string instancia;

    switch (inst)
    {
    case 1:
        instancia = "i25";
        break;
    case 2:
        instancia = "i100";
        break;
    case 3:
        instancia = "i250";
        break;
    case 4:
        instancia = "i500";
        break;
    case 5:
        instancia = "i750";
        break;
    case 6:
        instancia = "i1000";
        break;
    case 7:
        instancia = "i13206";
        break;
    case 8:
        instancia = "i13206p8";
        break;

    default:
        break;
    }

    switch (seed)
    {
    case 1:
        srand(29834765);
        break;
    case 2:
        srand(1678364);
        break;
    case 3:
        srand(94892619);
        break;
    default:
        break;
    }

    switch (tempo)
    {
    case 1:
        tempo_max = 60;
        break;
    case 2:
        tempo_max = 120;
        break;
    case 3:
        tempo_max = 1800;
        break;
    default:
        break;
    }

    string arq = instancia + ".txt";
    double tempo_melhor, tempo_total;
    int pop = 5, cro = 5;
    double mut = 7, eli = 15;

    lerDados(arq);

    algoritmoGenetico(pop * numPosCan, cro * numPosCan, mut, eli, tempo_max, s, tempo_melhor, tempo_total);

    //Gera arquivo com nome "{instância}s{seed}.txt"
    imprimeSolTem(s, (instancia + "s" + to_string(seed) + ".txt"), seed, tempo_melhor, tempo_total);

    return 0;
}

void algoritmoGenetico(int pop, int cro, double mut, double eli, double tempo_max,
                       Solucao &s, double &tempo_melhor, double &tempo_total)
{
    int filho, p1, p2;
    clock_t hI, hF;

    hI = clock();
    for (int i = 0; i < pop; i++)
    {
        heuConAle(populacao[i]);
        calcFO(populacao[i]);
    }
    ordena_populacao(pop);
    hF = clock() - hI;

    tempo_melhor = (double)hF / CLOCKS_PER_SEC;
    memcpy(&s, &populacao[0], sizeof(populacao[0]));

    tempo_total = tempo_melhor;

    while (tempo_total < tempo_max)
    {
        filho = pop;
        for (int i = 0; i < cro; i++)
        {
            p1 = MAX(1, (eli / 100.0) * pop);
            p1 = rand() % p1;

            do
            {
                p2 = rand() % pop;
            } while (p2 == p1);

            crossover(p1, p2, filho, filho + 1);

            if (populacao[filho].FO > s.FO)
            {
                memcpy(&s, &populacao[filho], sizeof(populacao[filho]));
                hF = clock() - hI;
                tempo_melhor = (double)hF / CLOCKS_PER_SEC;
            }

            if (populacao[filho + 1].FO > s.FO)
            {
                memcpy(&s, &populacao[filho + 1], sizeof(populacao[filho + 1]));
                hF = clock() - hI;
                tempo_melhor = (double)hF / CLOCKS_PER_SEC;
            }

            if (rand() % 100 < mut)
            {
                mutacao(populacao[filho]);
                if (populacao[filho + 1].FO > s.FO)
                {
                    memcpy(&s, &populacao[filho + 1], sizeof(populacao[filho + 1]));
                    hF = clock() - hI;
                    tempo_melhor = (double)hF / CLOCKS_PER_SEC;
                }
            }
            if (rand() % 100 < mut)
            {
                mutacao(populacao[filho + 1]);
                if (populacao[filho + 1].FO > s.FO)
                {
                    memcpy(&s, &populacao[filho + 1], sizeof(populacao[filho + 1]));
                    hF = clock() - hI;
                    tempo_melhor = (double)hF / CLOCKS_PER_SEC;
                }
            }
            filho += 2;
        }
        ordena_populacao(pop + 2 * cro);
        hF = clock() - hI;
        tempo_total = (double)hF / CLOCKS_PER_SEC;
    }
}

void crossover(int p1, int p2, int f1, int f2)
{
    int p = 1 + rand() % (numPon - 1);
    for (int i = 0; i < p; i++)
    {
        populacao[f1].vetPosPon[i] = populacao[p1].vetPosPon[i];
        populacao[f2].vetPosPon[i] = populacao[p2].vetPosPon[i];
    }

    for (int i = p; i < numPon; i++)
    {
        populacao[f1].vetPosPon[i] = populacao[p2].vetPosPon[i];
        populacao[f2].vetPosPon[i] = populacao[p1].vetPosPon[i];
    }
    calcFO(populacao[f1]);
    calcFO(populacao[f2]);
}

void ordena_populacao(int limite)
{
    int flag = 1;
    Solucao aux;
    while (flag)
    {
        flag = 0;
        for (int i = 0; i < limite - 1; i++)
        {
            if (populacao[i].FO < populacao[i + 1].FO)
            {
                flag = 1;
                memcpy(&aux, &populacao[i], sizeof(populacao[i]));
                memcpy(&populacao[i], &populacao[i + 1], sizeof(populacao[i + 1]));
                memcpy(&populacao[i + 1], &aux, sizeof(aux));
            }
        }
    }
}

void mutacao(Solucao &s)
{
    int pos, pon;
    pon = 1 + rand() % numPon;
    do
    {
        pos = (pon * numPosCan) - rand() % numPosCan;
    } while (pos == s.vetPosPon[pon - 1]);

    s.vetPosPon[pon - 1] = pos;
    calcFO(s);
}

void testeHeu(Solucao &s, int qtd)
{
    clock_t h;
    double tempo;
    h = clock();
    for (int i = 0; i < qtd; i++)
    {
        heuConAle(s);
    }
    h = clock() - h;
    tempo = (double)h / CLOCKS_PER_SEC;
    printf("\nTempo de Execucao da Heuristica Construtiva por %d repeticoes: %fs", qtd, tempo);
}

void testeCalcFO(Solucao &s, int qtd)
{
    clock_t h;
    double tempo;
    h = clock();
    for (int i = 0; i < qtd; i++)
    {
        calcFO(s);
    }
    h = clock() - h;
    tempo = (double)h / CLOCKS_PER_SEC;
    printf("\nTempo de Execucao do Calculo da FO por %d repeticoes: %.5fs", qtd, tempo);
}

bool lerDados(string arq)
{
    FILE *f = fopen(arq.c_str(), "r");

    if (f != NULL)
    {
        fscanf(f, "%d %d", &numPon, &numPosCan);

        memset(&matPosCan, -1, sizeof(matPosCan));

        for (int i = 0; i < (numPon * numPosCan); i++)
        {
            fscanf(f, "%d", &qtdConPos[i]);
            for (int j = 0; j < qtdConPos[i]; j++)
            {
                fscanf(f, "%d ", &matPosCan[i][j]);
            }
        }

        fclose(f);
        return true;
    }
    else
    {
        cout << "Erro ao abrir instancia!" << endl;
        return false;
    }
}

void imprimeSol(string arq, Solucao &s, bool t)
{
    FILE *f;

    if (arq.empty())
        f = stdout;
    else
        f = fopen(arq.c_str(), "w");

    if (f != NULL)
    {
        calcCon(s);
        fprintf(f, "Num. de Pontos: %d\nNum. de posicoes candidatas: %d", numPon, numPosCan);
        fprintf(f, "\nValor FO (Num. de pontos livres): %d", s.FO);
        fprintf(f, "\nNumero de Conflitos: %d", s.con);
        if (t)
        {
            fprintf(f, "\nPosicao de cada ponto:");
            for (int i = 0; i < numPon; i++)
            {
                int a = (s.vetPosPon[i] % numPosCan);
                fprintf(f, "\n[%d] = %d", i + 1, (a == 0) ? numPosCan : a);
            }
        }

        fclose(f);
    }
    else
    {
        cout << "Erro ao abrir arquivo!" << endl;
    }
}

void clonaSol(Solucao &s1, Solucao &s2)
{
    memcpy(&s2, &s1, sizeof(s2));
}

void lerSol(string arq, Solucao &s)
{
    FILE *f = fopen(arq.c_str(), "r");
    if (f != NULL)
    {
        fscanf(f, "%d\n%d\n%d\n", &numPon, &numPosCan, &s.FO);
        for (int i = 0; i < numPon; i++)
        {
            int a;
            fscanf(f, "%d ", &a);
            s.vetPosPon[i] = (i * numPosCan) + a;
        }
        fclose(f);
    }
    else
    {
        cout << "Erro ao abrir arquivo de solucao!" << endl;
    }
}

void testarDados(string arq)
{
    FILE *f;
    if (arq == "")
        f = stdout;
    else
        f = fopen(arq.c_str(), "w");

    fprintf(f, "%d\n%d", numPon, numPosCan);
    for (int i = 0; i < (numPon * numPosCan); i++)
    {
        fprintf(f, "\n%d\n", qtdConPos[i]);
        for (int j = 0; j < qtdConPos[i]; j++)
        {
            fprintf(f, "%d ", matPosCan[i][j]);
        }
    }
    fclose(f);
}

void heuConAle(Solucao &s)
{
    for (int i = 1, j = 0; j < numPon; i += numPosCan, j++)
    {
        s.vetPosPon[j] = i + rand() % numPosCan;
    }
}

void calcFO(Solucao &s)
{
    int aux = 0;
    bool flag;
    for (int i = 0; i < numPon; i++)
    {
        flag = false;
        int j = 0;
        while (j < numPon && !flag)
        {
            if (j != i)
            {
                int k = 0;
                while (k < qtdConPos[s.vetPosPon[j] - 1] && !flag)
                {
                    if (s.vetPosPon[i] == matPosCan[s.vetPosPon[j] - 1][k])
                    {
                        aux++;
                        flag = true;
                    }
                    k++;
                }
            }
            j++;
        }
    }

    s.FO = numPon - aux;
}

void calcCon(Solucao &s)
{
    int aux = 0;
    for (int i = 0; i < numPon; i++)
    {
        for (int j = 0; j < numPon; j++)
        {
            if (i != j)
            {
                for (int k = 0; k < qtdConPos[s.vetPosPon[j] - 1]; k++)
                {
                    if (s.vetPosPon[i] == matPosCan[s.vetPosPon[j] - 1][k])
                    {
                        aux++;
                    }
                }
            }
        }
    }
    s.con = (aux / 2);
}

//Função para imprimir usada no Trabalho 2, para imprimir os tempos em arquivos diferentes
void imprimeSolTem(Solucao &s, string arq, int seed, double tempo_melhor, double tempo_total)
{
    FILE *f = fopen(arq.c_str(), "w");

    if (f != NULL)
    {
        fprintf(f, "Seed: %d", seed);
        fprintf(f, "\nValor FO: %d", s.FO);
        fprintf(f, "\nTempo de Execucao: %.5f", tempo_melhor);
        fprintf(f, "\nTempo Total: %.5f", tempo_total);
        fclose(f);
    }
}
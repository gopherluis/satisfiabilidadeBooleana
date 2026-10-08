#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int v1; // Primeira variavel
    int v2; // Segunda variavel
} Clausula;

int contarSatisfeitas(Clausula *clausulas, int nClausulas, int *atribuicoes) {
    int satisfeitas = 0;
    for (int i = 0; i < nClausulas; i++) {
        int v1 = abs(clausulas[i].v1) - 1;
        int v2 = abs(clausulas[i].v2) - 1;
        int val1 = clausulas[i].v1 > 0 ? atribuicoes[v1] : !atribuicoes[v1];
        int val2 = clausulas[i].v2 > 0 ? atribuicoes[v2] : !atribuicoes[v2];

        if (val1 || val2) {
            satisfeitas++;
        }
    }
    return satisfeitas;
}

void inicializarSolucaoAleatoria(int *solucao, int nVariaveis) {
    for (int i = 0; i < nVariaveis; i++) {
        solucao[i] = rand() % 2;
    }
}

void buscaLocal(Clausula *clausulas, int nClausulas, int *solucao, int nVariaveis, int maxIteracoes) {
    int melhorFitness = contarSatisfeitas(clausulas, nClausulas, solucao);
    for (int iter = 0; iter < maxIteracoes; iter++) {
        int varAleatoria = rand() % nVariaveis;
        solucao[varAleatoria] = !solucao[varAleatoria];
        int novoFitness = contarSatisfeitas(clausulas, nClausulas, solucao);
        if (novoFitness > melhorFitness) {
            melhorFitness = novoFitness;
        } else {
            solucao[varAleatoria] = !solucao[varAleatoria];
        }
    }
}

void mutacao(int *solucao, int nVariaveis) {
    int varAleatoria = rand() % nVariaveis;
    solucao[varAleatoria] = !solucao[varAleatoria];
}

void cruzamento(int *pai1, int *pai2, int *filho, int nVariaveis) {
    int pontoCorte = rand() % nVariaveis;
    for (int i = 0; i < pontoCorte; i++) {
        filho[i] = pai1[i];
    }
    for (int i = pontoCorte; i < nVariaveis; i++) {
        filho[i] = pai2[i];
    }
}

void algoritmoEvolutivo(Clausula *clausulas, int nClausulas, int nVariaveis, int tamPopulacao, int maxGeracoes, int *melhorSolucao) {
    int populacao[tamPopulacao][nVariaveis];
    int aptidao[tamPopulacao];

    for (int i = 0; i < tamPopulacao; i++) {
        inicializarSolucaoAleatoria(populacao[i], nVariaveis);
    }

    for (int geracao = 0; geracao < maxGeracoes; geracao++) {
        for (int i = 0; i < tamPopulacao; i++) {
            aptidao[i] = contarSatisfeitas(clausulas, nClausulas, populacao[i]);
        }

        for (int i = 0; i < tamPopulacao / 2; i++) {
            int maxIdx = i;
            for (int j = i + 1; j < tamPopulacao; j++) {
                if (aptidao[j] > aptidao[maxIdx]) {
                    maxIdx = j;
                }
            }
            int tempAptidao = aptidao[i];
            aptidao[i] = aptidao[maxIdx];
            aptidao[maxIdx] = tempAptidao;

            int tempSolucao[nVariaveis];
            for (int k = 0; k < nVariaveis; k++) {
                tempSolucao[k] = populacao[i][k];
                populacao[i][k] = populacao[maxIdx][k];
                populacao[maxIdx][k] = tempSolucao[k];
            }
        }

        for (int i = 0; i < tamPopulacao / 2; i++) {
            mutacao(populacao[i], nVariaveis);
        }
        for (int i = 0; i < tamPopulacao / 2; i += 2) {
            cruzamento(populacao[i], populacao[i + 1], populacao[tamPopulacao / 2 + i], nVariaveis);
        }
    }

    int melhorAptidao = -1;
    for (int i = 0; i < tamPopulacao; i++) {
        int apt = contarSatisfeitas(clausulas, nClausulas, populacao[i]);
        if (apt > melhorAptidao) {
            melhorAptidao = apt;
            for (int j = 0; j < nVariaveis; j++) {
                melhorSolucao[j] = populacao[i][j];
            }
        }
    }
}

int main() {
    srand(time(NULL));

    int nVariaveis = 0;
    while (nVariaveis <= 0) {
        printf("Digite o numero de variaveis (deve ser maior que 0): ");
        scanf("%d", &nVariaveis);
    }

    int nClausulas = 0;
    while (nClausulas <= 0) {
        printf("Digite o numero de clausulas (deve ser maior que 0): ");
        scanf("%d", &nClausulas);
    }

    Clausula clausulas[nClausulas];
    for (int i = 0; i < nClausulas; i++) {
        printf("Digite a clausula %d (v1 v2): ", i + 1);
        scanf("%d %d", &clausulas[i].v1, &clausulas[i].v2);
    }

    int atribuicoes[nVariaveis];
    for (int i = 0; i < nVariaveis; i++) {
        atribuicoes[i] = -1;
    }

    int metodo = 0;
    while (metodo < 1 || metodo > 2) {
        printf("Escolha o metodo:\n");
        printf("1. Busca Local\n");
        printf("2. Algoritmo Evolutivo\n");
        printf("Opcao: ");
        scanf("%d", &metodo);
        if (metodo < 1 || metodo > 2) {
            printf("Opcao invalida\n");
        }
    }

    clock_t start = clock();
    double tempo;

    switch (metodo) {
        case 1: {
                int maxIteracoes = 10000000;
                buscaLocal(clausulas, nClausulas, atribuicoes, nVariaveis, maxIteracoes);
            }
            break;

        case 2: {
                int tamPopulacao = 50;
                int maxGeracoes = 100;
                int melhorSolucao[nVariaveis];
                algoritmoEvolutivo(clausulas, nClausulas, nVariaveis, tamPopulacao, maxGeracoes, melhorSolucao);
                for (int i = 0; i < nVariaveis; i++) {
                    atribuicoes[i] = melhorSolucao[i];
                }
            }
            break;

        default:
            printf("Metodo nao reconhecido.\n");
            break;
    }

    tempo = (double)(clock() - start) / CLOCKS_PER_SEC;
    int satisfeitas = contarSatisfeitas(clausulas, nClausulas, atribuicoes);
    printf("Numero de clausulas satisfeitas: %d\n", satisfeitas);
    printf("Tempo de execucao: %.12f segundos.\n", tempo);
    return 0;
}

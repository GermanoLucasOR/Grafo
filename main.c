#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int numVertices;
    int** adjMatrix;
} GrafoMatriz;

typedef struct Nodo {
    int vertice;
    struct Nodo* prox;
} Nodo;

typedef struct {
    int numVertices;
    Nodo** adjList;
} GrafoLista;

void inicializaGrafoMatriz(GrafoMatriz* grafo, int numVertices) {
    grafo->numVertices = numVertices;
    grafo->adjMatrix = (int**) malloc(numVertices * sizeof(int*));
    for (int i = 0; i < numVertices; i++) {
        grafo->adjMatrix[i] = (int*) calloc(numVertices, sizeof(int));
    }
}

void adicionaArestaMatriz(GrafoMatriz* grafo, int origem, int destino) {
    if (origem >= 0 && origem < grafo->numVertices && destino >= 0 && destino < grafo->numVertices) {
        grafo->adjMatrix[origem][destino] = 1;
        grafo->adjMatrix[destino][origem] = 1; 
    }
}

void liberaGrafoMatriz(GrafoMatriz* grafo) {
    for (int i = 0; i < grafo->numVertices; i++) {
        free(grafo->adjMatrix[i]);
    }
    free(grafo->adjMatrix);
}

void inicializaGrafoLista(GrafoLista* grafo, int numVertices) {
    grafo->numVertices = numVertices;
    grafo->adjList = (Nodo**) malloc(numVertices * sizeof(Nodo*));
    for (int i = 0; i < numVertices; i++) {
        grafo->adjList[i] = NULL;
    }
}

Nodo* criaNodo(int vertice) {
    Nodo* novoNodo = (Nodo*) malloc(sizeof(Nodo));
    novoNodo->vertice = vertice;
    novoNodo->prox = NULL;
    return novoNodo;
}

void adicionaArestaLista(GrafoLista* grafo, int origem, int destino) {
    Nodo* novoNodo = criaNodo(destino);
    novoNodo->prox = grafo->adjList[origem];
    grafo->adjList[origem] = novoNodo;

    novoNodo = criaNodo(origem);
    novoNodo->prox = grafo->adjList[destino];
    grafo->adjList[destino] = novoNodo;
}

void liberaGrafoLista(GrafoLista* grafo) {
    for (int i = 0; i < grafo->numVertices; i++) {
        Nodo* temp = grafo->adjList[i];
        while (temp) {
            Nodo* prox = temp->prox;
            free(temp);
            temp = prox;
        }
    }
    free(grafo->adjList);
}

void lerArquivoECarregarGrafo(char* nomeArquivo, GrafoMatriz* grafoMatriz, GrafoLista* grafoLista) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    int numVertices;
    fscanf(arquivo, "%d", &numVertices);
    inicializaGrafoMatriz(grafoMatriz, numVertices);
    inicializaGrafoLista(grafoLista, numVertices);

    for (int i = 0; i < numVertices; i++) {
        for (int j = 0; j < numVertices; j++) {
            int valor;
            fscanf(arquivo, "%d", &valor);
            if (valor != 0) {
                adicionaArestaMatriz(grafoMatriz, i, j);
                adicionaArestaLista(grafoLista, i, j);
            }
        }
    }

    fclose(arquivo);
}

bool bfs(GrafoMatriz* grafo, int inicio, int fim) {
    if (inicio < 0 || inicio >= grafo->numVertices || fim < 0 || fim >= grafo->numVertices) {
        return false;
    }

    bool* visitado = (bool*) calloc(grafo->numVertices, sizeof(bool));
    int* antecessor = (int*) malloc(grafo->numVertices * sizeof(int));
    for (int i = 0; i < grafo->numVertices; i++) {
        antecessor[i] = -1;
    }

    int* fila = (int*) malloc(grafo->numVertices * sizeof(int));
    int frente = 0, traseira = 0;
    visitado[inicio] = true;
    fila[traseira++] = inicio;

    while (frente < traseira) {
        int verticeAtual = fila[frente++];

        for (int i = 0; i < grafo->numVertices; i++) {
            if (grafo->adjMatrix[verticeAtual][i] && !visitado[i]) {
                fila[traseira++] = i;
                visitado[i] = true;
                antecessor[i] = verticeAtual;

                if (i == fim) {
                    int* caminho = (int*) malloc(grafo->numVertices * sizeof(int));
                    int tamanhoCaminho = 0;
                    for (int v = fim; v != -1; v = antecessor[v]) {
                        caminho[tamanhoCaminho++] = v;
                    }

                    printf("Caminho entre %d e %d: ", inicio, fim);
                    for (int v = tamanhoCaminho - 1; v >= 0; v--) {
                        printf("%d ", caminho[v]);
                    }
                    printf("\n");
                    free(caminho);
                    free(visitado);
                    free(antecessor);
                    free(fila);
                    return true;
                }
            }
        }
    }

    free(visitado);
    free(antecessor);
    free(fila);
    return false;
}

void dfsIterativo(GrafoMatriz* grafo, int inicio) {
    if (inicio < 0 || inicio >= grafo->numVertices) {
        printf("Vértice fora dos limites.\n");
        return;
    }

    bool* visitado = (bool*) calloc(grafo->numVertices, sizeof(bool));
    int* pilha = (int*) malloc(grafo->numVertices * sizeof(int));
    int topo = -1;

    pilha[++topo] = inicio;

    while (topo != -1) {
        int verticeAtual = pilha[topo--];

        if (!visitado[verticeAtual]) {
            printf("%d ", verticeAtual);
            visitado[verticeAtual] = true;
        }

        for (int i = 0; i < grafo->numVertices; i++) {
            if (grafo->adjMatrix[verticeAtual][i] && !visitado[i]) {
                pilha[++topo] = i;
            }
        }
    }
    printf("\n");
    free(visitado);
    free(pilha);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo_grafo>\n", argv[0]);
        return 1;
    }

    GrafoMatriz grafoMatriz;
    GrafoLista grafoLista;

    lerArquivoECarregarGrafo(argv[1], &grafoMatriz, &grafoLista);

    int destino;
    bool caminhoEncontrado = false;

    while (!caminhoEncontrado) {
        printf("Escolha o vértice de destino (0 a %d) ou -1 para sair: ", grafoMatriz.numVertices - 1);
        scanf("%d", &destino);

        if (destino == -1) {
            printf("Saindo...\n");
            break;
        }

        if (destino < 0 || destino >= grafoMatriz.numVertices) {
            printf("Vértice de destino inválido.\n");
            continue;
        }

        caminhoEncontrado = bfs(&grafoMatriz, 0, destino);

        if (!caminhoEncontrado) {
            printf("Não há caminho entre o vértice 0 e o vértice %d. Tente outro vértice.\n", destino);
        }
    }

    printf("DFS Iterativo a partir do vértice 0:\n");
    dfsIterativo(&grafoMatriz, 0);

    liberaGrafoMatriz(&grafoMatriz);
    liberaGrafoLista(&grafoLista);

    return 0;
}

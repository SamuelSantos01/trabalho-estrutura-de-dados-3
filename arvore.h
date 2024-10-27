#ifndef ARVORE_B_H
#define ARVORE_B_H

#include "carro.h"

#define ORDEM 3  // Ordem da árvore-B (número máximo de filhos é ORDEM*2)

typedef struct No {
    int num_chaves;                  // Número de chaves no nó
    char chaves[ORDEM * 2][8];       // Chaves (placas dos carros), array de strings
    long offsets[ORDEM * 2];         // Offsets dos registros no arquivo de dados
    struct No* filhos[ORDEM * 2 + 1]; // Ponteiros para os filhos
    int folha;                       // 1 se o nó é folha, 0 caso contrário
} No;

typedef struct {
    No* raiz;       // Ponteiro para o nó raiz
    int ordem;       // Ordem da árvore
} ArvoreB;

// Função para inicializar a árvore-B
ArvoreB* inicializar_arvoreB();
ArvoreB*  carregar_criar(const char* nome_arquivo);
void inserir_na_arvore_b(ArvoreB* arvore, const char* placa, long offset);
void salvar_arvore_no_arquivo(ArvoreB* arvore, const char* nome_arquivo);



#endif

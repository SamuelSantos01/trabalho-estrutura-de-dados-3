#ifndef ESTRUTURADEDADOS2_H
#define ESTRUTURADEDADOS2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAVES 3            // Máximo de chaves por nó na árvore-B
#define MAX_RECORD_LENGTH 256 // Comprimento máximo de um registro

typedef struct {
    char placa[8];        
    char* modelo;         
    char* marca;         
    int ano;             
    float diaria;         
    char* categoria;     
    int disponivel;       
} Carro;

typedef struct No {
    int num_chaves;                       // Número de chaves no nó
    char chaves[MAX_CHAVES * 2][8];         // Chaves (placas dos carros)
    long offsets[MAX_CHAVES * 2];           // Offsets dos registros no arquivo de dados
    struct No* filhos[MAX_CHAVES * 2 + 1];  // Ponteiros para os filhos
    int folha;                            // 1 se o nó é folha, 0 caso contrário
} No;

typedef struct {
    No* raiz;      // Ponteiro para o nó raiz
    int ordem;     // Ordem da árvore
} ArvoreB;

void iniciar_arquivo_dados(const char* nome_arquivo);
long inserir_registro(const Carro* carro, const char* nome_arquivo);
Carro* buscar_registro(long offset, const char* nome_arquivo);
void atualizar_registro(long offset, const Carro* carro, const char* nome_arquivo);

ArvoreB* criar_arvore_B(int ordem);
No* criar_no(int folha);
void inserir_chave(ArvoreB* arvore, const char* placa, long offset);
long buscar_chave(ArvoreB* arvore, const char* placa);
void remover_chave(ArvoreB* arvore, const char* placa);

void dividir_no(No* pai, int indice, No* no_cheio);
void inserir_nao_cheio(No* no, const char* placa, long offset);
void remover_no(No* no, const char* placa, ArvoreB* arvore);
void garantir_balanceamento(No* no, int indice);
void listar_chaves(No* no);
void listar_todas_as_chaves(ArvoreB* arvore);
void liberar_nos(No* no);
void liberar_arvoreB(ArvoreB* arvore);
long buscar_no(No* no, const char* placa);
void combinar_no(No* no, int indice);
Carro buscar_dados_por_placa(const char* placa);
// Novas funções adicionadas
void salvarDadosNoArquivo(const char* nome_arquivo, ArvoreB* arvore);
void escreverArvoreBNoArquivo(FILE* arquivo, No* no);
void carregarDadosDoArquivo(const char* nome_arquivo, ArvoreB* arvore);

#endif
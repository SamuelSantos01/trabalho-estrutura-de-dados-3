#ifndef CADASTRO_H
#define CADASTRO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define T 3  // Ordem mínima da árvore B

// Estrutura para armazenar os dados de um aluno
typedef struct elementoNodo {
    char nome[100];               // Nome do aluno (campo de tamanho fixo)
    char cpf[15];                 // CPF do aluno (campo de tamanho fixo, incluindo pontuação)
    unsigned long long chavePrimaria;   // Chave primária para identificação (pode ser CPF ou outro identificador único)
    struct elementoNodo *proximo;            // Ponteiro para o próximo aluno (para facilitar a navegação sequencial)
    struct elementoNodo *anterior;            // Ponteiro para o aluno anterior (para navegação sequencial)
} ElementoNodo;

// Estrutura para um nó da árvore B
typedef struct NodoArvoreB {
    ElementoNodo* chaves[2 * T - 1];  // Array de ponteiros para ElementoNodo, representando os dados dos alunos
    struct NodoArvoreB *filhos[2 * T]; // Ponteiros para os filhos do nó
    int numeroDeChaves;                // Número de chaves armazenadas no nó
    int ehFolha;                      // Indica se o nó é uma folha
} NodoArvoreB;

// Cabeçalho de funções
void SalvarArvoreBinariaNoArquivo(NodoArvoreB* nodo, FILE* arquivo);
void SalvarArvoreBinaria(NodoArvoreB* raiz);
void SalvarArvoreNoArquivo(NodoArvoreB* nodo, FILE* arquivo);
NodoArvoreB* CarregarArvoreBinariaDoArquivo(FILE* arquivo);
void CarregarArvoreBinaria(NodoArvoreB** raiz);
void SalvarArvore(NodoArvoreB* raiz);
void CarregarArvore(NodoArvoreB** raiz);
NodoArvoreB* CriarNodo(bool ehFolha);
ElementoNodo* CriarElementoNodo(const char* nome, unsigned long long chavePrimaria);
void DividirFilho(NodoArvoreB* pai, int i, NodoArvoreB* filho);
void InserirNaoCheio(NodoArvoreB* nodo, ElementoNodo* chave);
void Inserir(NodoArvoreB** raiz, const char* nome);
ElementoNodo* BuscarNos(NodoArvoreB* nodo, unsigned long long chavePrimaria);
ElementoNodo* ObterPredecessor(NodoArvoreB* nodo, int indice);
ElementoNodo* ObterSucessor(NodoArvoreB* nodo, int indice);
void Preencher(NodoArvoreB* nodo, int indice);
void EmprestarDoAnterior(NodoArvoreB* nodo, int indice);
void EmprestarDoProximo(NodoArvoreB* nodo, int indice);
void Mesclar(NodoArvoreB* nodo, int indice);
void RemoverDaFolha(NodoArvoreB* nodo, int indice);
void RemoverDeNaoFolha(NodoArvoreB* nodo, int indice);
void RemoverChave(NodoArvoreB** raiz, unsigned long long chavePrimaria);
void ImprimirArvoreB(NodoArvoreB* nodo, int profundidade, const char* posicao);
void ImprimirEmOrdem(NodoArvoreB* nodo);
unsigned long long FuncaoHash(const char *nome);
void TestarHash(char *nome);
void Menu();

#endif  // CADASTRO_H

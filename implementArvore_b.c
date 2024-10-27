#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore.h"

// Inicializa uma nova árvore-B
ArvoreB* inicializar_arvoreB() {
    ArvoreB* arvore = (ArvoreB*)malloc(sizeof(ArvoreB));
    arvore->raiz = NULL; // Inicialmente, a árvore não tem raiz
    arvore->ordem = ORDEM;
    return arvore;
}

// Função para carregar ou criar a árvore-B
ArvoreB* carregar_criar(const char* nome_arquivo) {
    FILE* arquivo = fopen(nome_arquivo, "rb"); // Tenta abrir o arquivo em modo binário

    ArvoreB* arvore;

    if (arquivo == NULL) {
        // O arquivo não existe, cria um novo arquivo e inicializa uma nova árvore
        printf("Arquivo não encontrado. Criando uma nova árvore.\n");
        arvore = inicializar_arvore_b();
        
        // Cria o arquivo para armazenar a árvore (pode ser melhorado para armazenar a estrutura da árvore)
        arquivo = fopen(nome_arquivo, "wb");
        if (arquivo == NULL) {
            perror("Erro ao criar o arquivo");
            exit(EXIT_FAILURE);
        }

        // Aqui você pode armazenar a estrutura inicial da árvore, se necessário
        // fwrite(arvore, sizeof(ArvoreB), 1, arquivo); // Exemplo de como salvar a árvore
        
    } else {
        // O arquivo existe, carrega a árvore existente
        printf("Arquivo encontrado. Carregando a árvore existente.\n");
        
        arvore = (ArvoreB*)malloc(sizeof(ArvoreB));
        fread(arvore, sizeof(ArvoreB), 1, arquivo); // Lê a estrutura da árvore do arquivo
        
        fclose(arquivo);
        return arvore;
    }

    // Fecha o arquivo após a criação
    fclose(arquivo);
    return arvore;
}
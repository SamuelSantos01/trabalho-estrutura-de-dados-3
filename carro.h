#ifndef CARRO_H
#define CARRO_H

typedef struct {
    char placa[8];        // Placa do carro, tamanho fixo
    char* modelo;         // Modelo do carro, tamanho variável
    char* marca;          // Marca do carro, tamanho variável
    int ano;              // Ano do carro, tamanho fixo
    float diaria;         // Preço por dia de aluguel, tamanho fixo
    char* categoria;      // Categoria do carro (econômico, SUV, luxo, etc.), tamanho variável
    int disponivel;       // Status de disponibilidade (1 para disponível, 0 para alugado), tamanho fixo
} Carro;

// Funções para manipulação de arquivos de dados e gerenciamento de memória

#endif

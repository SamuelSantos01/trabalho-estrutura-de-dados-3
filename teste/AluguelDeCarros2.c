#include "EstruturaDeDados2.h"
#include <locale.h>


//funções para a manipulação de arquivos
void iniciar_arquivo_dados(const char* nome_arquivo) {
    printf("Tentando criar ou abrir o arquivo: %s\n", nome_arquivo);
    FILE* arquivo = fopen(nome_arquivo, "a+"); 
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo de dados");
        exit(1);
    }
    printf("Arquivo criado com sucesso: %s\n", nome_arquivo);
    fclose(arquivo); 
}


long inserir_registro(const Carro* carro, const char* nome_arquivo) {
    FILE* arquivo = fopen(nome_arquivo, "a+"); 
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo de dados para escrita");
        return -1;
    }

    // Salva a posição atual do arquivo, que será o offset do registro
    long offset = ftell(arquivo);

       fprintf(arquivo, "%s|%s|%s|%d|%.2f|%s|%d\n",
            carro->placa,
            carro->modelo,
            carro->marca,
            carro->ano,
            carro->diaria,
            carro->categoria,
            carro->disponivel);

    fclose(arquivo);
    return offset; // Retorna o offset para uso na árvore-B
}

Carro* buscar_registro(long offset, const char* nome_arquivo) {
    FILE* arquivo = fopen(nome_arquivo, "r"); // Abre para leitura
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo de dados para leitura");
        return NULL;
    }

    // Movimenta o ponteiro do arquivo para o offset desejado
    fseek(arquivo, offset, SEEK_SET);

    // Aloca memória para um novo registro Carro
    Carro* carro = (Carro*)malloc(sizeof(Carro));
    if (carro == NULL) {
        perror("Erro de alocação de memória para Carro");
        fclose(arquivo);
        return NULL;
    }

    // Lê o registro com os campos separados por '|'
    char modelo[50], marca[50], categoria[50];
    fscanf(arquivo, "%7[^|]|%49[^|]|%49[^|]|%d|%f|%49[^|]|%d\n",
           carro->placa, modelo, marca, &carro->ano, &carro->diaria, categoria, &carro->disponivel);

    // Aloca memória e copia os campos de tamanho variável
    carro->modelo = strdup(modelo);
    carro->marca = strdup(marca);
    carro->categoria = strdup(categoria);

    fclose(arquivo);
    return carro;
}

void atualizar_registro(long offset, const Carro* carro, const char* nome_arquivo) {
    FILE* arquivo = fopen(nome_arquivo, "r+"); // Abre para leitura/escrita
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo de dados para atualização");
        return;
    }

    // Move para o offset, escreve o novo registro e preenche com espaços caso o novo registro seja menor
    fseek(arquivo, offset, SEEK_SET);
    fprintf(arquivo, "%s|%s|%s|%d|%.2f|%s|%d\n",
            carro->placa,
            carro->modelo,
            carro->marca,
            carro->ano,
            carro->diaria,
            carro->categoria,
            carro->disponivel);

    fclose(arquivo);
}
// Função nova para salvar os dados no arquivo
void salvarDadosNoArquivo(const char* nome_arquivo, ArvoreB* arvore) {
    FILE* arquivo = fopen(nome_arquivo, "wb"); 
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo de dados");
        exit(1);
    }
    escreverArvoreBNoArquivo(arquivo, arvore->raiz);
    fclose(arquivo);
}
//funções para a manipulação da árvore

// Cria uma nova árvore-B
ArvoreB* criar_arvore_B(int ordem) {
    ArvoreB* arvore = (ArvoreB*)malloc(sizeof(ArvoreB));
    if (arvore == NULL) {
        perror("Erro ao alocar memória para a árvore-B");
        exit(1);
    }
    arvore->ordem = ordem;
    arvore->raiz = criar_no(1);
    return arvore;
}

// Cria um novo nó da árvore-B
No* criar_no(int folha) {
    No* no = (No*)malloc(sizeof(No));
    if (no == NULL) {
        perror("Erro ao alocar memória para o nó da árvore-B");
        exit(1);
    }
    no->folha = folha;
    no->num_chaves = 0;
    for (int i = 0; i < MAX_CHAVES * 2 + 1; i++) {
        no->filhos[i] = NULL; 
    }
    return no;
}

void inserir_chave(ArvoreB* arvore, const char* placa, long offset) {
    No* raiz = arvore->raiz;

       if (raiz->num_chaves == MAX_CHAVES * 2) {
        No* novo_no = criar_no(0); 
        novo_no->filhos[0] = raiz;
        arvore->raiz = novo_no;

        dividir_no(novo_no, 0, raiz);
        inserir_nao_cheio(novo_no, placa, offset);
    } else {
        inserir_nao_cheio(raiz, placa, offset);
    }
}
// Funções novas para escrever a árvore-B no arquivo e carregar do arquivo
void escreverArvoreBNoArquivo(FILE* arquivo, No* no) {
    if (no == NULL) return;
    fwrite(no, sizeof(No), 1, arquivo);
    if (!no->folha) {
        for (int i = 0; i <= no->num_chaves; i++) {
            escreverArvoreBNoArquivo(arquivo, no->filhos[i]);
        }
    }
}
void carregarDadosDoArquivo(const char* nome_arquivo, ArvoreB* arvore) {
    FILE* arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para leitura");
        exit(1);
    }
    fread(arvore->raiz, sizeof(No), 1, arquivo);
    fclose(arquivo);
}
long buscar_chave(ArvoreB* arvore, const char* placa) {
    return buscar_no(arvore->raiz, placa);
}

void remover_chave(ArvoreB* arvore, const char* placa) {
    remover_no(arvore->raiz, placa, arvore); 
}

//Funções auxiliares
void dividir_no(No* pai, int indice, No* no_cheio) {
    int meio = MAX_CHAVES; 
    No* novo_no = criar_no(no_cheio->folha);
    novo_no->num_chaves = MAX_CHAVES;

    
    for (int j = 0; j < MAX_CHAVES; j++) {
        strcpy(novo_no->chaves[j], no_cheio->chaves[meio + 1 + j]);
        novo_no->offsets[j] = no_cheio->offsets[meio + 1 + j];
    }

   
    if (!no_cheio->folha) {
        for (int j = 0; j <= MAX_CHAVES; j++) {
            novo_no->filhos[j] = no_cheio->filhos[meio + 1 + j];
        }
    }

    no_cheio->num_chaves = MAX_CHAVES;

 
    for (int j = pai->num_chaves; j >= indice + 1; j--) {
        pai->filhos[j + 1] = pai->filhos[j];
    }
    pai->filhos[indice + 1] = novo_no;

    for (int j = pai->num_chaves - 1; j >= indice; j--) {
        strcpy(pai->chaves[j + 1], pai->chaves[j]);
        pai->offsets[j + 1] = pai->offsets[j];
    }

    // Move a chave do meio para o pai
    strcpy(pai->chaves[indice], no_cheio->chaves[meio]);
    pai->offsets[indice] = no_cheio->offsets[meio];
    pai->num_chaves++;
}

void inserir_nao_cheio(No* no, const char* placa, long offset) {
    int i = no->num_chaves - 1;

    if (no->folha) {
        while (i >= 0 && strcmp(placa, no->chaves[i]) < 0) {
            strcpy(no->chaves[i + 1], no->chaves[i]);
            no->offsets[i + 1] = no->offsets[i];
            i--;
        }
        strcpy(no->chaves[i + 1], placa);
        no->offsets[i + 1] = offset;
        no->num_chaves++;
    } else {
        
        while (i >= 0 && strcmp(placa, no->chaves[i]) < 0) {
            i--;
        }
        i++;
        
       
        if (no->filhos[i]->num_chaves == MAX_CHAVES * 2) {
            dividir_no(no, i, no->filhos[i]);
            if (strcmp(placa, no->chaves[i]) > 0) {
                i++;
            }
        }
        inserir_nao_cheio(no->filhos[i], placa, offset);
    }
}

// Função auxiliar para remover a chave
void remover_no(No* no, const char* placa, ArvoreB* arvore) { 
    int i = 0;

    // Localiza a posição onde a chave pode estar
    while (i < no->num_chaves && strcmp(placa, no->chaves[i]) > 0) {
        i++;
    }

    // Se a chave for encontrada neste nó
    if (i < no->num_chaves && strcmp(placa, no->chaves[i]) == 0) {
        if (no->folha) {
            // Se o nó é folha, remove a chave diretamente
            for (int j = i; j < no->num_chaves - 1; j++) {
                strcpy(no->chaves[j], no->chaves[j + 1]);
                no->offsets[j] = no->offsets[j + 1];
            }
            no->num_chaves--;
        } else {
            // Se o nó não é folha, deve encontrar o predecessor ou sucessor
            // (por simplicidade, vamos usar o predecessor)
            const char* chave_predecessor = no->chaves[i - 1];
            long offset_predecessor = buscar_chave(arvore, chave_predecessor); // Usando a árvore passada como argumento
            remover_no(no->filhos[i], chave_predecessor, arvore); // Remove do filho

            // Substitui a chave no nó atual
            strcpy(no->chaves[i], chave_predecessor);
            no->offsets[i] = offset_predecessor;
        }
    } else {
        // Se não é folha, deve buscar no filho adequado
        if (no->folha) {
            printf("A chave não está presente.\n");
            return; // Chave não encontrada
        }

        // Verifica se o filho que deve ser acessado tem chaves suficientes
        No* filho = no->filhos[i];
        if (filho->num_chaves < MAX_CHAVES) {
            // Garantir que o filho tenha chaves suficientes
            garantir_balanceamento(no, i);
        }

        // Se o filho foi balanceado, deve chamar a remoção
        if (i < no->num_chaves && strcmp(placa, no->chaves[i]) > 0) {
            remover_no(filho, placa, arvore);
        } else {
            remover_no(no->filhos[i], placa, arvore);
        }
    }
}

// Função para garantir que o nó tenha chaves suficientes após a remoção
void garantir_balanceamento(No* no, int indice) {
    No* filho = no->filhos[indice];
    No* irmao_esquerdo = (indice > 0) ? no->filhos[indice - 1] : NULL;
    No* irmao_direito = (indice < no->num_chaves) ? no->filhos[indice + 1] : NULL;

    // Se o irmão à esquerda tiver chaves suficientes
    if (irmao_esquerdo && irmao_esquerdo->num_chaves > MAX_CHAVES) {
        // Empresta uma chave do irmão à esquerda
        for (int j = filho->num_chaves; j > 0; j--) {
            strcpy(filho->chaves[j], filho->chaves[j - 1]); // Move as chaves do filho para a direita
            filho->offsets[j] = filho->offsets[j - 1]; // Move os offsets
        }
        strcpy(filho->chaves[0], no->chaves[indice - 1]); // Copia a chave do nó pai para o filho
        filho->offsets[0] = no->offsets[indice - 1]; // Copia o offset correspondente
        filho->num_chaves++;

        strcpy(no->chaves[indice - 1], irmao_esquerdo->chaves[irmao_esquerdo->num_chaves - 1]); // Correção aqui
        no->offsets[indice - 1] = irmao_esquerdo->offsets[irmao_esquerdo->num_chaves - 1]; // Atualiza o offset correspondente

        irmao_esquerdo->num_chaves--; // Reduz o número de chaves do irmão esquerdo
    }
    // Se o irmão à direita tiver chaves suficientes
    else if (irmao_direito && irmao_direito->num_chaves > MAX_CHAVES) {
        // Empresta uma chave do irmão à direita
        strcpy(filho->chaves[filho->num_chaves], no->chaves[indice]); // Copia a chave do nó pai para o filho
        filho->offsets[filho->num_chaves] = no->offsets[indice]; // Copia o offset correspondente
        filho->num_chaves++;

        // Atualiza a chave do nó pai
        strcpy(no->chaves[indice], irmao_direito->chaves[0]); // Correção aqui
        no->offsets[indice] = irmao_direito->offsets[0]; // Atualiza o offset do nó pai

        // Move as chaves do irmão direito para a esquerda
        for (int j = 1; j < irmao_direito->num_chaves; j++) {
            strcpy(irmao_direito->chaves[j - 1], irmao_direito->chaves[j]); // Move as chaves
            irmao_direito->offsets[j - 1] = irmao_direito->offsets[j]; // Move os offsets
        }
        if (!irmao_direito->folha) {
            irmao_direito->filhos[irmao_direito->num_chaves] = irmao_direito->filhos[irmao_direito->num_chaves + 1]; // Move os filhos
        }
        irmao_direito->num_chaves--; // Reduz o número de chaves do irmão direito
    }
    // Se nenhum irmão tiver chaves suficientes, combina os nós
    else {
        if (irmao_esquerdo) {
            combinar_no(no, indice - 1); // Combina com o irmão esquerdo
        } else {
            combinar_no(no, indice); // Combina com o irmão direito
        }
    }
}

void listar_chaves(No* no) {
    if (no == NULL) {
        return;
    }

    // Percorre todas as chaves deste nó
    for (int i = 0; i < no->num_chaves; i++) {
        // Se não for um nó folha, visita o filho
        if (!no->folha) {
            listar_chaves(no->filhos[i]); // Recursão para o filho
        }
        // Imprime a chave (placa do carro)
        printf("Placa: %s\n", no->chaves[i]);
    }

    // Se não for um nó folha, visita o último filho
    if (!no->folha) {
        listar_chaves(no->filhos[no->num_chaves]);
    }
}

// Função que chama listar_chaves a partir da raiz
void listar_todas_as_chaves(ArvoreB* arvore) {
    if (arvore != NULL && arvore->raiz != NULL) {
        listar_chaves(arvore->raiz);
    } else {
        printf("A árvore está vazia.\n");
    }
}

void liberar_nos(No* no) {
    if (no == NULL) {
        return;
    }

    // Libera todos os filhos
    for (int i = 0; i <= no->num_chaves; i++) {
        liberar_nos(no->filhos[i]);
    }

    // Libera a memória das chaves (placas) e dos campos de tamanho variável
    for (int i = 0; i < no->num_chaves; i++) {
        free(no->chaves[i]);   // Libera cada chave
    }

    free(no); // Libera o nó atual
}

void liberar_arvoreB(ArvoreB* arvore) {
    if (arvore == NULL) {
        return;
    }

    liberar_nos(arvore->raiz); // Libera todos os nós a partir da raiz
    free(arvore);              // Libera a estrutura da árvore
}


// Função auxiliar para buscar na árvore-B
long buscar_no(No* no, const char* placa) {
    int i = 0;

    // Localiza a posição onde a chave pode estar
    while (i < no->num_chaves && strcmp(placa, no->chaves[i]) > 0) {
        i++;
    }

    // Se a chave for encontrada, retorna o offset correspondente
    if (i < no->num_chaves && strcmp(placa, no->chaves[i]) == 0) {
        return no->offsets[i];
    }

    // Se for um nó folha, a chave não está presente
    if (no->folha) {
        return -1; // Chave não encontrada
    }

    // Busca recursivamente no filho correspondente
    return buscar_no(no->filhos[i], placa);
}

void combinar_no(No* no, int indice) {
    No* filho = no->filhos[indice];
    No* irmao = no->filhos[indice + 1];

    // Move a chave do nó pai para o filho
    strcpy(filho->chaves[filho->num_chaves], no->chaves[indice]); // Copia a chave
    filho->offsets[filho->num_chaves] = no->offsets[indice]; // Copia o offset
    filho->num_chaves++;

    // Move todas as chaves do irmão para o filho
    for (int j = 0; j < irmao->num_chaves; j++) {
        strcpy(filho->chaves[filho->num_chaves], irmao->chaves[j]); // Copia a chave
        filho->offsets[filho->num_chaves] = irmao->offsets[j]; // Copia o offset
        filho->num_chaves++;
    }

    // Move os filhos do irmão, se não for folha
    if (!filho->folha) {
        for (int j = 0; j <= irmao->num_chaves; j++) {
            filho->filhos[filho->num_chaves] = irmao->filhos[j]; // Aloca os filhos
        }
    }

    // Ajusta o número de chaves do nó pai
    for (int j = indice + 1; j < no->num_chaves; j++) {
        strcpy(no->chaves[j - 1], no->chaves[j]);
        no->offsets[j - 1] = no->offsets[j];
    }
    no->num_chaves--;
    free(irmao); // Libera a memória do nó combinado
}

Carro buscar_dados_por_placa(const char* placa) {
    Carro carro;
    FILE* arquivo = fopen("seu_arquivo.txt", "r"); // Ajuste o nome do arquivo conforme necessário
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    // Lê o arquivo linha por linha
    while (fscanf(arquivo, "%7s %49s %49s %d %f %29s %d",
                  carro.placa, carro.modelo, carro.marca,
                  &carro.ano, &carro.diaria, carro.categoria,
                  &carro.disponivel) == 6) {
        if (strcmp(carro.placa, placa) == 0) {
            fclose(arquivo);
            return carro; // Retorna o carro encontrado
        }
    }

    fclose(arquivo);
    // Retorne um carro vazio ou trate o caso de não encontrar
    return carro; // ou use uma maneira de indicar que não encontrou
}



void exibir_menu() {
    printf("\nMenu:\n");
    printf("1. Iniciar arquivo de dados\n");
    printf("2. Inserir registro\n");
    printf("3. Buscar registro\n");
    printf("4. Atualizar registro\n");
    printf("5. Remover registro\n");
    printf("6. Listar todos os registros\n");
    printf("7. Sair\n");
    printf("Escolha uma opção: ");
}

int main() {
    system("chcp 65001");
    ArvoreB* arvore = criar_arvore_B(3); // Criar a árvore-B com ordem 3
    const char* nome_arquivo = "dados_carros2.txt";
    iniciar_arquivo_dados(nome_arquivo);
    //Caregar os dados do arquivo se ele existir 
    carregarDadosDoArquivo(nome_arquivo, arvore);
    int opcao;
    while (1) {
        exibir_menu();
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: {
                iniciar_arquivo_dados(nome_arquivo);
                printf("Arquivo de dados iniciado com sucesso.\n");
                break;
            }
            case 2: {
                Carro carro;
                printf("Digite a placa do carro: ");
                scanf("%s", carro.placa);
                printf("Digite o modelo do carro: ");
                carro.modelo = (char*)malloc(50 * sizeof(char));
                scanf("%s", carro.modelo);
                printf("Digite a marca do carro: ");
                carro.marca = (char*)malloc(50 * sizeof(char));
                scanf("%s", carro.marca);
                printf("Digite o ano do carro: ");
                scanf("%d", &carro.ano);
                printf("Digite a diária do carro: ");
                scanf("%f", &carro.diaria);
                printf("Digite a categoria do carro: ");
                carro.categoria = (char*)malloc(50 * sizeof(char));
                scanf("%s", carro.categoria);
                printf("Digite a disponibilidade (1 para disponível, 0 para não disponível): ");
                scanf("%d", &carro.disponivel);

                long offset = inserir_registro(&carro, nome_arquivo);
                if (offset != -1) {
                    inserir_chave(arvore, carro.placa, offset);
                    printf("Registro inserido com sucesso.\n");
                } else {
                    printf("Erro ao inserir o registro.\n");
                }

                free(carro.modelo);
                free(carro.marca);
                free(carro.categoria);
                break;
            }
            case 3: {
                char placa[8];
                printf("Digite a placa do carro a ser buscada: ");
                scanf("%s", placa);
                long offset = buscar_chave(arvore, placa);
                if (offset != -1) {
                    Carro* carro = buscar_registro(offset, nome_arquivo);
                    printf("Registro encontrado: \n");
                    printf("Placa: %s, Modelo: %s, Marca: %s, Ano: %d, Diária: %.2f, Categoria: %s, Disponível: %d\n",
                           carro->placa, carro->modelo, carro->marca,
                           carro->ano, carro->diaria, carro->categoria, carro->disponivel);
                    free(carro->modelo);
                    free(carro->marca);
                    free(carro->categoria);
                    free(carro);
                } else {
                    printf("Registro não encontrado.\n");
                }
                break;
            }
            case 4: {
                char placa[8];
                printf("Digite a placa do carro a ser atualizado: ");
                scanf("%s", placa);
                long offset = buscar_chave(arvore, placa);
                if (offset != -1) {
                    Carro carro;
                    printf("Digite o novo modelo do carro: ");
                    carro.modelo = (char*)malloc(50 * sizeof(char));
                    scanf("%s", carro.modelo);
                    printf("Digite a nova marca do carro: ");
                    carro.marca = (char*)malloc(50 * sizeof(char));
                    scanf("%s", carro.marca);
                    printf("Digite o novo ano do carro: ");
                    scanf("%d", &carro.ano);
                    printf("Digite a nova diária do carro: ");
                    scanf("%f", &carro.diaria);
                    printf("Digite a nova categoria do carro: ");
                    carro.categoria = (char*)malloc(50 * sizeof(char));
                    scanf("%s", carro.categoria);
                    printf("Digite a nova disponibilidade (1 para disponível, 0 para não disponível): ");
                    scanf("%d", &carro.disponivel);

                    strcpy(carro.placa, placa); // Manter a mesma placa
                    atualizar_registro(offset, &carro, nome_arquivo);
                    printf("Registro atualizado com sucesso.\n");

                    free(carro.modelo);
                    free(carro.marca);
                    free(carro.categoria);
                } else {
                    printf("Registro não encontrado.\n");
                }
                break;
            }
            case 5: {
                char placa[8];
                printf("Digite a placa do carro a ser removido: ");
                scanf("%s", placa);
                long offset = buscar_chave(arvore, placa);
                if (offset != -1) {
                    remover_chave(arvore, placa);
                    printf("Registro removido com sucesso.\n");
                } else {
                    printf("Registro não encontrado.\n");
                }
                break;
            }
            case 6: {
                printf("Listando todas as placas dos carros:\n");
                listar_todas_as_chaves(arvore);
                break;
            }
            case 7: {
                salvarDadosNoArquivo(nome_arquivo, arvore); // Salavar dados antes de sair
                escreverArvoreBNoArquivo(arvore, "arvoreB.txt"); // Salvar a árvore b
                liberar_arvoreB(arvore); // Libera memória da árvore-B
                printf("Saindo do programa.\n");
                exit(0);
            }
            default: {
                printf("Opção inválida. Tente novamente.\n");
                break;
            }
        }
    }

    return 0;
}

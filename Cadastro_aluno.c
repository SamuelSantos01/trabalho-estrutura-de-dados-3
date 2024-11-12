#include "Cadastro.h"

// Funçções para a manipulação de arquivos de dados
void SalvarArvoreBinariaNoArquivo(NodoArvoreB* nodo, FILE* arquivo) {
    if (nodo != NULL) {
        fwrite(&nodo->numeroDeChaves, sizeof(int), 1, arquivo);
        fwrite(&nodo->ehFolha, sizeof(int), 1, arquivo);

        // Escreve todas as chaves do nó
        for (int i = 0; i < nodo->numeroDeChaves; i++) {
            fwrite(nodo->chaves[i], sizeof(ElementoNodo), 1, arquivo);
        }

        // Escreve os filhos, se não for uma folha
        if (!nodo->ehFolha) {
            for (int i = 0; i <= nodo->numeroDeChaves; i++) {
                SalvarArvoreBinariaNoArquivo(nodo->filhos[i], arquivo);
            }
        }
    }
}

void SalvarArvoreBinaria(NodoArvoreB* raiz) {
    FILE* arquivo = fopen("arvore_alunos.bin", "wb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo binário para salvar.\n");
        return;
    }
    SalvarArvoreBinariaNoArquivo(raiz, arquivo);
    fclose(arquivo);
    printf("Árvore salva no arquivo binário.\n");
}

void SalvarArvoreNoArquivo(NodoArvoreB* nodo, FILE* arquivo) {
    if (nodo != NULL) {
        for (int i = 0; i < nodo->numeroDeChaves; i++) {
            fprintf(arquivo, "%llu,%s\n", nodo->chaves[i]->chavePrimaria, nodo->chaves[i]->nome);
        }
        if (!nodo->ehFolha) {
            for (int i = 0; i <= nodo->numeroDeChaves; i++) {
                SalvarArvoreNoArquivo(nodo->filhos[i], arquivo);
            }
        }
    }
}

NodoArvoreB* CarregarArvoreBinariaDoArquivo(FILE* arquivo) {
    NodoArvoreB* nodo = (NodoArvoreB*)malloc(sizeof(NodoArvoreB));

    if (fread(&nodo->numeroDeChaves, sizeof(int), 1, arquivo) == 0) {
        free(nodo);
        return NULL;
    }

    fread(&nodo->ehFolha, sizeof(int), 1, arquivo);

    for (int i = 0; i < nodo->numeroDeChaves; i++) {
        nodo->chaves[i] = (ElementoNodo*)malloc(sizeof(ElementoNodo));
        fread(nodo->chaves[i], sizeof(ElementoNodo), 1, arquivo);
    }

    if (!nodo->ehFolha) {
        for (int i = 0; i <= nodo->numeroDeChaves; i++) {
            nodo->filhos[i] = CarregarArvoreBinariaDoArquivo(arquivo);
        }
    } else {
        for (int i = 0; i <= nodo->numeroDeChaves; i++) {
            nodo->filhos[i] = NULL;
        }
    }

    return nodo;
}

void CarregarArvoreBinaria(NodoArvoreB** raiz) {
    FILE* arquivo = fopen("arvore_alunos.bin", "rb");
    if (arquivo == NULL) {
        printf("Nenhum arquivo binário encontrado. Iniciando nova árvore.\n");
        *raiz = NULL;
    } else {
        *raiz = CarregarArvoreBinariaDoArquivo(arquivo);
        fclose(arquivo);
        printf("Árvore carregada a partir do arquivo binário.\n");
    }
}

void SalvarArvore(NodoArvoreB* raiz) {
    FILE* arquivo = fopen("dados_alunos.csv", "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para salvar.\n");
        return;
    }
    SalvarArvoreNoArquivo(raiz, arquivo);
    fclose(arquivo);
}

void CarregarArvore(NodoArvoreB** raiz) {
    FILE* arquivo = fopen("dados_alunos.csv", "r");
    if (arquivo == NULL) {
        printf("Nenhum arquivo de dados encontrado. Iniciando nova árvore.\n");
        return;
    }

    char linha[100];
    while (fgets(linha, sizeof(linha), arquivo)) {
        unsigned long long chavePrimaria;
        char nome[100];
        scanf(linha, "%llu,%99[^\n]", &chavePrimaria, nome);
        Inserir(raiz, nome); // Insere os alunos lidos na árvore
    }
    fclose(arquivo);
}


// Funções de criação de árvores
NodoArvoreB* CriarNodo(bool ehFolha) {
    NodoArvoreB* novoNodo = (NodoArvoreB*)malloc(sizeof(NodoArvoreB));
    novoNodo->ehFolha = ehFolha;
    novoNodo->numeroDeChaves = 0;
    for (int i = 0; i < 2 * T; i++) {
        novoNodo->filhos[i] = NULL;
    }
    for (int i = 0; i < 2 * T - 1; i++) {
        novoNodo->chaves[i] = NULL;
    }
    return novoNodo;
}

ElementoNodo* CriarElementoNodo(const char* nome,unsigned long long chavePrimaria) {
    ElementoNodo* novoElemento = (ElementoNodo*)malloc(sizeof(ElementoNodo));
    strncpy(novoElemento->nome, nome, 100); // Altere para 100, pois 'nome' tem tamanho 100 na sua estrutura.
    novoElemento->chavePrimaria = chavePrimaria;
    novoElemento->proximo = NULL;
    novoElemento->anterior = NULL;
    return novoElemento;
}

// Função para dividir um No
void DividirFilho(NodoArvoreB* pai, int i, NodoArvoreB* filho) {
    NodoArvoreB* novoFilho = CriarNodo(filho->ehFolha);
    novoFilho->numeroDeChaves = T - 1;

    // Copiar os últimos T-1 elementos e filhos para o novo nó
    for (int j = 0; j < T - 1; j++) {
        novoFilho->chaves[j] = filho->chaves[j + T];
    }

    if (!filho->ehFolha) {
        for (int j = 0; j < T; j++) {
            novoFilho->filhos[j] = filho->filhos[j + T];
        }
    }

    filho->numeroDeChaves = T - 1;

    // Inserir o novo filho no pai
    for (int j = pai->numeroDeChaves; j >= i + 1; j--) {
        pai->filhos[j + 1] = pai->filhos[j];
    }

    pai->filhos[i + 1] = novoFilho;

    // Subir a chave do meio para o pai
    for (int j = pai->numeroDeChaves - 1; j >= i; j--) {
        pai->chaves[j + 1] = pai->chaves[j];
    }

    pai->chaves[i] = filho->chaves[T - 1];
    pai->numeroDeChaves++;
}

//Função de inserir em um no não cheio
void InserirNaoCheio(NodoArvoreB* nodo, ElementoNodo* chave) {
    int i = nodo->numeroDeChaves - 1;

    if (nodo->ehFolha) {
        while (i >= 0 && chave->chavePrimaria < nodo->chaves[i]->chavePrimaria) {
            nodo->chaves[i + 1] = nodo->chaves[i];
            i--;
        }
        nodo->chaves[i + 1] = chave;
        nodo->numeroDeChaves++;
    } else {
        while (i >= 0 && chave->chavePrimaria < nodo->chaves[i]->chavePrimaria) {
            i--;
        }
        i++;

        if (nodo->filhos[i]->numeroDeChaves == 2 * T - 1) {
            DividirFilho(nodo, i, nodo->filhos[i]);

            if (chave->chavePrimaria > nodo->chaves[i]->chavePrimaria) {
                i++;
            }
        }
        InserirNaoCheio(nodo->filhos[i], chave);
    }
}


void Inserir(NodoArvoreB** raiz, const char* nome) {
    unsigned long long chavePrimaria = FuncaoHash(nome);
    ElementoNodo* chave = CriarElementoNodo(nome, chavePrimaria);
    NodoArvoreB* r = *raiz;

    // Se a árvore está vazia, cria uma nova raiz
    if (r == NULL) {
        r = CriarNodo(true);  // Cria um nó folha
        *raiz = r;
    }

    if (r->numeroDeChaves == 2 * T - 1) {
        NodoArvoreB* s = CriarNodo(false);
        s->filhos[0] = r;
        DividirFilho(s, 0, r);
        InserirNaoCheio(s, chave);
        *raiz = s;
    } else {
        InserirNaoCheio(r, chave);
    }
}


//Buscar Nos
ElementoNodo* BuscarNos(NodoArvoreB* nodo, unsigned long long chavePrimaria) {
    int i = 0;

    while (i < nodo->numeroDeChaves && chavePrimaria > nodo->chaves[i]->chavePrimaria) {
        i++;
    }

    if (i < nodo->numeroDeChaves && chavePrimaria == nodo->chaves[i]->chavePrimaria) {
        return nodo->chaves[i];
    }

    if (nodo->ehFolha) {
        return NULL;
    } else {
        return BuscarNos(nodo->filhos[i], chavePrimaria);
    }
}

// Funções auxiliares
ElementoNodo* ObterPredecessor(NodoArvoreB* nodo, int indice) {
    NodoArvoreB* atual = nodo->filhos[indice];
    while (!atual->ehFolha) {
        atual = atual->filhos[atual->numeroDeChaves];
    }
    return atual->chaves[atual->numeroDeChaves - 1];
}

ElementoNodo* ObterSucessor(NodoArvoreB* nodo, int indice) {
    NodoArvoreB* atual = nodo->filhos[indice + 1];
    while (!atual->ehFolha) {
        atual = atual->filhos[0];
    }
    return atual->chaves[0];
}

void Preencher(NodoArvoreB* nodo, int indice) {
    if (indice != 0 && nodo->filhos[indice - 1]->numeroDeChaves >= T) {
        EmprestarDoAnterior(nodo, indice);
    } else if (indice != nodo->numeroDeChaves && nodo->filhos[indice + 1]->numeroDeChaves >= T) {
        EmprestarDoProximo(nodo, indice);
    } else {
        if (indice != nodo->numeroDeChaves) {
            Mesclar(nodo, indice);
        } else {
            Mesclar(nodo, indice - 1);
        }
    }
}

void EmprestarDoAnterior(NodoArvoreB* nodo, int indice) {
    NodoArvoreB* filho = nodo->filhos[indice];
    NodoArvoreB* irmao = nodo->filhos[indice - 1];

    for (int i = filho->numeroDeChaves - 1; i >= 0; --i) {
        filho->chaves[i + 1] = filho->chaves[i];
    }

    if (!filho->ehFolha) {
        for (int i = filho->numeroDeChaves; i >= 0; --i) {
            filho->filhos[i + 1] = filho->filhos[i];
        }
    }

    filho->chaves[0] = nodo->chaves[indice - 1];
    if (!nodo->ehFolha) {
        filho->filhos[0] = irmao->filhos[irmao->numeroDeChaves];
    }

    nodo->chaves[indice - 1] = irmao->chaves[irmao->numeroDeChaves - 1];

    filho->numeroDeChaves++;
    irmao->numeroDeChaves--;
}

void EmprestarDoProximo(NodoArvoreB* nodo, int indice) {
    NodoArvoreB* filho = nodo->filhos[indice];
    NodoArvoreB* irmao = nodo->filhos[indice + 1];

    filho->chaves[filho->numeroDeChaves] = nodo->chaves[indice];

    if (!filho->ehFolha) {
        filho->filhos[filho->numeroDeChaves + 1] = irmao->filhos[0];
    }

    nodo->chaves[indice] = irmao->chaves[0];

    for (int i = 1; i < irmao->numeroDeChaves; ++i) {
        irmao->chaves[i - 1] = irmao->chaves[i];
    }

    if (!irmao->ehFolha) {
        for (int i = 1; i <= irmao->numeroDeChaves; ++i) {
            irmao->filhos[i - 1] = irmao->filhos[i];
        }
    }

    filho->numeroDeChaves++;
    irmao->numeroDeChaves--;
}

void Mesclar(NodoArvoreB* nodo, int indice) {
    NodoArvoreB* filho = nodo->filhos[indice];
    NodoArvoreB* irmao = nodo->filhos[indice + 1];

    filho->chaves[T - 1] = nodo->chaves[indice];

    for (int i = 0; i < irmao->numeroDeChaves; ++i) {
        filho->chaves[i + T] = irmao->chaves[i];
    }

    if (!filho->ehFolha) {
        for (int i = 0; i <= irmao->numeroDeChaves; ++i) {
            filho->filhos[i + T] = irmao->filhos[i];
        }
    }

    for (int i = indice + 1; i < nodo->numeroDeChaves; ++i) {
        nodo->chaves[i - 1] = nodo->chaves[i];
    }

    for (int i = indice + 2; i <= nodo->numeroDeChaves; ++i) {
        nodo->filhos[i - 1] = nodo->filhos[i];
    }

    filho->numeroDeChaves += irmao->numeroDeChaves + 1;
    nodo->numeroDeChaves--;

    free(irmao);
}


// ===== FUNÇÕES DE REMOÇÃO DE UM ELEMENTO =====

void RemoverDaFolha(NodoArvoreB* nodo, int indice) {
    for (int i = indice + 1; i < nodo->numeroDeChaves; ++i) {
        nodo->chaves[i - 1] = nodo->chaves[i];
    }
    nodo->numeroDeChaves--;
}

void RemoverDeNaoFolha(NodoArvoreB* nodo, int indice) {
    ElementoNodo* chave = nodo->chaves[indice];

    if (nodo->filhos[indice]->numeroDeChaves >= T) {
        ElementoNodo* pred = ObterPredecessor(nodo, indice);
        nodo->chaves[indice] = pred;
        RemoverChave(&(nodo->filhos[indice]), pred->chavePrimaria);
    } else if (nodo->filhos[indice + 1]->numeroDeChaves >= T) {
        ElementoNodo* succ = ObterSucessor(nodo, indice);
        nodo->chaves[indice] = succ;
        RemoverChave(&(nodo->filhos[indice + 1]), succ->chavePrimaria);
    } else {
        Mesclar(nodo, indice);
        RemoverChave(&(nodo->filhos[indice]), chave->chavePrimaria);
    }
}

void RemoverChave(NodoArvoreB** raiz, unsigned long long chavePrimaria) {
    NodoArvoreB* nodo = *raiz;
    int indice = 0;
    while (indice < nodo->numeroDeChaves && nodo->chaves[indice]->chavePrimaria < chavePrimaria) {
        ++indice;
    }

    if (indice < nodo->numeroDeChaves && nodo->chaves[indice]->chavePrimaria == chavePrimaria) {
        if (nodo->ehFolha) {
            RemoverDaFolha(nodo, indice);
        } else {
            RemoverDeNaoFolha(nodo, indice);
        }
    } else {
        if (nodo->ehFolha) {
            printf("A chave %llu não está presente na árvore.\n", chavePrimaria);
            return;
        }

        int flag = (indice == nodo->numeroDeChaves);

        if (nodo->filhos[indice]->numeroDeChaves < T) {
            Preencher(nodo, indice);
        }

        if (flag && indice > nodo->numeroDeChaves) {
            RemoverChave(&(nodo->filhos[indice - 1]), chavePrimaria);
        } else {
            RemoverChave(&(nodo->filhos[indice]), chavePrimaria);
        }
    }

    // Atualiza a raiz caso esteja vazia após a exclusão
    if (*raiz != NULL && (*raiz)->numeroDeChaves == 0) {
        NodoArvoreB* temporario = *raiz;
        if ((*raiz)->ehFolha) {
            *raiz = NULL;
        } else {
            *raiz = (*raiz)->filhos[0];
        }
        free(temporario);
    }
}


// ===== FUNÇÃO DE EXIBIÇÃO DA ÁRVORE =====
void ExibirArvore(NodoArvoreB* nodo, int profundidade, const char* posicao) {
    if (nodo != NULL) {
        printf("%*s%s:\n", profundidade * 4, "", posicao); 
        for (int i = 0; i < nodo->numeroDeChaves; i++) {
            printf("%*s(%s, %llu)\n", (profundidade + 1) * 4, "", nodo->chaves[i]->nome, nodo->chaves[i]->chavePrimaria);
        }
        if (!nodo->ehFolha) {
            for (int i = 0; i <= nodo->numeroDeChaves; i++) {
                if (i == 0) {
                    ExibirArvore(nodo->filhos[i], profundidade + 1, "Esquerda");
                } else if (i == nodo->numeroDeChaves) {
                    ExibirArvore(nodo->filhos[i], profundidade + 1, "Direita");
                } else {
                    ExibirArvore(nodo->filhos[i], profundidade + 1, "Meio");
                }
            }
        }
    }
}

void ExibirEmOrdem(NodoArvoreB* nodo) {
    if (nodo != NULL) {
        // Percorre todos os filhos da esquerda até o meio
        for (int i = 0; i < nodo->numeroDeChaves; i++) {
            // Primeiro, visita a subárvore à esquerda da chave
            if (!nodo->ehFolha) {
                ExibirEmOrdem(nodo->filhos[i]);
            }
            // Depois, imprime a chave atual
            printf("--- Título: %s, Chave Primária: %llu\n", nodo->chaves[i]->nome, nodo->chaves[i]->chavePrimaria);
        }
        
        // Após as chaves, percorre o último filho à direita
        if (!nodo->ehFolha) {
            ExibirEmOrdem(nodo->filhos[nodo->numeroDeChaves]);
        }
    }
}

// ===== FUNÇÃO HASH PARA GERAR UMA CHAVE PRIMÁRIA =====
// Função que converte uma string para seus valores ASCII e retorna a soma dos produtos
unsigned long long FuncaoHash(const char *titulo) {
    unsigned long long valorHash = 0;
    size_t comprimento = strlen(titulo);

    // Constante para o cálculo do hash
    const unsigned long long constanteCalc = 31;

    for (size_t i = 0; i < 8; i++) {
        int valorAscii = (int)titulo[i];  // Valor ASCII do caractere
        valorHash = valorHash * constanteCalc + valorAscii;
    }

    return valorHash;
}

void TestarHash(char *titulo) {
    unsigned long long hash = FuncaoHash(titulo);
    
    if (hash != 0) {
        printf("Resultado do Hash: %llu\n", hash);  // Uso correto de %llu para unsigned long long
    }
}

// ===== INTERAÇÃO COM O USUÁRIO =====
void Menu() {
    NodoArvoreB* raiz = NULL;
    CarregarArvoreBinaria(&raiz);  // Carrega a árvore a partir do arquivo binário, se existir
    int opcao;
    char nome[50];
    unsigned long long chavePrimaria;

    do {
        printf("\nMenu:\n");
        printf("1. Adicionar elemento\n");
        printf("2. Remover elemento\n");
        printf("3. Buscar elemento\n");
        printf("4. Exibir árvore\n");
        printf("5. Listar dados ordenados por chave primária\n");
        printf("0. Sair\n");
        printf("--> Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                printf("Digite o nome do aluno: ");
                scanf(" %[^\n]s", nome);
                Inserir(&raiz, nome);
                printf("Elemento adicionado com sucesso!\n");
                break;

            case 2:
                printf("Digite a chave primária do elemento para remover: ");
                scanf("%llu", &chavePrimaria);
                RemoverChave(&raiz, chavePrimaria);
                printf("Elemento removido com sucesso!\n");
                break;

            case 3:
                printf("Digite a chave primária do elemento para buscar: ");
                scanf("%llu", &chavePrimaria);
                ElementoNodo* encontrado = BuscarNos(raiz, chavePrimaria);
                if (encontrado != NULL) {
                    printf("Elemento encontrado: (%s, %llu)\n", encontrado->nome, encontrado->chavePrimaria);
                } else {
                    printf("Elemento com chave %llu não encontrado.\n", chavePrimaria);
                }
                break;

            case 4:
                printf("Árvore B:\n");
                ExibirArvore(raiz, 0, "Raiz");
                break;

            case 5:
                ExibirEmOrdem(raiz);
                break;

            case 0:
                printf("Saindo...\n");
                SalvarArvore(raiz);  // Salva a árvore no arquivo CSV ao sair
                SalvarArvoreBinaria(raiz);     // Salva a árvore no arquivo binário
                printf("Arquivos salvos!!\n");

                break;

            default:
                printf("Opção inválida. Tente novamente.\n");
                break;
        }
    } while (opcao != 0);
}

int main() {
    system("chcp 65001");
    Menu();

    return 0;
}

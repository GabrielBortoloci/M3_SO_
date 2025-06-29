#include "filesystem.h"
#include <stdbool.h>

BTree* btree_create() {
    
    BTree* tree = malloc(sizeof(BTree));
    tree->root = NULL;
    return tree;
}

TreeNode* create_txt_file(const char* name, const char* content) {
    
    //verificação do tamanho do arquivo
    size_t content_size = strlen(content);

    
    if (content_size > MAX_FILE_SIZE) {
        printf("ERRO: conteúdo do arquivo excede 1MB.\n");
        return NULL;
    }

    File* file = malloc(sizeof(File));
    file->name = strdup(name);
    file->content = strdup(content);
    file->size = strlen(content);

    TreeNode* node = malloc(sizeof(TreeNode));
    node->name = strdup(name);
    node->type = FILE_TYPE;
    node->data.file = file;
    return node;
}

TreeNode* create_directory(const char* name) {
    
    Directory* dir = malloc(sizeof(Directory));
    dir->tree = btree_create();

    TreeNode* node = malloc(sizeof(TreeNode));
    node->name = strdup(name);
    node->type = DIRECTORY_TYPE;
    node->data.directory = dir;
    return node;
}

void delete_txt_file(BTree* tree, const char* name) {
    if (!tree || !tree->root) {
        printf("Árvore inválida ou vazia.\n");
        return;
    }

    TreeNode* node = btree_search(tree, name);
    if (!node) {
        printf("Erro: Arquivo '%s' não encontrado.\n", name);
        return;
    }

    if (node->type != FILE_TYPE) {
        printf("Erro: '%s' não é um arquivo.\n", name);
        return;
    }

    // Remove da árvore
    btree_delete(tree, name);

    // Libera memória usada pelo conteúdo do arquivo
    free(node->data.file->content);
    free(node->data.file);
    free(node);

    printf("Arquivo '%s' deletado com sucesso.\n", name);
}

// verifica se o diretório esta vazio
bool directory_vazio(TreeNode* dir_node) {
    
    if (dir_node->type != DIRECTORY_TYPE) return false;
    return dir_node->data.directory->tree->root->num_keys == 0;
}

void delete_directory(BTree* tree, const char* name) {
    
    if (!tree || !tree->root) {
        printf("Árvore inválida ou vazia.\n");
        return;
    }

    TreeNode* node = btree_search(tree, name);
    if (!node) {
        printf("Erro: Diretório '%s' não encontrado.\n", name);
        return;
    }

    if (node->type != DIRECTORY_TYPE) {
        printf("Erro: '%s' não é um diretório.\n", name);
        return;
    }

    if (!directory_vazio(node)) {
        printf("Erro: Diretório '%s' não está vazio.\n", name);
        return;
    }

    // Chama a função que faz a remoção real do nó na B-Tree
    btree_delete(tree, name);

    printf("Diretório '%s' deletado com sucesso.\n", name);
}

Directory* get_root_directory() {
    
    Directory* root = malloc(sizeof(Directory));
    root->tree = btree_create();
    return root;
}

void change_directory(Directory** current, const char* path) {
    printf("Mudando para o diretório: %s (simulado)\n", path);
}

void list_directory_contents(Directory* dir) {
     
    if (!dir || !dir->tree) {
        printf("Diretório inválido ou vazio.\n");
        return;
    }

    printf("Conteúdo do diretório:\n");
    btree_traverse(dir->tree);
}

TreeNode* btree_search(BTree* tree, const char* name) {
    
    if(tree == NULL || tree->root == NULL) {
        printf("Árvore vazia.\n");
        return NULL;
    }

    printf("Buscando: %s\n", name);

    BTreeNode* current = tree->root;

    while (current != NULL) {
        int i = 0;

        // Procura a primeira chave maior e igual ao nome
        while (i < current->num_keys && strcmp(name, current->keys[i]->name) > 0) {
            i++;
        }

        
        // Se encontrou a chave
        if (i < current->num_keys && strcmp(name, current->keys[i]->name) == 0) {
            printf("\nAPALMEIRASSSS\n");
            return current->keys[i];
        }

        // Se é folha, não encontrou
        if (current->folha) {
            printf("\nErro: arquivo '%s' não encontrado.\n", name);
            printf("blau");
            return NULL;
        }

        // Vai para o filho apropriado
        current = current->filho[i];
    }
    printf("\nErro: arquivo '%s' não encontrado.\n", name);
    return NULL;
}


int compare_keys(const char* key1, const char* key2) {
    return strcmp(key1, key2);
}


//Função para dividir os nós
void split(BTreeNode* pai, int i, BTreeNode* filho_cheio) {
    
    int t = BTREE_ORDER;

    BTreeNode* novo_filho = malloc(sizeof(BTreeNode));
    novo_filho->folha = filho_cheio->folha;
    novo_filho->num_keys = t - 1;

    // Copia as chaves para o novo nó
    for (int j = 0; j < t - 1; j++) {
        novo_filho->keys[j] = filho_cheio->keys[j + t];
    }

    // Copia os filhos, se não for folha
    if (!filho_cheio->folha) {
        for (int j = 0; j < t; j++) {
            novo_filho->filho[j] = filho_cheio->filho[j + t];
        }
    }

    filho_cheio->num_keys = t - 1;

    // Move os filhos do pai para abrir espaço
    for (int j = pai->num_keys; j >= i + 1; j--) {
        pai->filho[j + 1] = pai->filho[j];
    }
    pai->filho[i + 1] = novo_filho;

    // Move as chaves do pai
    for (int j = pai->num_keys - 1; j >= i; j--) {
        pai->keys[j + 1] = pai->keys[j];
    }

    // Sobe a chave do meio do filho cheio
    pai->keys[i] = filho_cheio->keys[t - 1];
    pai->num_keys++;
}

//Função para inserir em nós não cheios
void insert_nonfull(BTreeNode* node, TreeNode* k) {
    
    int i = node->num_keys - 1;

    if (node->folha) {
        // Move as chaves maiores para abrir espaço
        while (i >= 0 && compare_keys(k->name, node->keys[i]->name) < 0) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }

        // Verifica duplicata
        if (i >= 0 && compare_keys(k->name, node->keys[i]->name) == 0) {
            printf("\nErro: Arquivo com nome '%s' já existe.\n", k->name);
            return;
        }

        node->keys[i + 1] = k;
        node->num_keys++;
    } else {
        // Encontra o filho apropriado
        while (i >= 0 && compare_keys(k->name, node->keys[i]->name) < 0) {
            i--;
        }
        i++;

        // Verifica duplicata no ponto atual
        if (i < node->num_keys && compare_keys(k->name, node->keys[i]->name) == 0) {
            printf("\nErro: Arquivo com nome '%s' já existe.\n", k->name);
            return;
        }

        // Se o filho estiver cheio, divide
        if (node->filho[i]->num_keys == 2 * BTREE_ORDER - 1) {
            split(node, i, node->filho[i]);

            if (compare_keys(k->name, node->keys[i]->name) > 0) {
                i++;
            }
        }

        insert_nonfull(node->filho[i], k);
    }
}

void btree_insert(BTree* tree, TreeNode* k) {
    //printf("Inserindo: %s (simulado)\n", node->name);
    if (!tree->root) {
        // Cria raiz
        tree->root = malloc(sizeof(BTreeNode));
        tree->root->folha = 1;
        tree->root->num_keys = 1;
        tree->root->keys[0] = k;
        return;
    }

    BTreeNode* root = tree->root;

    // Verifica duplicata na raiz
    for (int i = 0; i < root->num_keys; i++) {
        if (compare_keys(k->name, root->keys[i]->name) == 0) {
            printf("\nErro: Arquivo com nome '%s' já existe.\n", k->name);
            return;
        }
    }

    if (root->num_keys == 2 * BTREE_ORDER - 1) {
        // Cria nova raiz
        BTreeNode* new_root = malloc(sizeof(BTreeNode));
        new_root->folha = 0;
        new_root->num_keys = 0;
        new_root->filho[0] = root;
        tree->root = new_root;

        split(new_root, 0, root);

        insert_nonfull(new_root, k);
    } else {
        insert_nonfull(root, k);
    }
}




void btree_delete_node(BTreeNode* node, const char* name) {
    int i = 0;
    while (i < node->num_keys && strcmp(name, node->keys[i]->name) > 0)
        i++;

    // Caso 1: A chave está neste nó
    if (i < node->num_keys && strcmp(name, node->keys[i]->name) == 0) {
        TreeNode* valida = node->keys[i];

        // Diretório: verifica se está vazio
        if (valida->type == DIRECTORY_TYPE && !directory_vazio(valida)) {
            printf("Erro: Diretório '%s' não está vazio.\n", name);
            return;
        }

        // Remove deslocando
        for (int j = i; j < node->num_keys - 1; j++) {
            node->keys[j] = node->keys[j + 1];
        }
        node->num_keys--;

        printf("%s '%s' deletado com sucesso.\n",
               valida->type == DIRECTORY_TYPE ? "Diretório" : "Arquivo", name);
        return;
    }

    // Caso 2: Se nó é folha e não achou a chave
    if (node->folha) {
        printf("Erro: '%s' não encontrado.\n", name);
        return;
    }

    // Caso 3: Repetir busca no filho apropriado
    btree_delete_node(node->filho[i], name);
}


void btree_delete(BTree* tree, const char* name) {
    if (!tree || !tree->root) {
        printf("Erro: Árvore vazia.\n");
        return;
    }

    btree_delete_node(tree->root, name);

    // Se após deletar a raiz ficou sem chaves e tem um filho, sobe o filho como nova raiz
    if (tree->root->num_keys == 0 && !tree->root->folha) {
        tree->root = tree->root->filho[0];
    }
}


void btree_traverse_node(BTreeNode* node) {
    if (!node) return;

    for (int i = 0; i < node->num_keys; i++) {
        // Visita filho à esquerda antes da chave
        if (!node->folha) {
            btree_traverse_node(node->filho[i]);
        }

        // Imprime o nome da chave (arquivo ou diretório)
        printf("  - %s\n", node->keys[i]->name);

        // (opcional) Se for diretório, poderia listar recursivamente aqui
    }

    // Visita último filho
    if (!node->folha) {
        btree_traverse_node(node->filho[node->num_keys]);
    }
}

void btree_traverse(BTree* tree) {
    if (tree && tree->root) {
        btree_traverse_node(tree->root);
    } else {
        printf("  [vazio]\n");
    }
}
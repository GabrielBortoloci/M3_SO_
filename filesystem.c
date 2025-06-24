#include "filesystem.h"
#include <string.h>
#define MAX_FILE_SIZE (1024 * 1024) // Definindo arquivos para 1MB
BTree* btree_create() {
    BTree* tree = malloc(sizeof(BTree));
    tree->root = NULL;
    return tree;
}

TreeNode* create_txt_file(const char* name, const char* content) {
    size_t = content_size = strlen(content);
    if(content_size > MAX_FILE_SIZE) {
        cout << "ERRO. Conteúdo do arquivo maior que 1MB." << endl;
        return NULL;
    } 
    
    File* file = malloc(sizeof(File));
    file->name = strdup(name);
    file->content = strdup(content);
    file->size = content_size;

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
    printf("Arquivo '%s' deletado (simulado)\n", name);
}

void delete_directory(BTree* tree, const char* name) {
    printf("Diretório '%s' deletado (simulado)\n", name);
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
    printf("Conteúdo do diretório:\n");
    btree_traverse(dir->tree);
}

TreeNode* btree_search(BTree* tree, const char* name) {
    printf("Buscando: %s (simulado)\n", name);
    return NULL;
}

void btree_insert(BTree* tree, TreeNode* node) {
    printf("Inserindo: %s (simulado)\n", node->name);
}

void btree_delete(BTree* tree, const char* name) {
    printf("Removendo: %s (simulado)\n", name);
}

void btree_traverse(BTree* tree) {
    printf("[Exemplo] arquivo.txt\n");
}
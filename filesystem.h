#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BTREE_ORDER 3 

typedef enum { FILE_TYPE, DIRECTORY_TYPE } NodeType;

typedef struct File {
    char* name;
    char* content;
    size_t size;
} File;

struct Directory;

typedef struct TreeNode {
    char* name;
    NodeType type;
    union {
        File* file;
        struct Directory* directory;
    } data;
} TreeNode;

typedef struct BTreeNode {
    int num_keys;
    TreeNode* keys[2 * BTREE_ORDER - 1];
    struct BTreeNode* children[2 * BTREE_ORDER];
    int leaf;
} BTreeNode;

typedef struct BTree {
    BTreeNode* root;
} BTree;

typedef struct Directory {
    BTree* tree;
    struct Directory* parent;
    char* name; // adicionei nome para facilitar na navegação entre diretorios 
} Directory;

// Funções
BTree* btree_create();
BTreeNode* btree_node_create(int leaf);
void btree_split_child(BTreeNode* parent, int i, BTreeNode* child);
void btree_insert_non_full(BTreeNode* node, TreeNode* key);
void btree_insert(BTree* tree, TreeNode* node);
void btree_delete(BTree* tree, const char* name);
int find_key_index(BTreeNode* node, const char* name);
TreeNode* get_ante(BTreeNode* node, int idx);
TreeNode* get_prox(BTreeNode* node, int idx);
void enche_no(BTreeNode* node, int idx);
void empresta_ante(BTreeNode* node, int idx);
void empresta_prox(BTreeNode* node, int idx);
void junta_no(BTreeNode* node, int idx);
void deleta_recursivo(BTreeNode* node, const char* name);
TreeNode* btree_search(BTree* tree, const char* name);
void btree_traverse(BTree* tree);
TreeNode* create_txt_file(const char* name, const char* content);
TreeNode* create_directory(const char* name);
void delete_txt_file(BTree* tree, const char* name);
void delete_directory(BTree* tree, const char* name);
Directory* get_root_directory();
void change_directory(Directory** current, Directory* root, const char* path);
void list_directory_contents(Directory* dir);
void imprime_arvore(Directory* root_dir);
void gera_arq_fs(Directory* root_dir, const char* filename);

#endif
#include "filesystem.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

BTree* btree_create() {
    BTree* tree = (BTree*)malloc(sizeof(BTree));
    tree->root = NULL;
    return tree;
}

BTreeNode* btree_node_create(int leaf) {
    BTreeNode* newNode = (BTreeNode*)malloc(sizeof(BTreeNode));
    newNode->num_keys = 0;
    newNode->leaf = leaf;
    for (int i = 0; i < 2 * BTREE_ORDER; i++) {
        newNode->children[i] = NULL;
    }
    return newNode;
}

Directory* get_root_directory() {
    Directory* root = (Directory*)malloc(sizeof(Directory));
    root->tree = btree_create();
    root->parent = NULL;
    root->name = strdup("root"); 
    return root;
}

void btree_split_child(BTreeNode* parent, int i, BTreeNode* child) {
    BTreeNode* new_node = btree_node_create(child->leaf);
    new_node->num_keys = BTREE_ORDER - 1;

    for (int j = 0; j < BTREE_ORDER - 1; j++) {
        new_node->keys[j] = child->keys[j + BTREE_ORDER];
    }

    if (!child->leaf) {
        for (int j = 0; j < BTREE_ORDER; j++) {
            new_node->children[j] = child->children[j + BTREE_ORDER];
        }
    }

    child->num_keys = BTREE_ORDER - 1;

    for (int j = parent->num_keys; j >= i + 1; j--) {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[i + 1] = new_node;

    for (int j = parent->num_keys - 1; j >= i; j--) {
        parent->keys[j + 1] = parent->keys[j];
    }
    parent->keys[i] = child->keys[BTREE_ORDER - 1];
    parent->num_keys++;
}

void btree_insert_non_full(BTreeNode* node, TreeNode* key) {
    int i = node->num_keys - 1;
    if (node->leaf) {
        while (i >= 0 && strcmp(key->name, node->keys[i]->name) < 0) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
    } else {
        while (i >= 0 && strcmp(key->name, node->keys[i]->name) < 0) {
            i--;
        }
        i++;
        if (node->children[i]->num_keys == 2 * BTREE_ORDER - 1) {
            btree_split_child(node, i, node->children[i]);
            if (strcmp(key->name, node->keys[i]->name) > 0) {
                i++;
            }
        }
        btree_insert_non_full(node->children[i], key);
    }
}

void btree_insert(BTree* tree, TreeNode* node) {
    if (tree->root == NULL) {
        tree->root = btree_node_create(1);
        tree->root->keys[0] = node;
        tree->root->num_keys = 1;
    } else {
        if (tree->root->num_keys == 2 * BTREE_ORDER - 1) {
            BTreeNode* s = btree_node_create(0);
            s->children[0] = tree->root;
            btree_split_child(s, 0, tree->root);
            int i = 0;
            if (strcmp(node->name, s->keys[0]->name) > 0) {
                i++;
            }
            btree_insert_non_full(s->children[i], node);
            tree->root = s;
        } else {
            btree_insert_non_full(tree->root, node);
        }
    }
}

TreeNode* create_txt_file(const char* name, const char* content) {
    File* file = (File*)malloc(sizeof(File));
    file->name = strdup(name);
    file->content = strdup(content);
    file->size = strlen(content);

    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    node->name = strdup(name);
    node->type = FILE_TYPE;
    node->data.file = file;
    return node;
}

TreeNode* create_directory(const char* name) {
    Directory* dir = (Directory*)malloc(sizeof(Directory));
    dir->tree = btree_create();
    dir->parent = NULL;
    dir->name = strdup(name);

    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    node->name = strdup(name);
    node->type = DIRECTORY_TYPE;
    node->data.directory = dir;
    return node;
}

TreeNode* btree_search_recursive(BTreeNode* node, const char* name) {
    int i = 0;
    while (i < node->num_keys && strcmp(name, node->keys[i]->name) > 0) {
        i++;
    }

    if (i < node->num_keys && strcmp(name, node->keys[i]->name) == 0) {
        return node->keys[i];
    }

    if (node->leaf) {
        return NULL;
    } else {
        return btree_search_recursive(node->children[i], name);
    }
}

TreeNode* btree_search(BTree* tree, const char* name) {
    if (tree->root == NULL) {
        return NULL;
    }
    return btree_search_recursive(tree->root, name);
}

int find_key_index(BTreeNode* node, const char* name) {
    int idx = 0;
    while (idx < node->num_keys && strcmp(node->keys[idx]->name, name) < 0) {
        ++idx;
    }
    return idx;
}

TreeNode* get_antes(BTreeNode* node, int idx) {
    BTreeNode* atual = node->children[idx];
    while (!atual->leaf) {
        atual = atual->children[atual->num_keys];
    }
    return atual->keys[atual->num_keys - 1];
}

TreeNode* get_prox(BTreeNode* node, int idx) {
    BTreeNode* atual = node->children[idx + 1];
    while (!atual->leaf) {
        atual = atual->children[0];
    }
    return atual->keys[0];
}

void enche_no(BTreeNode* node, int idx) {
    if (idx != 0 && node->children[idx - 1]->num_keys >= BTREE_ORDER) {
        empresta_ante(node, idx);
    } else if (idx != node->num_keys && node->children[idx + 1]->num_keys >= BTREE_ORDER) {
        empresta_prox(node, idx);
    } else {
        if (idx != node->num_keys) {
            junta_no(node, idx);
        } else {
            junta_no(node, idx - 1);
        }
    }
}

void empresta_ante(BTreeNode* node, int idx) {
    BTreeNode* no1 = node->children[idx];
    BTreeNode* no2 = node->children[idx - 1];

    for (int i = no1->num_keys - 1; i >= 0; --i) {
        no1->keys[i + 1] = no1->keys[i];
    }

    if (!no1->leaf) {
        for (int i = no1->num_keys; i >= 0; --i) {
            no1->children[i + 1] = no1->children[i];
        }
    }

    no1->keys[0] = node->keys[idx - 1];
    if (!no1->leaf) {
        no1->children[0] = no2->children[no2->num_keys];
    }

    node->keys[idx - 1] = no2->keys[no2->num_keys - 1];

    no1->num_keys++;
    no2->num_keys--;
}

void empresta_prox(BTreeNode* node, int idx) {
    BTreeNode* no1 = node->children[idx];
    BTreeNode* no2 = node->children[idx + 1];

    no1->keys[no1->num_keys] = node->keys[idx];

    if (!no1->leaf) {
        no1->children[no1->num_keys + 1] = no2->children[0];
    }

    node->keys[idx] = no2->keys[0];

    for (int i = 1; i < no2->num_keys; ++i) {
        no2->keys[i - 1] = no2->keys[i];
    }

    if (!no2->leaf) {
        for (int i = 1; i <= no2->num_keys; ++i) {
            no2->children[i - 1] = no2->children[i];
        }
    }

    no1->num_keys++;
    no2->num_keys--;
}

void junta_no(BTreeNode* node, int idx) {
    BTreeNode* no1 = node->children[idx];
    BTreeNode* no2 = node->children[idx + 1];

    no1->keys[BTREE_ORDER - 1] = node->keys[idx];

    for (int i = 0; i < no2->num_keys; ++i) {
        no1->keys[i + BTREE_ORDER] = no2->keys[i];
    }

    if (!no1->leaf) {
        for (int i = 0; i <= no2->num_keys; ++i) {
            no1->children[i + BTREE_ORDER] = no2->children[i];
        }
    }

    for (int i = idx + 1; i < node->num_keys; ++i) {
        node->keys[i - 1] = node->keys[i];
    }

    for (int i = idx + 2; i <= node->num_keys; ++i) {
        node->children[i - 1] = node->children[i];
    }

    no1->num_keys += no2->num_keys + 1;
    node->num_keys--;

    free(no2);
}

void deleta_recursivo(BTreeNode* node, const char* name) {
    int idx = find_key_index(node, name);

    if (idx < node->num_keys && strcmp(node->keys[idx]->name, name) == 0) {
        if (node->leaf) {
            for (int i = idx + 1; i < node->num_keys; ++i) {
                node->keys[i - 1] = node->keys[i];
            }
            node->num_keys--;
        } else {
            BTreeNode* pred_child = node->children[idx];
            BTreeNode* succ_child = node->children[idx + 1];

            if (pred_child->num_keys >= BTREE_ORDER) {
                TreeNode* pred = get_antes(node, idx);
                node->keys[idx] = pred;
                deleta_recursivo(pred_child, pred->name);
            } else if (succ_child->num_keys >= BTREE_ORDER) {
                TreeNode* succ = get_prox(node, idx);
                node->keys[idx] = succ;
                deleta_recursivo(succ_child, succ->name);
            } else {
                junta_no(node, idx);
                deleta_recursivo(node->children[idx], name);
            }
        }
    } else {
        if (node->leaf) {
            cout << "Chave '" << name << "' não existe na árvore." << endl;
            return;
        }

        int flag = ((idx == node->num_keys) ? 1 : 0);

        if (node->children[idx]->num_keys < BTREE_ORDER) {
            enche_no(node, idx);
        }

        if (flag && idx > node->num_keys) {
            deleta_recursivo(node->children[idx - 1], name);
        } else {
            deleta_recursivo(node->children[idx], name);
        }
    }
}

void btree_delete(BTree* tree, const char* name) {
    if (!tree->root) {
        cout << "A árvore está vazia" << endl;
        return;
    }

    deleta_recursivo(tree->root, name);

    if (tree->root->num_keys == 0) {
        BTreeNode* temp = tree->root;
        if (tree->root->leaf) {
            tree->root = NULL;
        } else {
            tree->root = tree->root->children[0];
        }
        free(temp);
    }
}

void delete_txt_file(BTree* tree, const char* name) {
    TreeNode* no = btree_search(tree, name);
    if (no && no->type == FILE_TYPE) {
        btree_delete(tree, name);
        free(no->data.file->name);
        free(no->data.file->content);
        free(no->data.file);
        free(no->name);
        free(no);
        cout << "Arquivo '" << name << "' deletado com sucesso.\n";
    } else {
        cout << "Arquivo '" << name << "' não encontrado ou não é um arquivo.\n";
    }
}

void delete_directory(BTree* tree, const char* name) {
    TreeNode* no = btree_search(tree, name);
    if (no && no->type == DIRECTORY_TYPE) {
        if (no->data.directory->tree->root == NULL || no->data.directory->tree->root->num_keys == 0) {
            btree_delete(tree, name);

            free(no->data.directory->tree);
            free(no->data.directory);
            free(no->name);
            free(no);
            cout << "Diretório '" << name << "' deletado com sucesso.\n";
        } else {
            cout << "Diretório '" << name << "' não está vazio e não pode ser deletado.\n";
        }
    } else {
        cout << "Diretório '" << name << "' não encontrado ou não é um diretório.\n";
    }
}

void change_directory(Directory** current, Directory* root, const char* path) {
    if (strcmp(path, ".") == 0) {
        return;
    } else if (strcmp(path, "..") == 0) {
        if ((*current)->parent != NULL) {
            *current = (*current)->parent;
            cout << "Mudou para o diretório pai: " << (*current)->name << endl;
        } else {
            cout << "Já está no diretório raiz." << endl;
        }
        return;
    } else if (strcmp(path, "/") == 0 || strcmp(path, "root") == 0) {
        *current = root;
        cout << "Mudou para o diretório raiz: " << (*current)->name << endl;
        return;
    }

    Directory* temp_current;
    char* path_copy = strdup(path);
    char* token;

    if (path[0] == '/') {
        temp_current = root;
        token = strtok(path_copy + 1, "/");
    } else {
        temp_current = *current;
        token = strtok(path_copy, "/");
    }

    while (token != NULL) {
        TreeNode* target_node = btree_search(temp_current->tree, token);
        if (target_node && target_node->type == DIRECTORY_TYPE) {
            target_node->data.directory->parent = temp_current;
            temp_current = target_node->data.directory;
        } else {
            cout << "Caminho '" << path << "' inválido ou diretório '" << token << "' não encontrado." << endl;
            free(path_copy);
            return;
        }
        token = strtok(NULL, "/");
    }

    *current = temp_current;
    cout << "Mudou para o diretório: " << (*current)->name << endl;
    free(path_copy);
}

void list_directory_contents(Directory* dir) {
    cout << "Conteúdo do diretório:" << endl;
    btree_traverse(dir->tree);
}

void btree_traverse_recursive(BTreeNode* node) {
    int i;
    for (i = 0; i < node->num_keys; i++) {
        if (!node->leaf) {
            btree_traverse_recursive(node->children[i]);
        }
        cout << (node->keys[i]->type == FILE_TYPE ? "FILE: " : "DIR: ") << node->keys[i]->name << endl;
    }

    if (!node->leaf) {
        btree_traverse_recursive(node->children[i]);
    }
}

void btree_traverse(BTree* tree) {
    if (tree->root != NULL) {
        btree_traverse_recursive(tree->root);
    }
}

void imprime_arvore_recursivo(BTreeNode* node, int level, ostream& os) {
    int i;
    for (i = 0; i < node->num_keys; i++) {
        if (!node->leaf) {
            imprime_arvore_recursivo(node->children[i], level + 1, os);
        }

        for (int j = 0; j < level; j++) {
            os << "  ";
        }
        os << "|-- " << (node->keys[i]->type == FILE_TYPE ? "FILE: " : "DIR: ") << node->keys[i]->name << endl;

        if (node->keys[i]->type == DIRECTORY_TYPE) {
            if (node->keys[i]->data.directory->tree->root != NULL) {
                imprime_arvore_recursivo(node->keys[i]->data.directory->tree->root, level + 2, os);
            }
        }
    }

    if (!node->leaf) {
        imprime_arvore_recursivo(node->children[i], level + 1, os);
    }
}

void imprime_arvore(Directory* root_dir) {
    cout << endl << "--- Estrutura Completa do Sistema de Arquivos ---" << endl;
    if (root_dir != NULL && root_dir->tree != NULL && root_dir->tree->root != NULL) {
        cout << root_dir->name << endl;
        imprime_arvore_recursivo(root_dir->tree->root, 1, cout);
    } else {
        cout << "Sistema de arquivos vazio." << endl;
    }
    cout << "------------------------------------------------" << endl;
}

void gera_arq_fs(Directory* root_dir, const char* filename) {
    ofstream output_file(filename);
    if (!output_file.is_open()) {
        perror("Erro ao criar o arquivo de imagem");
        return;
    }

    stringstream ss;
    ss << endl << "--- Estrutura Completa do Sistema de Arquivos ---" << endl;
    if (root_dir != NULL && root_dir->tree != NULL && root_dir->tree->root != NULL) {
        ss << root_dir->name << endl;
        imprime_arvore_recursivo(root_dir->tree->root, 1, ss);
    } else {
        ss << "Sistema de arquivos vazio." << endl;
    }
    ss << "------------------------------------------------" << endl;

    output_file << ss.str();
    output_file.close();

    cout << "Imagem do sistema de arquivos gerada em '" << filename << "'." << endl;
}
#include "filesystem.h"


int main() {
    // Criação do sistema de arquivos
   Directory* root = get_root_directory();

    // Diretórios de segundo nível
    TreeNode* dirSO = create_directory("SO");
    TreeNode* dirTEST = create_directory("TEST");
    btree_insert(root->tree, dirSO);
    btree_insert(root->tree, dirTEST);

    printf("Inserido com sucesso\n");
    // Arquivo em SO
    TreeNode* file1 = create_txt_file("arquivo1.txt", "Arquivo de teste de SO.");
    btree_insert(dirSO->data.directory->tree, file1);
    TreeNode* file2 = create_txt_file("arquivo2.txt", "Arquivo de teste de SO.");
    btree_insert(dirSO->data.directory->tree, file2);

    TreeNode* file3 = create_txt_file("documento2.txt", "Documento número 2.");
btree_insert(dirSO->data.directory->tree, file2);

TreeNode* file4 = create_txt_file("notas3.txt", "Estas são as notas do arquivo 3.");
btree_insert(dirSO->data.directory->tree, file3);

TreeNode* file5 = create_txt_file("relatorio4.txt", "Relatório do quarto arquivo.");
btree_insert(dirSO->data.directory->tree, file4);

TreeNode* file6 = create_txt_file("relatorio4.txt", "Dados importantes do arquivo .");
btree_insert(dirSO->data.directory->tree, file5);

TreeNode* file7 = create_txt_file("teste6.txt", "Arquivo de teste número 6.");
btree_insert(dirSO->data.directory->tree, file6);

TreeNode* file8 = create_txt_file("info7.txt", "Informações do arquivo 7.");
btree_insert(dirSO->data.directory->tree, file7);

TreeNode* file9 = create_txt_file("backup8.txt", "Backup do arquivo 8.");
btree_insert(dirSO->data.directory->tree, file8);

TreeNode* file10 = create_txt_file("log9.txt", "Log do arquivo 9.");
btree_insert(dirSO->data.directory->tree, file9);

TreeNode* file11 = create_txt_file("manual10.txt", "Manual do arquivo 10.");
btree_insert(dirSO->data.directory->tree, file10);

    // Listagem
    //printf("\n--- Conteúdo do diretório ROOT ---\n");
    //list_directory_contents(root);

    //printf("\n--- Conteúdo do diretório SO ---\n");
    //list_directory_contents(dirSO->data.directory);

    btree_search(dirSO->data.directory->tree, "info7.txt");
    /* Simulação de persistência
    FILE* img = fopen("fs.img", "w");
    if (img) {
        fprintf(img, "ROOT\n");
        fprintf(img, "├── SO\n");
        fprintf(img, "│   └── arquivo1.txt: Arquivo de teste de SO.\n");
        fprintf(img, "└── TEST\n");
        fclose(img);
        printf("\nSistema de arquivos salvo em fs.img\n");
    } else {
        perror("Erro ao criar fs.img");
    }
    */
    return 0;
}

#include "filesystem.h"
#include "filesystem.cpp"
#include <iostream>
#include <string>
#include <limits>
#include <string.h>
#include <fstream>
#include <sstream>

using namespace std;

#define MAX_NAME_LENGTH 256
#define MAX_PATH_LENGTH 256
#define MAX_CONTENT_LENGTH 1024 * 1024 // 1MB

// Menu
void print_menu() {
    cout << endl << "--- Menu do Sistema de Arquivos ---" << endl;
    cout << "1 - Criar arquivo" << endl;
    cout << "2 - Criar diretório" << endl;
    cout << "3 - Mudar diretório" << endl;
    cout << "4 - Listar conteúdo" << endl;
    cout << "5 - Deletar arquivo" << endl;
    cout << "6 - Deletar diretório" << endl;
    cout << "7 - Imprimir árvore completa" << endl;
    cout << "8 - Criar arquivo fs.img" << endl;
    cout << "9 - Sair" << endl;
    cout << "-----------------------------------" << endl;
}

int main() {
    Directory* root = get_root_directory();
    Directory* dir_atual = root;
    int op1;
    char name[MAX_NAME_LENGTH];
    char cont[MAX_CONTENT_LENGTH];
    char path[MAX_PATH_LENGTH];

    while (true) {
        print_menu();
        cout << "\nSeu diretório atual: " << dir_atual->name << "\n";
        cout << "Escolha uma opção: ";

        cin >> op1;
        if (cin.fail()) {
            cout << "Entrada inválida. Por favor, digite um número.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        

        system("clear");
        switch (op1) {
            case 1:{ // Criar arquivo
                cout << "Digite o caminho do arquivo .txt (ex: /caminho/para/arquivo.txt): ";
                cin.getline(path, MAX_PATH_LENGTH);

                ifstream file(path);
                if (!file.is_open()) {
                    cout << "Erro ao abrir o arquivo '" << path << "'." << endl;
                } else {
                    stringstream buffer;
                    buffer << file.rdbuf();
                    string tam_file = buffer.str();
                    file.close();

                    if (tam_file.length() > MAX_CONTENT_LENGTH) {
                        cout << "O arquivo excede o tamanho máximo permitido (1MB)." << endl;
                    } else {
                        const char* nome_ini = strrchr(path, '/');
                        if (nome_ini == NULL) {
                            nome_ini = path;
                        } else {
                            nome_ini++;
                        }

                       if (btree_search(dir_atual->tree, nome_ini) != NULL) {
                           cout << "Já existe um arquivo ou diretório com o nome '" << nome_ini << "' neste diretório." << endl;
                       } else {
                           char* content_buffer = strdup(tam_file.c_str());
                           if (content_buffer == NULL) {
                               cout << "Erro de alocação de memória para o conteúdo do arquivo." << endl;
                           } else {
                               TreeNode* new_file = create_txt_file(nome_ini, content_buffer);
                               btree_insert(dir_atual->tree, new_file);
                               cout << "Arquivo '" << new_file->name << "' criado a partir de '" << path << "'." << endl;
                               free(content_buffer);
                           }
                       }
                   }
               }
               break;
            }
            case 2:{ // Criar diretório
                cout << "Digite o nome do diretório: ";
                cin.getline(name, MAX_NAME_LENGTH);

                TreeNode* new_dir = create_directory(name);
                btree_insert(dir_atual->tree, new_dir);
                cout << "Diretório '" << name << "' criado." << endl;
                break;
            }
            case 3:{ // Mudar diretório
                cout << "Digite o caminho do diretório (ex: nome_do_diretorio ou .. para voltar): ";
                cin.getline(path, MAX_PATH_LENGTH);

                change_directory(&dir_atual, root, path);
                break;
            }
            case 4:{ // Listar conteúdo
                list_directory_contents(dir_atual);
                break;
            }
            case 5:{ // Deletar arquivo
                cout << "Digite o nome do arquivo a ser deletado: ";
                cin.getline(name, MAX_NAME_LENGTH);

                delete_txt_file(dir_atual->tree, name);
                break;
            }
            case 6:{ // Deletar diretório
                cout << "Digite o nome do diretório a ser deletado: ";
                cin.getline(name, MAX_NAME_LENGTH);

                delete_directory(dir_atual->tree, name);
                break;
            }
            case 7:{ // Imprimir árvore completa
                imprime_arvore(root);
                break;
            }
            case 8:{ // Gerar arquivo fs.img
                gera_arq_fs(root, "fs.img");
                break;
            }
            case 9:{ // Sair
                cout << "Saindo do sistema de arquivos." << endl;
                return 0;
            }
            default:{
                cout << "Opção inválida. Por favor, escolha um número de 1 a 9." << endl;
                break;
            }
        }
    }

    return 0;
}
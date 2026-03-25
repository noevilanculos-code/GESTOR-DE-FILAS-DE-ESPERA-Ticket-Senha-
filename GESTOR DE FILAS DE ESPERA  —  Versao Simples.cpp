#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

const int MAX = 100;

// Vetores
int senhas[MAX];
string nomes[MAX];
string servicos[MAX];
bool atendido[MAX];

int inicio = 0;
int fim = 0;
int contador = 1;

// ===============================
// Função para hora actual
// ===============================
string horaAtual() {
    time_t t = time(0);
    tm *now = localtime(&t);

    char buffer[6];
    strftime(buffer, sizeof(buffer), "%H:%M", now);

    return buffer;
}

// ===============================
// 1. Retirar senha
// ===============================
void retirarSenha() {
    if (fim >= MAX) {
        cout << "\nFila cheia!\n";
        return;
    }

    senhas[fim] = contador++;
    atendido[fim] = false;

    cout << "Nome: ";
    getline(cin, nomes[fim]);

    cout << "Servico (Tesouraria / Secretaria / RH / Geral): ";
    getline(cin, servicos[fim]);

    cout << "Hora: " << horaAtual() << "\n";
    cout << "Senha: " << senhas[fim] << " retirada com sucesso!\n";

    fim++;
}

// ===============================
// Gravar no ficheiro
// ===============================
void gravarLog(int i) {
    ofstream f("atendimentos.txt", ios::app);

    if (!f) {
        cout << "Erro ao abrir ficheiro!\n";
        return;
    }

    f << "Senha: " << senhas[i]
      << " | Nome: " << nomes[i]
      << " | Servico: " << servicos[i]
      << " | Hora atendimento: " << horaAtual()
      << "\n";

    f.close();
}

// ===============================
// 2. Chamar proximo
// ===============================
void chamarProximo() {
    while (inicio < fim && atendido[inicio])
        inicio++;

    if (inicio >= fim) {
        cout << "\nFila vazia!\n";
        return;
    }

    atendido[inicio] = true;

    cout << "\n--- ATENDENDO ---\n";
    cout << "Senha: " << senhas[inicio] << "\n";
    cout << "Nome: " << nomes[inicio] << "\n";
    cout << "Servico: " << servicos[inicio] << "\n";

    gravarLog(inicio);

    inicio++;
}

// ===============================
// 3. Ver fila
// ===============================
void verFila() {
    bool vazio = true;

    cout << "\n--- FILA ---\n";

    for (int i = inicio; i < fim; i++) {
        if (!atendido[i]) {
            cout << senhas[i] << " - "
                 << nomes[i] << " ("
                 << servicos[i] << ")\n";
            vazio = false;
        }
    }

    if (vazio)
        cout << "Fila vazia!\n";
}

// ===============================
// MENU
// ===============================
int main() {
    int op;

    do {
        cout << "\n=== GESTOR DE FILA ===\n";
        cout << "1. Retirar senha\n";
        cout << "2. Chamar proximo\n";
        cout << "3. Ver fila\n";
        cout << "0. Sair\n";
        cout << "Opcao: ";
        cin >> op;
        cin.ignore();

        switch (op) {
            case 1: retirarSenha(); break;
            case 2: chamarProximo(); break;
            case 3: verFila(); break;
            case 0: cout << "Encerrado.\n"; break;
            default: cout << "Opcao invalida!\n";
        }

    } while (op != 0);

    return 0;
}

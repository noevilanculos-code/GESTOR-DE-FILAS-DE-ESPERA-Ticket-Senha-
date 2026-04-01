#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

// -- Constantes -----------------------------------------------
const int MAX    = 100;
const int N_SERV = 4;

// -- Vetores da fila -------------------------------------------
int    senhas[MAX];
string nomes[MAX];
string servico[MAX];
string hora_entrada[MAX];
bool   atendido[MAX];

int total    = 0;
int frente   = 0;
int contador = 1;

string SERVICOS[N_SERV] = {
    "Tesouraria",
    "Secretaria",
    "Recursos Humanos",
    "Atendimento Geral"
};

// -- Utilitarios -----------------------------------------------
void limpar() {
    system("cls");
}

string hora_agora() {
    time_t t = time(NULL);
    char buf[6];
    strftime(buf, sizeof(buf), "%H:%M", localtime(&t));
    return buf;
}

string data_hoje() {
    time_t t = time(NULL);
    char buf[12];
    strftime(buf, sizeof(buf), "%d/%m/%Y", localtime(&t));
    return buf;
}

void pausar() {
    cout << "\nPrima ENTER para continuar...";
    cin.ignore();
}

int em_espera() {
    int n = 0;
    for (int i = frente; i < total; i++)
        if (!atendido[i]) n++;
    return n;
}

void cabecalho() {
    limpar();
    cout << "================================\n";
    cout << "  GESTOR DE FILAS DE ESPERA\n";
    cout << "  " << data_hoje() << "  " << hora_agora() << "\n";
    cout << "================================\n";
    cout << "  Total: "     << total
         << "  Atendidos: " << total - em_espera()
         << "  Em espera: " << em_espera() << "\n";
    cout << "================================\n\n";
}

// -- 1. Retirar senha ------------------------------------------
void retirar_senha() {
    cabecalho();

    if (total >= MAX) {
        cout << "[!] Fila cheia!\n";
        pausar(); return;
    }

    senhas[total]       = contador++;
    atendido[total]     = false;
    hora_entrada[total] = hora_agora();

    cout << "Nome do cliente: ";
    getline(cin, nomes[total]);
    if (nomes[total].empty()) nomes[total] = "Anonimo";

    cout << "\nEscolha o servico:\n";
    for (int i = 0; i < N_SERV; i++)
        cout << "  [" << i + 1 << "] " << SERVICOS[i] << "\n";
    cout << "Opcao: ";

    int op;
    cin >> op;
    cin.ignore();

    if (op < 1 || op > N_SERV)
        servico[total] = "Atendimento Geral";
    else
        servico[total] = SERVICOS[op - 1];

    total++;

    cout << "\nSENHA EMITIDA!\n";
    cout << "Numero  : " << senhas[total - 1]      << "\n";
    cout << "Nome    : " << nomes[total - 1]        << "\n";
    cout << "Servico : " << servico[total - 1]      << "\n";
    cout << "Hora    : " << hora_entrada[total - 1] << "\n";
    pausar();
}

// -- 2. Chamar proximo -----------------------------------------
void chamar_proximo() {
    cabecalho();

    while (frente < total && atendido[frente])
        frente++;

    if (frente >= total) {
        cout << "Nao ha clientes em espera.\n";
        pausar(); return;
    }

    atendido[frente] = true;

    cout << ">>> PROXIMO ATENDIMENTO <<<\n";
    cout << "Senha   : " << senhas[frente]      << "\n";
    cout << "Nome    : " << nomes[frente]        << "\n";
    cout << "Servico : " << servico[frente]      << "\n";
    cout << "Entrada : " << hora_entrada[frente] << "\n";
    cout << "Em espera: " << em_espera() - 1     << "\n";

    // Grava no log
    ofstream f("atendimentos.txt", ios::app);
    if (f)
        f << "[" << data_hoje() << "] Senha: " << senhas[frente]
          << " | Entrada: " << hora_entrada[frente]
          << " | Saida: "   << hora_agora()
          << " | Cliente: " << nomes[frente]
          << " | Servico: " << servico[frente] << "\n";

    frente++;
    pausar();
}

// -- 3. Ver fila -----------------------------------------------
void ver_fila() {
    cabecalho();
    cout << "--- FILA ACTUAL ---\n";

    int pos = 1;
    for (int i = frente; i < total; i++) {
        if (!atendido[i]) {
            cout << pos << ". [" << senhas[i] << "] "
                 << nomes[i] << " - " << servico[i]
                 << " (" << hora_entrada[i] << ")";
            if (pos == 1) cout << "  << PROXIMO";
            cout << "\n";
            pos++;
        }
    }

    if (pos == 1) cout << "(fila vazia)\n";
    pausar();
}

// -- 4. Historico ----------------------------------------------
void ver_historico() {
    cabecalho();
    cout << "--- HISTORICO ---\n";

    bool vazio = true;
    for (int i = 0; i < total; i++) {
        if (atendido[i]) {
            cout << "[" << senhas[i] << "] " << nomes[i]
                 << " - " << servico[i]
                 << " (" << hora_entrada[i] << ")\n";
            vazio = false;
        }
    }

    if (vazio) cout << "(nenhum atendimento ainda)\n";
    pausar();
}

// -- 5. Ver log ------------------------------------------------
void ver_log() {
    cabecalho();
    cout << "--- LOG: atendimentos.txt ---\n";

    ifstream f("atendimentos.txt");
    if (!f) { cout << "Ficheiro nao encontrado.\n"; pausar(); return; }

    string linha;
    int n = 0;
    while (getline(f, linha)) { cout << linha << "\n"; n++; }
    if (n == 0) cout << "(vazio)\n";
    pausar();
}

// -- 6. Estatisticas -------------------------------------------
void estatisticas() {
    cabecalho();
    cout << "--- ESTATISTICAS ---\n";
    cout << "Total emitidas : " << total             << "\n";
    cout << "Atendidos      : " << total - em_espera()<< "\n";
    cout << "Em espera      : " << em_espera()        << "\n\n";

    for (int s = 0; s < N_SERV; s++) {
        int c = 0;
        for (int i = 0; i < total; i++)
            if (servico[i] == SERVICOS[s]) c++;
        cout << SERVICOS[s] << ": " << c << "\n";
    }
    pausar();
}

// -- Menu e main -----------------------------------------------
int main() {
    int op;

    do {
        cabecalho();
        cout << "[1] Retirar senha\n";
        cout << "[2] Chamar proximo\n";
        cout << "[3] Ver fila\n";
        cout << "[4] Historico\n";
        cout << "[5] Ver log\n";
        cout << "[6] Estatisticas\n";
        cout << "[0] Sair\n";
        cout << "\nOpcao: ";

        if (!(cin >> op)) {
            cin.clear();
            cin.ignore(1000, '\n');
            op = -1;
        } else {
            cin.ignore();
        }

        switch (op) {
            case 1: retirar_senha();  break;
            case 2: chamar_proximo(); break;
            case 3: ver_fila();       break;
            case 4: ver_historico();  break;
            case 5: ver_log();        break;
            case 6: estatisticas();   break;
            case 0: cout << "\nAte logo!\n"; break;
            default: cout << "\nOpcao invalida.\n"; pausar();
        }

    } while (op != 0);

    return 0;
}

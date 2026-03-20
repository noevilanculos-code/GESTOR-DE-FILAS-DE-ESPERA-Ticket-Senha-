// ============================================================
//   GESTOR DE FILAS DE ESPERA  (Ticket / Senha)  —  C++17
//   OOP: classes, vector, string, fstream, namespace
//   Cores ANSI — Windows 10+ / Linux / macOS
// ============================================================

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <ctime>
#include <limits>

#ifdef _WIN32
  #include <windows.h>
  static void ativar_ansi() {
      HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
      DWORD  modo;
      GetConsoleMode(h, &modo);
      SetConsoleMode(h, modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
      SetConsoleOutputCP(65001);
  }
#else
  static void ativar_ansi() {}
#endif

// ============================================================
//  namespace Cor  — todos os codigos ANSI num so lugar
// ============================================================
namespace Cor {
    // Estilos
    constexpr const char* RST     = "\033[0m";
    constexpr const char* B       = "\033[1m";     // negrito
    constexpr const char* DIM     = "\033[2m";

    // Primeiro plano
    constexpr const char* VERM    = "\033[31m";
    constexpr const char* VERDE   = "\033[32m";
    constexpr const char* AMAR    = "\033[33m";
    constexpr const char* AZUL    = "\033[34m";
    constexpr const char* MAG     = "\033[35m";
    constexpr const char* CIANO   = "\033[36m";
    constexpr const char* BRANCO  = "\033[37m";

    // Brilhantes
    constexpr const char* AMAR_B  = "\033[93m";
    constexpr const char* CIANO_B = "\033[96m";
    constexpr const char* MAG_B   = "\033[95m";
    constexpr const char* VERDE_B = "\033[92m";

    // Fundo
    constexpr const char* BG_AZUL = "\033[44m";
    constexpr const char* BG_MAG  = "\033[45m";

    // Aliases semanticos
    inline std::string titulo()   { return std::string(B)    + CIANO;   }
    inline std::string destaque() { return std::string(B)    + AMAR;    }
    inline std::string sucesso()  { return std::string(B)    + VERDE;   }
    inline std::string aviso()    { return std::string(B)    + VERM;    }
    inline std::string info()     { return std::string(B)    + AZUL;    }
    inline std::string label()    { return std::string(DIM)  + BRANCO;  }
    inline std::string valor()    { return std::string(B)    + BRANCO;  }
}

// ============================================================
//  class UI  — desenho do ecra
// ============================================================
class UI {
public:
    static constexpr int LARGURA = 54;

    static void linha_h(const std::string& cor, char c, int n = LARGURA) {
        std::cout << "  " << cor;
        for (int i = 0; i < n; ++i) std::cout << c;
        std::cout << Cor::RST << "\n";
    }

    static void cabecalho_seccao(const std::string& tag,
                                  const std::string& titulo,
                                  const std::string& cor) {
        std::cout << "\n";
        linha_h(cor, '=');
        std::cout << "  " << cor << "  " << tag << "  "
                  << Cor::B << Cor::BRANCO
                  << std::left << std::setw(LARGURA - 7) << titulo
                  << Cor::RST << "\n";
        linha_h(cor, '=');
    }

    static void prop(const std::string& label,
                     const std::string& val,
                     const std::string& cor_val) {
        std::cout << "  " << Cor::label()
                  << "  " << std::left << std::setw(12) << label
                  << Cor::RST << " " << cor_val << val << Cor::RST << "\n";
    }

    static void barra_prog(int v, int max_v, const std::string& cor) {
        int cheio = (max_v > 0) ? (v * 20 / max_v) : 0;
        std::cout << "  " << cor << "[";
        for (int i = 0; i < 20; ++i)
            std::cout << (i < cheio ? '#' : '.');
        std::cout << "] " << Cor::RST << Cor::B << v << Cor::RST << "\n";
    }

    static void pausar() {
        std::cout << "\n  " << Cor::label()
                  << "Prima " << Cor::RST << Cor::destaque()
                  << "ENTER" << Cor::RST << Cor::label()
                  << " para continuar..." << Cor::RST << " ";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    static void limpar_ecra() {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif
    }
};

// ============================================================
//  class Senha  — registo de um cliente na fila
// ============================================================
class Senha {
public:
    int         numero;
    std::string nome;
    std::string servico;
    std::string hora_entrada;
    bool        atendido;

    Senha() : numero(0), atendido(false) {}

    Senha(int num, const std::string& n,
          const std::string& srv, const std::string& hora)
        : numero(num), nome(n), servico(srv),
          hora_entrada(hora), atendido(false) {}

    // Numero formatado como "042"
    std::string num_str() const {
        std::ostringstream oss;
        oss << std::setw(3) << std::setfill('0') << numero;
        return oss.str();
    }
};

// ============================================================
//  class GestorFila  — logica FIFO + log em disco
// ============================================================
class GestorFila {
public:
    // Servicos e cores correspondentes
    static constexpr int NUM_SERVICOS = 4;

    inline static const std::array<std::string, NUM_SERVICOS> SERVICOS = {
        "Tesouraria", "Secretaria", "Recursos Humanos", "Atendimento Geral"
    };
    inline static const std::array<std::string, NUM_SERVICOS> COR_SERV = {
        std::string(Cor::B) + Cor::AMAR_B,
        std::string(Cor::B) + Cor::CIANO_B,
        std::string(Cor::B) + Cor::MAG_B,
        std::string(Cor::B) + Cor::VERDE_B
    };

private:
    std::vector<Senha> senhas;
    int  frente   = 0;
    int  contador = 1;
    std::string ficheiro_log;

    // Hora / data actuais
    static std::string hora_actual() {
        time_t t = time(nullptr);
        char buf[6];
        strftime(buf, sizeof(buf), "%H:%M", localtime(&t));
        return buf;
    }
    static std::string data_actual() {
        time_t t = time(nullptr);
        char buf[12];
        strftime(buf, sizeof(buf), "%d/%m/%Y", localtime(&t));
        return buf;
    }

    // Cor pelo nome do servico
    std::string cor_servico(const std::string& nome) const {
        for (int i = 0; i < NUM_SERVICOS; ++i)
            if (SERVICOS[i] == nome) return COR_SERV[i];
        return Cor::BRANCO;
    }

    // Grava uma linha no ficheiro de log
    void gravar_log(const Senha& s) const {
        std::ofstream fp(ficheiro_log, std::ios::app);
        if (!fp) { std::cerr << "Erro ao abrir log.\n"; return; }
        fp << "[" << data_actual() << "]"
           << "  Senha: " << s.num_str()
           << "  |  Entrada: " << s.hora_entrada
           << "  |  Saida: "   << hora_actual()
           << "  |  Cliente: " << std::left << std::setw(40) << s.nome
           << "  |  Servico: " << s.servico
           << "\n";
    }

    int em_espera_count() const {
        int n = 0;
        for (const auto& s : senhas)
            if (!s.atendido) ++n;
        return n;
    }

public:
    explicit GestorFila(const std::string& log = "atendimentos.txt")
        : ficheiro_log(log) {}

    int total()     const { return static_cast<int>(senhas.size()); }
    int atendidos() const { return total() - em_espera_count(); }
    int em_espera() const { return em_espera_count(); }

    // ── 1. Retirar senha ──────────────────────────────────
    void retirar_senha() {
        if (em_espera_count() >= 100) {
            std::cout << "\n  " << Cor::aviso()
                      << "[!] Fila cheia!" << Cor::RST
                      << " Nao e possivel emitir mais senhas.\n";
            UI::pausar(); return;
        }

        UI::linha_h(Cor::AZUL, '-');
        std::cout << "  " << Cor::titulo()
                  << "  [+] NOVA SENHA" << Cor::RST << "\n";
        UI::linha_h(Cor::AZUL, '-');

        // Nome
        std::cout << "\n  " << Cor::label()
                  << "Nome do cliente: " << Cor::RST
                  << Cor::valor();
        std::string nome;
        std::getline(std::cin, nome);
        std::cout << Cor::RST;
        if (nome.empty()) nome = "Cliente Anonimo";

        // Escolha do servico
        std::cout << "\n  " << Cor::label()
                  << "Servicos disponiveis:\n\n" << Cor::RST;
        for (int i = 0; i < NUM_SERVICOS; ++i)
            std::cout << "    " << Cor::CIANO << "["
                      << Cor::destaque() << (i + 1) << Cor::CIANO << "]"
                      << Cor::RST << "  " << COR_SERV[i]
                      << SERVICOS[i] << Cor::RST << "\n";

        std::cout << "\n  " << Cor::label()
                  << "Opcao: " << Cor::RST << Cor::valor();
        int op = 0;
        std::cin >> op;
        std::cout << Cor::RST;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::string srv;
        if (op < 1 || op > NUM_SERVICOS) {
            std::cout << "  " << Cor::aviso() << "[!]" << Cor::RST
                      << " Invalido. Definido como 'Atendimento Geral'.\n";
            srv = "Atendimento Geral";
        } else {
            srv = SERVICOS[op - 1];
        }

        Senha s(contador++, nome, srv, hora_actual());
        senhas.push_back(s);

        // Bilhete visual
        std::cout << "\n";
        UI::linha_h(Cor::VERDE, '*');
        std::cout << "  " << Cor::sucesso()
                  << "  SENHA EMITIDA COM SUCESSO!" << Cor::RST << "\n";
        UI::linha_h(Cor::VERDE, '*');
        std::cout << "\n"
                  << "  " << Cor::BG_AZUL << Cor::B << Cor::BRANCO
                  << "  +---------------------------------+  " << Cor::RST << "\n"
                  << "  " << Cor::BG_AZUL << Cor::B << Cor::BRANCO
                  << "  |  SENHA No: " << Cor::AMAR_B << Cor::B
                  << std::left << std::setw(4) << s.num_str()
                  << Cor::BRANCO
                  << "               |  " << Cor::RST << "\n"
                  << "  " << Cor::BG_AZUL << Cor::B << Cor::BRANCO
                  << "  +---------------------------------+  " << Cor::RST << "\n\n";

        UI::prop("Nome",    s.nome,         Cor::valor());
        UI::prop("Servico", s.servico,      cor_servico(s.servico));
        UI::prop("Hora",    s.hora_entrada, Cor::destaque());

        std::cout << "\n";
        UI::linha_h(Cor::VERDE, '*');
        UI::pausar();
    }

    // ── 2. Chamar proximo ─────────────────────────────────
    void chamar_proximo() {
        // Avanca frente sobre ja atendidos
        while (frente < total() && senhas[frente].atendido)
            ++frente;

        if (frente >= total()) {
            std::cout << "\n  " << Cor::info()
                      << "[i]" << Cor::RST
                      << " Nao ha clientes em espera.\n";
            UI::pausar(); return;
        }

        Senha& s = senhas[frente];
        s.atendido = true;

        std::cout << "\n";
        UI::linha_h(Cor::AMAR_B, '#');
        std::cout << "  " << Cor::B << Cor::AMAR_B
                  << "  >>> PROXIMO ATENDIMENTO <<<" << Cor::RST << "\n";
        UI::linha_h(Cor::AMAR_B, '#');
        std::cout << "\n"
                  << "  " << Cor::BG_MAG << Cor::B << Cor::BRANCO
                  << "  CHAMANDO SENHA:  "
                  << Cor::AMAR_B << Cor::B
                  << std::left << std::setw(4) << s.num_str()
                  << Cor::BRANCO
                  << "                 " << Cor::RST << "\n\n";

        UI::prop("Nome",    s.nome,         Cor::valor());
        UI::prop("Servico", s.servico,      cor_servico(s.servico));
        UI::prop("Entrada", s.hora_entrada, Cor::destaque());

        // Quantos ainda aguardam?
        int espera = 0;
        for (int i = frente + 1; i < total(); ++i)
            if (!senhas[i].atendido) ++espera;

        std::cout << "\n";
        UI::linha_h(Cor::AMAR_B, '-');
        if (espera == 0)
            std::cout << "  " << Cor::sucesso()
                      << "  Fila vazia - todos foram atendidos!" << Cor::RST << "\n";
        else
            std::cout << "  " << Cor::info()
                      << "  Clientes ainda em espera: " << Cor::RST
                      << Cor::aviso() << Cor::B << " " << espera << Cor::RST << "\n";
        UI::linha_h(Cor::AMAR_B, '-');

        gravar_log(s);
        ++frente;
        UI::pausar();
    }

    // ── 3. Ver fila actual ────────────────────────────────
    void mostrar_fila() const {
        UI::cabecalho_seccao(">>", "FILA DE ESPERA ACTUAL", Cor::CIANO);
        std::cout << "\n  "
                  << Cor::B << Cor::CIANO
                  << std::left
                  << "  #    SENHA  "
                  << std::setw(26) << "NOME" << "  "
                  << std::setw(18) << "SERVICO" << "  HORA\n"
                  << Cor::RST;
        UI::linha_h(Cor::CIANO, '-');

        int pos = 1;
        bool encontrou = false;
        for (int i = frente; i < total(); ++i) {
            if (!senhas[i].atendido) {
                std::string cor = (pos == 1)
                    ? std::string(Cor::B) + Cor::VERDE_B
                    : std::string(Cor::BRANCO);
                std::cout << "  " << cor
                          << "  " << std::setw(2) << pos
                          << "   [" << senhas[i].num_str() << "]  "
                          << std::left << std::setw(26) << senhas[i].nome << "  "
                          << std::setw(18) << senhas[i].servico << "  "
                          << senhas[i].hora_entrada;
                if (pos == 1)
                    std::cout << "  " << Cor::B << Cor::VERDE_B << "<< PROXIMO";
                std::cout << Cor::RST << "\n";
                encontrou = true;
                ++pos;
            }
        }

        if (!encontrou)
            std::cout << "  " << Cor::sucesso()
                      << "  (fila vazia)" << Cor::RST << "\n";

        UI::linha_h(Cor::CIANO, '=');
        UI::pausar();
    }

    // ── 4. Historico ──────────────────────────────────────
    void mostrar_historico() const {
        UI::cabecalho_seccao("##", "HISTORICO DA SESSAO", Cor::MAG);
        std::cout << "\n  "
                  << Cor::B << Cor::MAG
                  << "  SENHA  "
                  << std::left << std::setw(28) << "NOME" << "  "
                  << std::setw(18) << "SERVICO" << "  HORA\n"
                  << Cor::RST;
        UI::linha_h(Cor::MAG, '-');

        bool encontrou = false;
        for (const auto& s : senhas) {
            if (s.atendido) {
                std::cout << "  " << Cor::DIM
                          << "  [" << s.num_str() << "]  "
                          << std::left << std::setw(28) << s.nome << "  "
                          << std::setw(18) << s.servico << "  "
                          << s.hora_entrada << Cor::RST << "\n";
                encontrou = true;
            }
        }

        if (!encontrou)
            std::cout << "  " << Cor::DIM
                      << "  (nenhum atendimento ainda)" << Cor::RST << "\n";

        UI::linha_h(Cor::MAG, '=');
        UI::pausar();
    }

    // ── 5. Ver log em disco ───────────────────────────────
    void ver_log() const {
        std::ifstream fp(ficheiro_log);
        if (!fp) {
            std::cout << "\n  " << Cor::info() << "[i]" << Cor::RST
                      << " Ficheiro " << Cor::destaque()
                      << "'" << ficheiro_log << "'" << Cor::RST
                      << " nao encontrado.\n";
            UI::pausar(); return;
        }

        UI::cabecalho_seccao("~~", "LOG DIARIO: " + ficheiro_log, Cor::AMAR);
        std::cout << "\n";

        std::string linha;
        int linhas = 0;
        while (std::getline(fp, linha)) {
            std::cout << "  " << Cor::DIM << linha << Cor::RST << "\n";
            ++linhas;
        }

        if (linhas == 0)
            std::cout << "  " << Cor::DIM
                      << "(ficheiro vazio)" << Cor::RST << "\n";

        std::cout << "\n";
        UI::linha_h(Cor::AMAR, '=');
        UI::pausar();
    }

    // ── 6. Estatisticas ───────────────────────────────────
    void mostrar_estatisticas() const {
        // Contagens por servico
        std::array<int, NUM_SERVICOS> contagem{};
        for (const auto& s : senhas)
            for (int i = 0; i < NUM_SERVICOS; ++i)
                if (s.servico == SERVICOS[i]) ++contagem[i];

        UI::cabecalho_seccao("%%", "ESTATISTICAS DA SESSAO", Cor::VERDE);
        std::cout << "\n"
                  << "  " << Cor::label() << "  Total emitidas :  "
                  << Cor::RST << Cor::valor()  << total()     << Cor::RST << "\n"
                  << "  " << Cor::label() << "  Atendidos      :  "
                  << Cor::RST << Cor::sucesso() << atendidos() << Cor::RST << "\n"
                  << "  " << Cor::label() << "  Em espera      :  "
                  << Cor::RST << Cor::aviso()   << em_espera() << Cor::RST << "\n\n";

        UI::linha_h(Cor::VERDE, '-');
        std::cout << "  " << Cor::B << Cor::VERDE
                  << "  Distribuicao por Servico:\n\n" << Cor::RST;

        for (int i = 0; i < NUM_SERVICOS; ++i) {
            std::cout << "  " << COR_SERV[i]
                      << "  " << std::left << std::setw(18) << SERVICOS[i]
                      << Cor::RST << "  ";
            UI::barra_prog(contagem[i], total() > 0 ? total() : 1, COR_SERV[i]);
        }

        std::cout << "\n";
        UI::linha_h(Cor::VERDE, '=');
        UI::pausar();
    }

    // ── Menu de estado (para o cabecalho) ─────────────────
    void imprimir_estado(const std::string& data, const std::string& hora) const {
        UI::limpar_ecra();
        std::cout << "\n"
            << "  " << Cor::BG_AZUL << Cor::B << Cor::BRANCO
            << "                                                      " << Cor::RST << "\n"
            << "  " << Cor::BG_AZUL << Cor::B << Cor::BRANCO
            << "    SISTEMA DE GESTAO DE FILAS  v3.0 (C++)           " << Cor::RST << "\n"
            << "  " << Cor::BG_AZUL << Cor::DIM << Cor::BRANCO
            << "    Log: " << std::left << std::setw(16) << ficheiro_log
            << "        " << data << "   " << hora << "    " << Cor::RST << "\n"
            << "  " << Cor::BG_AZUL << Cor::B << Cor::BRANCO
            << "                                                      " << Cor::RST << "\n\n";

        UI::linha_h(Cor::CIANO, '-');
        std::cout << "  " << Cor::CIANO << "  ESTADO:" << Cor::RST
                  << "  Total "     << Cor::destaque() << std::setw(3) << total()     << Cor::RST
                  << "  Atendidos " << Cor::sucesso()  << std::setw(3) << atendidos() << Cor::RST
                  << "  Em espera " << Cor::aviso()    << std::setw(3) << em_espera() << Cor::RST
                  << "\n";
        UI::linha_h(Cor::CIANO, '-');
    }
};

// ============================================================
//  Utilitario: hora / data como string
// ============================================================
static std::string hora_str() {
    time_t t = time(nullptr);
    char buf[6];
    strftime(buf, sizeof(buf), "%H:%M", localtime(&t));
    return buf;
}
static std::string data_str() {
    time_t t = time(nullptr);
    char buf[12];
    strftime(buf, sizeof(buf), "%d/%m/%Y", localtime(&t));
    return buf;
}

// ============================================================
//  Funcao de menu (usa GestorFila por referencia)
// ============================================================
static void mostrar_menu(GestorFila& fila) {
    fila.imprimir_estado(data_str(), hora_str());

    std::cout << "\n"
        << "  " << Cor::CIANO << "  [" << Cor::destaque() << "1" << Cor::CIANO << "]  "
        << Cor::RST << "Retirar senha        " << Cor::label() << "(cliente)"      << Cor::RST << "\n"
        << "  " << Cor::CIANO << "  [" << Cor::destaque() << "2" << Cor::CIANO << "]  "
        << Cor::RST << "Chamar proximo       " << Cor::label() << "(funcionario)"  << Cor::RST << "\n"
        << "  " << Cor::CIANO << "  [" << Cor::destaque() << "3" << Cor::CIANO << "]  "
        << Cor::RST << "Ver fila actual\n"
        << "  " << Cor::CIANO << "  [" << Cor::destaque() << "4" << Cor::CIANO << "]  "
        << Cor::RST << "Historico da sessao\n"
        << "  " << Cor::CIANO << "  [" << Cor::destaque() << "5" << Cor::CIANO << "]  "
        << Cor::RST << "Ver log em disco\n"
        << "  " << Cor::CIANO << "  [" << Cor::destaque() << "6" << Cor::CIANO << "]  "
        << Cor::RST << "Estatisticas\n"
        << "  " << Cor::CIANO << "  [" << Cor::aviso()    << "0" << Cor::CIANO << "]  "
        << Cor::VERM << "Sair" << Cor::RST << "\n\n";

    UI::linha_h(Cor::CIANO, '=');
    std::cout << "\n  " << Cor::label() << "Opcao: " << Cor::RST
              << Cor::destaque();
}

// ============================================================
//  main
// ============================================================
int main() {
    ativar_ansi();

    GestorFila fila("atendimentos.txt");
    int opcao = -1;

    do {
        mostrar_menu(fila);

        if (!(std::cin >> opcao)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            opcao = -1;
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cout << Cor::RST;

        switch (opcao) {
            case 1: fila.retirar_senha();        break;
            case 2: fila.chamar_proximo();       break;
            case 3: fila.mostrar_fila();         break;
            case 4: fila.mostrar_historico();    break;
            case 5: fila.ver_log();              break;
            case 6: fila.mostrar_estatisticas(); break;
            case 0:
                UI::limpar_ecra();
                std::cout << "\n  " << Cor::sucesso()
                          << "  Ate logo! Boa continuacao."
                          << Cor::RST << "\n\n";
                break;
            default:
                std::cout << "\n  " << Cor::aviso()
                          << "  [!] Opcao invalida." << Cor::RST << "\n";
                UI::pausar();
        }
    } while (opcao != 0);

    return 0;
}

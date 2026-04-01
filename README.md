 GESTRO-FILA-ESPERA-TICKET-SENHA

Sistema de **filas de espera em modo terminal**, implementado em **C++17** com **OOP** (classes, std::vector, streams).  
Permite gerenciar a emissão de senhas, chamada de clientes e estatísticas diárias.



  INTEGRANTES:
  
1.ARLINDO LUCAS MUCHANGA

2.NOE VILANCULOS 

3.GUILHERME MACHICUA



 Descrição
Este programa simula o atendimento em filas de espera (ex.: bancos, clínicas ou repartições públicas), permitindo:

- Chamada do próximo cliente de forma automática  
- Registro de estatísticas diárias em disco  

Foi desenvolvido como trabalho de programação em **C++**, aplicando conceitos de **orientação a objetos**, **vetores dinâmicos** e manipulação de arquivos.


#Como executar

```bash
# Dev-C++ / MinGW
g++ -std=c++11 -o fila fila_simples.cpp

# Linux / macOS
g++ -std=c++11 -o fila fila_simples.cpp
./fila
```
Compilando o projeto
No terminal, estando na pasta raiz do projeto


## Funcionalidades

| # | Opção | Descrição |
|---|-------|-----------|
| 1 | Retirar senha | O cliente insere o nome e escolhe o serviço. É emitida uma senha numerada com hora de entrada. |
| 2 | Chamar próximo | Chama o primeiro da fila (FIFO), mostra os dados e grava no log. |
| 3 | Ver fila | Lista todos os clientes em espera. O primeiro aparece marcado como PROXIMO. |
| 4 | Histórico | Mostra todos os atendimentos já concluídos na sessão. |
| 5 | Ver log | Lê e exibe o conteúdo do ficheiro `atendimentos.txt`. |
| 6 | Estatísticas | Totais globais e contagem por serviço. |

## Serviços disponíveis

```
[1] Tesouraria
[2] Secretaria
[3] Recursos Humanos
[4] Atendimento Geral
```

## Interface

Cada operação limpa o ecrã e mostra o cabeçalho com os contadores:

```
================================
  GESTOR DE FILAS DE ESPERA
  25/03/2026  14:32
================================
  Total: 5  Atendidos: 3  Em espera: 2
================================

[1] Retirar senha
[2] Chamar proximo
[3] Ver fila
[4] Historico
[5] Ver log
[6] Estatisticas
[0] Sair

Opcao:
```

## Ficheiro de log

Cada atendimento é gravado em `atendimentos.txt` no mesmo directório:

```
[25/03/2026] Senha: 1 | Entrada: 14:10 | Saida: 14:18 | Cliente: Ana Silva | Servico: Tesouraria
[25/03/2026] Senha: 2 | Entrada: 14:11 | Saida: 14:25 | Cliente: Joao Costa | Servico: Secretaria
```

> No Dev-C++: Tools → Compiler Options → adicionar `-std=c++11`

## Estrutura de ficheiros

```
03-CODIGO/
└── fila_simples.cpp     codigo fonte

atendimentos.txt         gerado automaticamente
```

## Conceitos praticados

- Vetores paralelos (`senhas[]`, `nomes[]`, `servico[]`, `hora_entrada[]`, `atendido[]`)
- Lógica FIFO com índice `frente`
- Leitura e escrita em ficheiros (`ifstream`, `ofstream`)
- Funções (`void`, retorno de `string`, parâmetros)
- `for`, `while`, `switch`, `if/else`
- `string`, `bool`, `int`
- Data e hora com `ctime`

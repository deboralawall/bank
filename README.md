# Bank — Trabalho 2 (Métodos Formais)

Este é o repositório do **Trabalho 2** da disciplina *Métodos Formais (MFO)*, cujo objetivo é conectar um modelo formal escrito em Quint a uma implementação em C++ por meio de **testes baseados em modelo** (Model-Based Testing – MBT). O propósito é usar o modelo para aumentar a confiança no código corrigindo bugs identificados durante a execução dos testes. Trabalho realizado pelas alunas Débora Lawall Langner e Giovana Corrêa de Barros


##  Objetivo

Detectar e consertar bugs na implementação em C++ (`bank.hpp`) usando testes gerados automaticamente a partir do modelo Quint (`bank.qnt`). Para cada teste que falhar, entende-se o desvio entre modelo e implementação, corrige-se o código e documenta-se o ajuste no relatório.

---

###  Conteúdo do Repositório

| Arquivo                | Propósito                                           |
|-------------------------|-----------------------------------------------------|
| `bank.qnt`              | Modelo formal correto (não modificado)               |
| `bank.hpp`              | Implementação corrigida com base nos testes   |
| `test.cpp`              | Framework dos testes que consomem os traces JSON    |
| `traces/`               | Armazenamento dos arquivos gerados pelo modelo      |
| Relatório em PDF        | Documentação das falhas identificadas e correções  |

---

###  Requisitos e Ferramentas

- **Quint** (versão **0.22.4** ou superior)
  ```bash
  npm install -g @informalsystems/quint@latest
  ```
- **Biblioteca nlohmann/json**, presente em `lib/nlohmann`.

---

### Como executar

1. **Gerar 10 000 execuções (traces) do modelo:**
   ```bash
   mkdir -p traces
   quint run bank.qnt --mbt --out-itf=traces/out.itf.json --n-traces=10000
   ```

2. **Executar os testes com base nos traces gerados:**
   ```bash
   g++ -std=c++11 -I lib test.cpp -o test_bank
   ./test_bank traces/out.itf.json
   ```

3. **Iterar conforme resultados:**
   - Se o teste **falhar**, examine o output; identifique o bug comparando modelo vs. código; corrija `bank.hpp`; recompile e execute novamente com os mesmos traces.
   - Se o teste **passar**, repita o processo com novos traces para aumentar a confiança na correspondência entre modelo e implementação.

---

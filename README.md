# Meta-heurística: Algoritmos Genéticos com Busca Local para o Problema de Cobertura de Conjuntos

Este repositório contém a implementação de uma meta-heurística baseada em **Algoritmos Genéticos com Busca Local**, desenvolvida em C para resolver instâncias do **Problema de Cobertura de Conjuntos (Set Covering Problem - SCP)**. O projeto também conta com rotinas automáticas de execução, análise estatística e geração de gráficos desenvolvidas em Python.

---

# 📁 Estrutura do Projeto

Os arquivos estão organizados da seguinte forma:

```text
├── [Raiz do Projeto]
│   ├── main.c                      # Ponto de entrada e temporização do algoritmo
│   ├── genetico.c                  # Operadores do AG (Cruzamento, Mutação, Elitismo e Vias)
│   ├── buscalocal.c                # Estruturas de Vizinhança (2-flip e 3-flip)
│   ├── instancia.c                 # Parser das instâncias e matriz esparsa
│   ├── auxiliares.c                # Funções utilitárias
│   ├── Makefile                    # Script de compilação 
│   │
│   ├── visualizacao.py             # Executa a bateria padrão de testes
│   ├── teste_vias_genetico.py      # Gera gráfico comparando as vias do algoritmo
│   ├── grafico_convergencia.py     # Plota a evolução do custo por geração
│   └── boxplot.py                  # Gera boxplots do GAP (%)
│
├── headers/
│   ├── genetico.h
│   ├── buscalocal.h
│   ├── instancia.h
│   └── auxiliares.h
│
├── instancias/
│
└── resultados/
    ├── BL_muta020_elite010_...csv  # Resultados consolidados
    ├── boxplot_gap.png             # Boxplot do GAP por instância
    │
    ├── vias_de_teste/              # Resultados das vias de teste do algoritmo genético
    │   ├── via1_sem_nada.csv
    │   ├── via2_so_limpeza.csv
    │   ├── via3_completo.csv
    │   └── grafico_vias.png
    │
    └── convergencia/               # Resultados da convergência do algoritmo genético para alguns exemplos
        ├── convergencia_arquivoteste.csv
        └── convergencia_arquivoteste.png
```

---

# 🛠️ Como Compilar e Executar o Projeto

Antes de executar, certifique-se de possuir o **GCC** e o **Python 3** instalados.

## Dependências Python

```bash
pip install pandas matplotlib numpy seaborn
```

---

## 1. Seleção da Via do Algoritmo

O comportamento do algoritmo é controlado por uma diretiva de compilação localizada no início do arquivo `genetico.c`.

Escolha uma das seguintes configurações:

```c
#define VIA 1   // AG puro (sem limpeza e sem busca local)

#define VIA 2   // AG + eliminação de colunas redundantes

#define VIA 3   // AG + limpeza + busca local (padrão)
```

---

## 2. Compilação e Execução

Compile o projeto:

```bash
make
```

Em seguida execute a bateria de testes:

```bash
python visualizacao.py
```

---

## ⚠️ Alteração de Parâmetros

Sempre que modificar a variável `VIA` ou qualquer outro parâmetro do código em C, é necessário limpar os arquivos compilados antes de recompilar.

Execute:

```bash
make clean
make
python visualizacao.py
```
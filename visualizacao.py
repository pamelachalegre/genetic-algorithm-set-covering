import csv
import statistics
import subprocess

EXECUTAVEL = "./algoritmo_genetico.exe"

INSTANCIAS = {
    # "scpnrh5": {"arquivo": "instancias/scpnrh5_convertido.txt",
    #             "otimo": 55},
    "Teste01": {"arquivo": "instancias/Teste_01.dat",
                "otimo": 557.44},
    "Teste02": {"arquivo": "instancias/Teste_02.dat",
                "otimo": 537.89},
    "Teste03": {"arquivo": "instancias/Teste_03.dat",
                "otimo": 517.58},
    "Teste04": {"arquivo": "instancias/Teste_04.dat",
                "otimo": 1162.8},
    "Teste05": {"arquivo": "instancias/Teste_05.dat",
                "otimo": 1020.12},
    "Wren01": {"arquivo": "instancias/Wren_01.dat",
                "otimo": 7856},
    "Wren02": {"arquivo": "instancias/Wren_02.dat",
                "otimo": 13908},
    "Wren03": {"arquivo": "instancias/Wren_03.dat",
                "otimo": 13780},
    "Wren04": {"arquivo": "instancias/Wren_04.dat",
                "otimo": 58161},
}

POPULACOES = [50, 100]
SEED_INICIAL = 39
NUM_EXECUCOES = 10

#define TORNEIO 2
#define TAXA_MUTACAO 0.2
#define TAXA_ELITISMO 0.1
#define LAMBDA_MIN 0.15
#define LAMBDA_MAX 0.4
#define MAX_GERACOES 2000

CSV = "resultados/BL_muta020_elite010_lmin015_lmax040.csv"

# Para vias de teste 
# DEFINE VIA 1: Sem busca local, sem limpeza de redundância
# CSV = "resultados/via1_sem_nada.csv"

# DEFINE VIA 2: Sem busca local, com limpeza de redundância
# CSV = "resultados/via2_so_limpeza.csv"

# DEFINE VIA 3: Com busca local, com limpeza de redundância
#CSV = "resultados/via3_completo.csv"

def executar(instancia, populacao, seed):
    processo = subprocess.run([EXECUTAVEL, instancia["arquivo"], str(populacao), str(seed)], capture_output=True, text=True)

    if processo.stderr:
        print(processo.stderr)

    texto = processo.stdout.strip()
    campos = texto.split(",")

    if len(campos) != 8:
        raise RuntimeError(f" (!) Erro na saída: {texto}")

    seed = int(campos[0])
    melhor_inicial = float(campos[1])
    melhor_final = float(campos[2])
    geracoes = int(campos[3])
    geracoes_sem_melhora = int(campos[4])
    geracao_ultima_melhoria = int(campos[5])
    criterio = campos[6]
    tempo_execucao = float(campos[7])
    gap = 100 * (melhor_final - instancia["otimo"]) / instancia["otimo"]
    melhoria_absoluta = melhor_inicial - melhor_final
    melhoria_percentual = 100 * melhoria_absoluta / melhor_inicial

    return {"melhor_inicial": melhor_inicial, 
            "melhor_final": melhor_final,
            "gap_percentual": gap,
            "melhoria_absoluta": melhoria_absoluta,
            "melhoria_percentual": melhoria_percentual,
            "geracoes": geracoes,
            "geracoes_sem_melhora": geracoes_sem_melhora,
            "geracao_ultima_melhoria": geracao_ultima_melhoria,
            "criterio_parada": criterio,
            "tempo_execucao": tempo_execucao}

linhas = []

for nome, instancia in INSTANCIAS.items():
    print(f"\nInstância {nome}")

    for populacao in POPULACOES:
        resultados_populacao = []
        print(f"  População = {populacao}")

        for i in range(NUM_EXECUCOES):
            seed = SEED_INICIAL + i
            print(f"    Execução {i+1}/{NUM_EXECUCOES} (seed={seed})")

            r = executar(instancia, populacao, seed)

            resultados_populacao.append(r["melhor_final"])
            linhas.append({"instancia": nome,
                "populacao": populacao,
                "seed": seed,
                "melhor_inicial": round(r["melhor_inicial"], 2),
                "melhor_final": round(r["melhor_final"], 2),
                "melhoria_absoluta": round(r["melhoria_absoluta"], 2),
                "melhoria_percentual": round(r["melhoria_percentual"], 2),
                "gap_percentual": round(r["gap_percentual"], 2),
                "geracoes": r["geracoes"],
                "geracoes_sem_melhora": r["geracoes_sem_melhora"],
                "geracao_ultima_melhoria": r["geracao_ultima_melhoria"],
                "criterio_parada": r["criterio_parada"],
                "tempo_segundos": round(r["tempo_execucao"], 3)})
            
        print("\tmelhor:", min(resultados_populacao))
        print("\tmédia:", statistics.mean(resultados_populacao))
        print("\tmediana:", statistics.median(resultados_populacao))
        print("\tdesvio padrão:", statistics.stdev(resultados_populacao))

with open(CSV, "w", newline="") as arq:
    campos = ["instancia","populacao", "seed", "melhor_inicial", "melhor_final", "melhoria_absoluta", "melhoria_percentual", "gap_percentual", "geracoes", "geracoes_sem_melhora", "geracao_ultima_melhoria", "criterio_parada", "tempo_segundos"]

    writer = csv.DictWriter(arq, fieldnames=campos)
    writer.writeheader()
    writer.writerows(linhas)

print(f"\n (!) Arquivo CSV salvo: {CSV}")
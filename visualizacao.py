import csv
import statistics
import subprocess

EXECUTAVEL = "./algoritmo_genetico.exe"

INSTANCIAS = {
    "scpnrh5": {"arquivo": "instancias/scpnrh5_convertido.txt",
                "otimo": 55},
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

POPULACOES = [50, 100, 200]
SEED_INICIAL = 39
NUM_EXECUCOES = 10

CSV = "resultados/resultados.csv"

def executar(instancia, populacao, seed):
    processo = subprocess.run([EXECUTAVEL, instancia["arquivo"], str(populacao), str(seed)], capture_output=True, text=True)

    if processo.stderr:
        print(processo.stderr)

    texto = processo.stdout.strip()
    campos = texto.split(",")

    if len(campos) != 7:
        raise RuntimeError(f" (!) Erro na saída: {texto}")

    seed = int(campos[0])
    melhor_inicial = float(campos[1])
    melhor_final = float(campos[2])
    geracoes = int(campos[3])
    geracoes_sem_melhora = int(campos[4])
    criterio = campos[5]
    tempo_execucao = float(campos[6])
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
                "melhor_inicial": r["melhor_inicial"],
                "melhor_final": r["melhor_final"],
                "melhoria_absoluta": r["melhoria_absoluta"], 
                "melhoria_percentual": r["melhoria_percentual"],
                "gap_percentual": r["gap_percentual"],
                "geracoes": r["geracoes"],
                "geracoes_sem_melhora": r["geracoes_sem_melhora"],
                "criterio_parada": r["criterio_parada"],
                "tempo_segundos": r["tempo_execucao"]})
            
        print("\tmelhor:", min(resultados_populacao))
        print("\tmédia:", statistics.mean(resultados_populacao))
        print("\tmediana:", statistics.median(resultados_populacao))
        print("\tdesvio padrão:", statistics.stdev(resultados_populacao))

with open(CSV, "w", newline="") as arq:
    campos = ["instancia","populacao", "seed", "melhor_inicial", "melhor_final", "melhoria_absoluta", "melhoria_percentual", "gap_percentual", "geracoes", "geracoes_sem_melhora", "criterio_parada", "tempo_segundos"]

    writer = csv.DictWriter(arq, fieldnames=campos)
    writer.writeheader()
    writer.writerows(linhas)

print(f"\n (!) Arquivo CSV salvo: {CSV}")
import pandas as pd
import matplotlib.pyplot as plt

ARQUIVO_VIA1 = "resultados/vias_de_teste/via1_sem_nada.csv"
ARQUIVO_VIA2 = "resultados/vias_de_teste/via2_so_limpeza.csv"
ARQUIVO_VIA3 = "resultados/vias_de_teste/via3_completo.csv"

m1 = pd.read_csv(ARQUIVO_VIA1).groupby("instancia")["melhor_final"].mean() 
m2 = pd.read_csv(ARQUIVO_VIA2).groupby("instancia")["melhor_final"].mean()
m3 = pd.read_csv(ARQUIVO_VIA3).groupby("instancia")["melhor_final"].mean()

df_vias = pd.DataFrame({
    "Via 1 (Sem Nada)": m1,
    "Via 2 (Só Limpeza)": m2,
    "Via 3 (Completo)": m3
})

ax = df_vias.plot(kind="bar", figsize=(12, 6), color=["#e74c3c", "#f1c40f", "#2ecc71"], width=0.8)

plt.title("Comparação do Custo Médio Final por Instância e Via", fontsize=14)
plt.ylabel("Custo Médio Final", fontsize=12)
plt.xlabel("Instância", fontsize=12)
plt.xticks(rotation=45, ha="right")
plt.grid(axis="y", linestyle="--", alpha=0.5)
plt.tight_layout()

plt.savefig("resultados/vias_de_teste/grafico_vias.png", dpi=300)
print("Gráfico 'grafico_vias.png' gerado com Pandas!")

print("\n" + "="*50)
print("="*50)
print("\\begin{table}[h!]")
print("\\centering")
print("\\caption{Comparação do custo médio final obtido pelas três vias de execução.}")
print("\\label{tab:comparacao_vias}")
print("\\begin{tabular}{|l|c|c|c|}")
print("\\hline")
print("Instância & Via 1 (Sem Nada) & Via 2 (Só Limpeza) & Via 3 (Completo) \\\\ \\hline")
for inst, linha in df_vias.iterrows(): 
    print(f"{inst} & {linha['Via 1 (Sem Nada)']:.2f} & {linha['Via 2 (Só Limpeza)']:.2f} & {linha['Via 3 (Completo)']:.2f} \\\\ \\hline")
print("\\end{tabular}")
print("\\end{table}")
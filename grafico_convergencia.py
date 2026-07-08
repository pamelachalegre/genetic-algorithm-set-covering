import pandas as pd
import matplotlib.pyplot as plt

ARQUIVO_CSV = "resultados/convergencia_wren01.csv" 

df = pd.read_csv(ARQUIVO_CSV)

plt.figure(figsize=(8, 5))

plt.plot(df["Geracao"], df["Custo"], linewidth=2)

plt.xlabel("Geração")
plt.ylabel("Melhor custo")
plt.title("Convergência do Algoritmo Genético (Instância Wren01)")

plt.grid(True, linestyle="--", alpha=0.5)

plt.tight_layout()

plt.savefig("resultados/convergencia_wren01.png", dpi=300)
plt.show()
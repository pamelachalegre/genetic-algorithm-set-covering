import pandas as pd
import matplotlib.pyplot as plt

dados = pd.read_csv("resultados/convergencia_wren03.csv")

plt.figure(figsize=(8, 5))

plt.plot(dados["Geracao"], dados["Custo"], linewidth=2)

plt.xlabel("Geração")
plt.ylabel("Melhor custo")
plt.title("Convergência do Algoritmo Genético (Instância Wren03)")

plt.grid(True, linestyle="--", alpha=0.5)

plt.tight_layout()

plt.savefig("resultados/convergencia_wren03.png", dpi=300)
plt.show()
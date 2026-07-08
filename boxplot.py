import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

ARQUIVO_CSV = "resultados/BL_muta020_elite010_lmin015_lmax040.csv" 

df = pd.read_csv(ARQUIVO_CSV)

plt.figure(figsize=(12, 6))
sns.set_theme(style="whitegrid")

# Eixo X = Nome das instâncias, Eixo Y = GAP percentual
ax = sns.boxplot(x="instancia", y="gap_percentual", data=df, palette="Set2")

plt.title("Boxplot do GAP (%) por Instância (10 execuções independentes)", fontsize=14)
plt.xlabel("Instâncias", fontsize=12)
plt.ylabel("GAP (%)", fontsize=12)
plt.xticks(rotation=45) 
plt.tight_layout()
plt.savefig("resultados/boxplot_gap.png", dpi=300)

print("Gráfico salvo como resultados/boxplot_gap.png")
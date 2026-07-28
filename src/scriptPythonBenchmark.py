import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Carrega os CSVs gerados pelo C++
try:
    df_avl = pd.read_csv('benchmark_avl.csv')
    df_rn = pd.read_csv('benchmark_rn.csv')
except FileNotFoundError:
    print("Erro: Os arquivos CSV não foram encontrados. Rode a opção 11 no C++ primeiro.")
    exit()

cenarios = df_avl['Cenario'].tolist()
tempo_avl = df_avl['Tempo_ns'].tolist()
tempo_rn = df_rn['Tempo_ns'].tolist()
rotacoes_avl = df_avl['Rotacoes'].tolist()
rotacoes_rn = df_rn['Rotacoes'].tolist()

# Configura o layout
x = np.arange(len(cenarios))
largura = 0.35
plt.style.use('seaborn-v0_8-whitegrid')
cor_avl = '#1b9e77'
cor_rn = '#d95f02'

# Gráfico de tempo de execução
fig1, ax1 = plt.subplots(figsize=(10, 6))
barras1_avl = ax1.bar(x - largura/2, tempo_avl, largura, label='AVL', color=cor_avl)
barras1_rn = ax1.bar(x + largura/2, tempo_rn, largura, label='Rubro-Negra', color=cor_rn)

ax1.set_ylabel('Tempo de Execução (ns / operação)', fontsize=12)
ax1.set_title('Desempenho: Tempo de Execução por Cenário', fontsize=14, fontweight='bold')
ax1.set_xticks(x)
ax1.set_xticklabels(cenarios, fontsize=11)
ax1.legend(fontsize=12)

def autolabel_tempo(barras, ax):
    for barra in barras:
        altura = barra.get_height()
        ax.annotate(f'{int(altura):,}'.replace(',','.') + ' ns',
                    xy=(barra.get_x() + barra.get_width() / 2, altura),
                    xytext=(0, 3), 
                    textcoords="offset points",
                    ha='center', va='bottom', fontsize=10)

autolabel_tempo(barras1_avl, ax1)
autolabel_tempo(barras1_rn, ax1)

fig1.tight_layout()
plt.savefig('grafico_tempo_execucao.png', dpi=300)
print("-> grafico_tempo_execucao.png gerado com sucesso!")

# Grafico de quantidade de rotações
fig2, ax2 = plt.subplots(figsize=(10, 6))
barras2_avl = ax2.bar(x - largura/2, rotacoes_avl, largura, label='AVL', color=cor_avl)
barras2_rn = ax2.bar(x + largura/2, rotacoes_rn, largura, label='Rubro-Negra', color=cor_rn)

ax2.set_ylabel('Número Total de Rotações', fontsize=12)
ax2.set_title('Rebalanceamento: Quantidade de Rotações por Cenário', fontsize=14, fontweight='bold')
ax2.set_xticks(x)
ax2.set_xticklabels(cenarios, fontsize=11)
ax2.legend(fontsize=12)

def autolabel_rotacoes(barras, ax):
    for barra in barras:
        altura = barra.get_height()
        ax.annotate(f'{int(altura):,}'.replace(',','.'), 
                    xy=(barra.get_x() + barra.get_width() / 2, altura),
                    xytext=(0, 3), 
                    textcoords="offset points",
                    ha='center', va='bottom', fontsize=10)

autolabel_rotacoes(barras2_avl, ax2)
autolabel_rotacoes(barras2_rn, ax2)

fig2.tight_layout()
plt.savefig('grafico_rotacoes.png', dpi=300)
print("-> grafico_rotacoes.png gerado com sucesso!")
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# 1. Leitura dos dados exportados pelo C++
try:
    df_avl = pd.read_csv('benchmark_avl.csv')
    df_rn = pd.read_csv('benchmark_rn.csv')
except FileNotFoundError:
    print("Erro: Os arquivos CSV não foram encontrados. Certifique-se de rodar a opção 11 no C++.")
    exit()

cenarios = df_avl['Cenario'].tolist()
tempo_avl = np.array(df_avl['Tempo_ns'].tolist())
tempo_rn = np.array(df_rn['Tempo_ns'].tolist())
rot_avl = np.array(df_avl['Rotacoes'].tolist())
rot_rn = np.array(df_rn['Rotacoes'].tolist())

# 2. Cálculo do Ganho Relativo (%)
# Positivo = AVL mais rápida; Negativo = RN mais rápida
ganho_avl = ((tempo_rn - tempo_avl) / tempo_rn) * 100

# 3. Configurações de layout do Dashboard
plt.style.use('seaborn-v0_8-whitegrid')
fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(18, 6))

cor_avl = '#1b9e77'
cor_rn = '#d95f02'
x = np.arange(len(cenarios))
width = 0.35
labels_eixo_x = ['Cen A', 'Cen B', 'Cen C', 'Cen D']

# ==========================================
# PAINEL 1: Tempos Absolutos
# ==========================================
ax1.bar(x - width/2, tempo_avl, width, label='AVL', color=cor_avl)
ax1.bar(x + width/2, tempo_rn, width, label='Rubro-Negra', color=cor_rn)
ax1.set_title('Tempo de Execução (ns/op)', fontsize=14, fontweight='bold')
ax1.set_xticks(x)
ax1.set_xticklabels(labels_eixo_x, fontsize=11)
ax1.set_ylabel('Nanossegundos', fontsize=12)
ax1.legend()

# ==========================================
# PAINEL 2: Rotações
# ==========================================
ax2.bar(x - width/2, rot_avl, width, label='AVL', color=cor_avl)
ax2.bar(x + width/2, rot_rn, width, label='Rubro-Negra', color=cor_rn)
ax2.set_title('Custo Estrutural (Rotações)', fontsize=14, fontweight='bold')
ax2.set_xticks(x)
ax2.set_xticklabels(labels_eixo_x, fontsize=11)
ax2.set_ylabel('Quantidade Total', fontsize=12)
ax2.legend()

# ==========================================
# PAINEL 3: Vantagem Relativa (%)
# ==========================================
cores_ganho = [cor_avl if val > 0 else cor_rn for val in ganho_avl]
barras_ganho = ax3.bar(labels_eixo_x, ganho_avl, color=cores_ganho)
ax3.set_title('Vantagem Relativa de Velocidade (%)', fontsize=14, fontweight='bold')
ax3.set_ylabel('Ganho de Desempenho (%)', fontsize=12)
ax3.axhline(0, color='black', linewidth=1.2)

# Adicionando os rótulos de porcentagem nas barras
for barra in barras_ganho:
    altura = barra.get_height()
    if altura > 0:
        ax3.annotate(f'+{altura:.1f}% (AVL)', xy=(barra.get_x() + barra.get_width()/2, altura),
                     xytext=(0, 3), textcoords="offset points", ha='center', va='bottom', 
                     fontsize=10, color=cor_avl, fontweight='bold')
    else:
        ax3.annotate(f'{altura:.1f}% (RN)', xy=(barra.get_x() + barra.get_width()/2, altura),
                     xytext=(0, -12), textcoords="offset points", ha='center', va='top', 
                     fontsize=10, color=cor_rn, fontweight='bold')

plt.tight_layout()
plt.savefig('dashboard_analitico.png', dpi=300)
print("-> dashboard_analitico.png gerado com sucesso!")
import matplotlib.pyplot as plt
import numpy as np

# ===== 修改这里 =====
sizes = [1000, 10000, 100000, 1000000]
speedups = [0.18, 1.03, 3.01, 2.24]
theoretical_peak = 100.0  # 根据你的GPU修改
# ===================

efficiency = [s / theoretical_peak * 100 for s in speedups]

plt.figure(figsize=(10, 6))
plt.plot(sizes, efficiency, 's-', linewidth=2, markersize=8, color='#D64933')
plt.ylim(0, 110)
plt.xscale('log')
plt.xlabel('Matrix Size (N)', fontsize=12)
plt.ylabel('Efficiency (%)', fontsize=12)
plt.title(f'GPU Efficiency (Theoretical Peak = {theoretical_peak:.0f}x)', fontsize=14)
plt.grid(True, alpha=0.3)
plt.axhline(y=100, color='green', linestyle='--', linewidth=1, label='100% Peak')
plt.axhline(y=50, color='orange', linestyle=':', linewidth=1, alpha=0.7)
plt.legend()

max_eff = max(efficiency)
max_idx = np.argmax(efficiency)
plt.annotate(f'{max_eff:.1f}%', 
             xy=(sizes[max_idx], max_eff),
             xytext=(5, 5), textcoords='offset points',
             fontsize=10, fontweight='bold')

plt.tight_layout()
plt.savefig('efficiency.png', dpi=300, bbox_inches='tight')
plt.show()
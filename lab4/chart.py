import matplotlib.pyplot as plt
import numpy as np

# ===== 数据 =====
sizes = [1000, 10000, 100000, 1000000]
speedups = [0.18, 1.03, 3.01, 2.24]
# =========================

plt.figure(figsize=(10, 6))
plt.plot(sizes, speedups, 'o-', linewidth=2, markersize=8, color='#D64933')
plt.ylim(0, max(speedups) * 1.25)  # 留更多空间给标注
plt.xscale('log')
plt.xlabel('Matrix Size (N)', fontsize=12)
plt.ylabel('Speedup', fontsize=12)
plt.title('GPU Speedup over CPU', fontsize=14)
plt.grid(True, alpha=0.3)
plt.axhline(y=1, color='gray', linestyle='--', linewidth=1)

# 标注所有数据点（统一在上方）
for size, sp in zip(sizes, speedups):
    plt.annotate(f'{sp:.2f}x', 
                 xy=(size, sp),
                 xytext=(0, 10), textcoords='offset points',
                 fontsize=9, ha='center',
                 bbox=dict(boxstyle='round,pad=0.2', facecolor='white', alpha=0.8))

plt.tight_layout()
plt.savefig('lab4/speedup.png', dpi=300, bbox_inches='tight')
plt.show()
print(f"图片已保存: speedup.png")
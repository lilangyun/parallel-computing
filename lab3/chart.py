#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Speedup and Efficiency Curve Plot
"""

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

# ============================================
# Input Data
# ============================================
threads = [1, 2, 4, 8, 16]
serial_time = 76.665
avg_times = [76.665, 45.370, 29.336, 22.792, 25.110]

# ============================================
# Calculate Speedup and Efficiency
# ============================================
speedup = [serial_time / t for t in avg_times]
efficiency = [s / threads[i] for i, s in enumerate(speedup)]
ideal_speedup = threads

print("=" * 60)
print("Performance Results")
print("=" * 60)
print(f"{'Processes':<10} {'Time(s)':<10} {'Speedup':<10} {'Efficiency':<10}")
print("-" * 60)
for i in range(len(threads)):
    print(f"{threads[i]:<10} {avg_times[i]:<10.3f} {speedup[i]:<10.2f} {efficiency[i]:<10.2f}")
print("=" * 60)

# ============================================
# Figure 1: Speedup Curve
# ============================================
plt.figure(figsize=(8, 6))

# Actual speedup curve
plt.plot(threads, speedup, 'b-o', linewidth=2, markersize=8, label='Actual Speedup')

# Ideal speedup curve
plt.plot(threads, ideal_speedup, 'r--', linewidth=2, markersize=8, label='Ideal Speedup')

# Label actual speedup values (above the points)
for i, (x, y) in enumerate(zip(threads, speedup)):
    plt.annotate(f'{y:.2f}', (x, y), textcoords="offset points", 
                 xytext=(0, 12), ha='center', fontsize=10, color='blue')

# Label ideal speedup values (below the points)
for i, (x, y) in enumerate(zip(threads, ideal_speedup)):
    plt.annotate(f'{y:.0f}', (x, y), textcoords="offset points", 
                 xytext=(0, -15), ha='center', fontsize=10, color='red')

plt.xlabel('Number of Processes', fontsize=12)
plt.ylabel('Speedup', fontsize=12)
plt.title('Speedup Curve', fontsize=14)
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend(fontsize=11)
plt.xticks(threads)
plt.tight_layout()
plt.savefig('lab3/speedup.png', dpi=150, bbox_inches='tight')
print("\nFigure 1 saved as: speedup.png")

# ============================================
# Figure 2: Efficiency Curve
# ============================================
plt.figure(figsize=(8, 6))

# Actual efficiency curve
plt.plot(threads, efficiency, 'g-s', linewidth=2, markersize=8, label='Actual Efficiency')

# Ideal efficiency curve (always 1)
ideal_efficiency = [1.0] * len(threads)
plt.plot(threads, ideal_efficiency, 'r--', linewidth=2, marker='^', markersize=8, label='Ideal Efficiency')

# Label actual efficiency values (above the points)
for i, (x, y) in enumerate(zip(threads, efficiency)):
    plt.annotate(f'{y:.2f}', (x, y), textcoords="offset points", 
                 xytext=(0, 10), ha='center', fontsize=10, color='green')

# Label ideal efficiency values (below the points)
for i, (x, y) in enumerate(zip(threads, ideal_efficiency)):
    plt.annotate(f'{y:.2f}', (x, y), textcoords="offset points", 
                 xytext=(0, -15), ha='center', fontsize=10, color='red')

plt.xlabel('Number of Processes', fontsize=12)
plt.ylabel('Efficiency', fontsize=12)
plt.title('Efficiency Curve', fontsize=14)
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend(fontsize=11)
plt.xticks(threads)
plt.ylim(0, 1.2)
plt.tight_layout()
plt.savefig('lab3/efficiency.png', dpi=150, bbox_inches='tight')
print("Figure 2 saved as: efficiency.png")

plt.close()
print("\nDone! Both figures saved.")
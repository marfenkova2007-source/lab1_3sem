import csv
import sys
from collections import defaultdict

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

source = sys.argv[1] if len(sys.argv) > 1 else "bench_results.csv"
target = sys.argv[2] if len(sys.argv) > 2 else "bench_results.png"

series = defaultdict(list)
with open(source, newline="") as file:
    for row in csv.DictReader(file):
        series[row["operation"]].append((int(row["n"]), float(row["seconds"])))

plt.figure(figsize=(9, 6))
for operation, points in series.items():
    points.sort()
    plt.plot([p[0] for p in points], [p[1] for p in points], marker="o", label=operation)

plt.xscale("log")
plt.yscale("log")
plt.xlabel("Количество элементов n")
plt.ylabel("Время, с")
plt.title("Бинарное дерево поиска: время операций от числа элементов")
plt.grid(True, which="both", alpha=0.3)
plt.legend()
plt.tight_layout()
plt.savefig(target, dpi=150)
print("Сохранено:", target)

import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import norm

# Исходные данные
a = -16  # левая граница
b = -1  # правая граница
n = [3, 18, 56, 86, 69, 42, 18, 3, 1]  # частоты
m = 9  # количество интервалов
N = sum(n)  # общее количество наблюдений

# 1. Расчет шага и границ интервалов
h = (b - a) / m
intervals = [(a + i * h, a + (i + 1) * h) for i in range(m)]

# 2. Относительные частоты
w = [ni / N for ni in n]

# 3. Оценка параметров распределения
midpoints = [(interval[0] + interval[1]) / 2 for interval in intervals]
mean = sum(ni * xi for ni, xi in zip(n, midpoints)) / N
variance = sum(ni * (xi - mean) ** 2 for ni, xi in zip(n, midpoints)) / N
std_dev = np.sqrt(variance)
print(mean)
print(std_dev)

# 4. Построение объединенного графика
plt.figure(figsize=(12, 6))

# Гистограмма с нормализацией (плотность)
plt.hist([interval[0] + h / 2 for interval in intervals],
         bins=[interval[0] for interval in intervals] + [b],
         weights=w, density=True, edgecolor='black', alpha=0.7, label='Гистограмма')

# Плотность нормального распределения
x = np.linspace(a, b, 1000)
pdf = norm.pdf(x, loc=mean, scale=std_dev)

plt.plot(x, pdf, 'r-', lw=2, label=f'N({mean:.2f}, {std_dev ** 2:.2f})')

# Форматирование графика
plt.title('Гистограмма и плотность нормального распределения')
plt.xlabel('Значения')
plt.ylabel('Относительная частота / Плотность')
plt.legend()
plt.grid(True, linestyle='--', alpha=0.7)
plt.tight_layout()
plt.show()

# 5. Таблица группированной выборки
print("\nТаблица группированной выборки:")
print(f"{'Интервал':<15} {'n_k':<5} {'w_k':<10}")
for i in range(m):
    print(f"{intervals[i][0]:.4f}-{intervals[i][1]:<.4f} {n[i]:<5} {w[i]:<10.5f}")

# 6. Таблица для критерия хи-квадрат
phi_values = []
p_star = []
table_data = []
chi_sq = 0
max_diff = 0

for i in range(m):
    if i == 0:
        lower = -np.inf
        upper = (intervals[i][1] - mean) / std_dev
    elif i == m - 1:
        lower = (intervals[i][0] - mean) / std_dev
        upper = np.inf
    else:
        lower = (intervals[i][0] - mean) / std_dev
        upper = (intervals[i][1] - mean) / std_dev

    p = norm.cdf(upper) - norm.cdf(lower)
    p_star.append(p)

    diff = abs(w[i] - p)
    chi_term = (N * diff ** 2) / p
    table_data.append([i + 1, w[i], p, diff, chi_term])
    chi_sq += chi_term
    if diff > max_diff:
        max_diff = diff

a = 0
print("\nТаблица для критерия хи-квадрат:")
print(f"{'k':<3} {'w_k':<10} {'p_k*':<10} {'|w_k - p_k*|':<12} {'N(w_k - p_k*)²/p_k*':<15}")
for row in table_data:
    a += row[4]
    print(f"{row[0]:<3} {row[1]:<10.5f} {row[2]:<10.5f} {row[3]:<12.5f} {row[4]:<15.5f}")
print(a)
print(f"\nМаксимальная разница |w_k - p_k*| = {max_diff:.5f}")
print(f"Значение критерия хи-квадрат: {chi_sq:.5f}")

# 7. Проверка гипотезы
alpha = 0.05
df = m - 1 - 1  # степени свободы: m интервалов - 1 (среднее) - 1 (дисперсия)
critical_value = norm.ppf(1 - alpha, df)
print(f"\nКритическое значение при α={alpha} и df={df}: {critical_value:.5f}")

if chi_sq < critical_value:
    print("Гипотеза о нормальном распределении принимается.")
else:
    print("Гипотеза отклоняется.")

# 8. Вывод оценок параметров
print(f"\nОценка математического ожидания ā = {mean:.5f}")
print(f"Оценка дисперсии σ² =  {variance:.5f}")

# 6. Таблица с Φ((a_k - ā)/σ̃) и p_k*
table_data_phi = []
for k in range(1, m + 1):
    a_k = intervals[k - 1][1]  # Правая граница k-го интервала
    z_k = (a_k - mean) / std_dev
    phi_value = norm.cdf(z_k)

    if k == 1:
        p_star = phi_value
    elif k == m:
        z_prev = (intervals[m - 2][1] - mean) / std_dev
        p_star = 1 - norm.cdf(z_prev)
    else:
        z_prev = (intervals[k - 2][1] - mean) / std_dev
        p_star = phi_value - norm.cdf(z_prev)

    table_data_phi.append([k, a_k, phi_value, p_star])

print("\nТаблица с Φ((a_k - ā)/σ̃) и p_k*:")
print(f"{'k':<3} {'a_k':<12} {'Φ((a_k - ā)/σ̃)':<15} {'p_k*':<10}")
for row in table_data_phi:
    print(f"{row[0]:<3} {row[1]:<12.4f} {row[2]:<15.5f} {row[3]:<10.5f}")
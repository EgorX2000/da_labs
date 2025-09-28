import pandas as pd
import matplotlib.pyplot as plt

# Чтение данных из файла .csv
data = pd.read_csv('build/benchmark_results.csv')

# Проверка, что файл содержит нужные столбцы
if 'AhoCorasickTime' not in data.columns or 'BruteForceTime' not in data.columns:
    print("Ошибка: Файл benchmark_results.csv должен содержать столбцы 'AhoCorasickTime' и 'BruteForceTime'.")
    exit(1)

# Создание графика
plt.figure(figsize=(10, 6))
plt.plot(data['AhoCorasickTime'], label='Aho-Corasick',
         marker='o', color='#1f77b4')
plt.plot(data['BruteForceTime'], label='Brute Force',
         marker='s', color='#ff7f0e')
plt.xlabel('Test Number (Increasing Data Size)')
plt.ylabel('Time (milliseconds)')
plt.title('Performance Comparison: Aho-Corasick vs Brute Force')
plt.legend()
plt.grid(True)

# Сохранение графика
plt.savefig('performance10.png')
plt.close()

print("График сохранен в performance.png")

# graphics_separate_windows.py
import pandas as pd
import matplotlib.pyplot as plt

# 1. Загрузка и сортировка данных
data = pd.read_csv("build/benchmark3.csv")
data.columns = ['N', 'bt_insert', 'bt_find', 'bt_remove',
                'st_insert', 'st_find', 'st_remove']
data = data.sort_values('N')

# 2. Настройка стиля
plt.style.use("seaborn-v0_8-whitegrid")

# 3. Параметры для каждой операции
operations = [
    ('Insert', 'bt_insert', 'st_insert'),
    ('Find', 'bt_find', 'st_find'),
    ('Remove', 'bt_remove', 'st_remove')
]

# 4. Создание отдельных окон для каждой операции
for op_name, bt_col, st_col in operations:
    # Создаем новое окно
    plt.figure(figsize=(10, 6))

    # Строим линии без маркеров
    plt.plot(data['N'], data[bt_col],
             color='#2ca02c', linewidth=2,
             label='BTree')

    plt.plot(data['N'], data[st_col],
             color='#d62728', linewidth=2, label='std::map')

    # Настройка оформления
    plt.title(f"Operation: {op_name}", fontsize=14, pad=15)
    plt.xlabel("Amount of data, n", fontsize=12)
    plt.ylabel("Time, ms", fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.legend(fontsize=12)

    # Форматирование осей
    plt.ticklabel_format(axis='x', style='plain')
    plt.ticklabel_format(axis='y', style='sci', scilimits=(0, 0))

# 5. Показать все окна
plt.show()

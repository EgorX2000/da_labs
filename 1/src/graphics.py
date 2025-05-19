import pandas as pd
import matplotlib.pyplot as plt

# 1. Загрузка данных
data = pd.read_csv("build\sort.csv")  # Формат: N,radix_time,std_time

# 2. Настройка стиля
plt.style.use("seaborn-v0_8-whitegrid")  # Чистый стиль с сеткой
plt.figure(figsize=(10, 6))

# 3. Построение линий (без точек)
plt.plot(data["N"], data["radix_time"],
         color="#1f77b4", linewidth=2, label="Radix Sort")
plt.plot(data["N"], data["std_time"],
         color="#ff7f0e", linewidth=2, label="std::sort")

# 4. Настройка осей и заголовка
plt.title("Сравнение времени сортировки", pad=20, fontsize=14)
plt.xlabel("Размер массива (N)", fontsize=12)
plt.ylabel("Время (секунды)", fontsize=12)

# 5. Логарифмические оси (если данные сильно различаются)
plt.xscale("log")
plt.yscale("log")

# 6. Легенда и сетка
plt.legend(fontsize=12, framealpha=1)
plt.grid(True, which="both", linestyle="-", alpha=0.4)

plt.show()

import numpy as np
import matplotlib.pyplot as plt
from math import *

# Открываем файл и читаем данные
data = np.loadtxt('results.txt')

# Извлекаем столбцы
x = data[:, 0]  # Первый столбец: количество итераций
y1 = data[:, 1]  # Второй столбец: количество операций для потока 0
y2 = data[:, 2]  # Третий столбец: количество операций для потока 1
conflicts = data[:, 3]  # Четвертый столбец: количество конфликтов

# Вычисляем общее количество операций
total_operations = y1 + y2

# Вычисляем относительные значения
relative_y1 = (y1 / total_operations) * 100  # Процент операций для потока 0
relative_y2 = (y2 / total_operations) * 100  # Процент операций для потока 1

# Строим график для абсолютных значений
plt.figure(figsize=(21, 6))

# Абсолютные значения
plt.subplot(2, 2, 1)
plt.plot(x, y1, marker='o', label='Поток 0', color='b')
plt.plot(x, y2, marker='x', label='Поток 1', color='r')
plt.title('Количество операций для потоков 0 и 1 в зависимости от числа итераций')
plt.xlabel('Количество итераций')
plt.ylabel('Количество операций')
plt.legend()
plt.grid(True)

# Относительные значения
plt.subplot(2, 2, 2)
plt.plot(x, relative_y1, marker='o', label='Поток 0', color='b')
plt.plot(x, relative_y2, marker='x', label='Поток 1', color='r')
plt.title('Относительное количество операций для потоков 0 и 1 в зависимости от числа итераций')
plt.xlabel('Количество итераций')
plt.ylabel('Относительное количество операций (%)')
plt.legend()
plt.grid(True)

x_log = []
for _ in x:
    x_log.append(log(_))

# График для конфликтов
plt.subplot(2, 2, 3)
plt.plot(x_log, conflicts, marker='s', color='g', label='Количество конфликтов')
plt.title('Количество конфликтов в зависимости от числа итераций')
plt.xlabel('Количество итераций')
plt.ylabel('Количество конфликтов')
plt.legend()
plt.grid(True)

# График для относительного значения конфликтов от числа итераций
plt.subplot(2, 2, 4)
plt.plot(x_log, conflicts / x, marker='s', color='g', label='Количество конфликтов')
plt.title('График для относительного значения конфликтов от числа итераций')
plt.xlabel('Количество итераций')
plt.ylabel('Отн. ч. конфликтов')
plt.legend()
plt.grid(True)

# Сохраняем график в формате PNG
plt.tight_layout()
plt.savefig('plot.png')

# Показываем график
plt.show()
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 2 // Число потоков
#define NUM_ITERATIONS 10000000 // Число итераций для каждого потока
#define WAIT_TIME 1 // Время ожидания в секундах

int want[NUM_THREADS] = {0, 0}; // Массив флагов для потоков
int turn; // Переменная, указывающая чей сейчас ход
volatile int conflict_counter = 0; // Счетчик конфликтов

void *thread_function(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        want[id] = 1; // Я хочу войти
        turn = 1 - id; // Теперь очередь другого потока
        while (want[1 - id] && turn == 1 - id); // Ждем, пока другой поток не закончится

        // Критическая секция
        printf("Поток %d вошёл в критическую секцию\n", id);
        sleep(WAIT_TIME); // Имитация работы в критической секции
        printf("Поток %d покинул критическую секцию\n", id);

        // Проверка на конфликты
        if (want[1 - id]) {
            conflict_counter++;
            printf("Конфликт обнаружен между потоками!\n");
        }

        // Завершение ввода
        want[id] = 0; // Я вышел
    }

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS] = {0, 1};

    // Запуск потоков
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, thread_function, (void *)&thread_ids[i]);
    }

    // Ожидание завершения потоков
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("Всего конфликтов: %d\n", conflict_counter);
    return 0;
}
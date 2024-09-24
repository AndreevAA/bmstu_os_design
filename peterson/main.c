#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 2

int want[NUM_THREADS] = {0, 0}; // Массив флагов для потоков
int turn; // Переменная, указывающая чей сейчас ход

void *thread_function(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 5; ++i) {
        // Этап ожидания
        want[id] = 1; // Я хочу войти
        turn = 1 - id; // Теперь очередь другого потока
        while (want[1 - id] && turn == 1 - id); // Ждем, пока другой поток не закончится

        // Критическая секция
        printf("Поток %d вошёл в критическую секцию\n", id);
        sleep(1); // Имитация работы в критической секции
        printf("Поток %d покинул критическую секцию\n", id);

        // Завершение ввода
        want[id] = 0; // Я вышел
    }

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS] = {0, 1};

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, thread_function, (void *)&thread_ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
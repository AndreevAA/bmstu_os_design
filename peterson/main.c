#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define NUM_THREADS 2 // Число потоков
#define NUM_ITERATIONS 1000 // Число итераций для каждого потока
#define WAIT_TIME 1 // Время ожидания в секундах

int want[NUM_THREADS] = {0, 0}; // Массив флагов для потоков
int turn; // Переменная, указывающая чей сейчас ход
volatile int conflict_counter = 0; // Счетчик конфликтов
volatile int total_counter = 0;

static int lim[ 16 ] = { 1, 13, 12, 9, 7, 15, 4, 3, 2,  11, 14, 5, 6, 8, 10, 0 };

void *thread_function(void *arg) {
    int id = *(int *)arg;
    int n = 1231;

    for (int i = 0; i < NUM_ITERATIONS; ++i) {

        for (volatile int j = 10*lim[n & 15] + 1; j >= 0; --j) {
        }

        want[id] = 1; // Я хочу войти
        turn = 1 - id; // Теперь очередь другого потока
        while (want[1 - id] && turn == 1 - id); // Ждем, пока другой поток не закончится
        n = __atomic_fetch_add( &total_counter, 1, __ATOMIC_SEQ_CST );
        // Критическая секция
//        printf("Поток %d вошёл в критическую секцию\n", id);
//        sleep(WAIT_TIME); // Имитация работы в критической секции
//        printf("Поток %d покинул критическую секцию\n", id);

        for (volatile int j = 10*lim[n & 15] + 1; j >= 0; --j) {
        }

        // Проверка на конфликты
        if ( 1 == ( 1 & n ) ) {
            __atomic_fetch_add( &conflict_counter, 1, __ATOMIC_SEQ_CST );
//            printf("Конфликт обнаружен между потоками!\n");
        }
        __atomic_fetch_add( &total_counter, 1, __ATOMIC_SEQ_CST );

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
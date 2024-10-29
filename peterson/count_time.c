#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 2 // Число потоков
#define NUM_ITERATIONS 1000000 // Число итераций для каждого потока
#define WAIT_TIME 1 // Время ожидания в секундах

int want[NUM_THREADS] = {0, 0}; // Массив флагов для потоков
int turn; // Переменная, указывающая чей сейчас ход
volatile int conflict_counter = 0; // Счетчик конфликтов
volatile int total_counter = 0;
volatile int critical_section_counter_first = 0; // Счетчик нахождения в критической секции потока 1
volatile int critical_section_counter_second = 0; // Счетчик нахождения в критической секции потока 2

double critical_section_time[NUM_THREADS] = {0.0, 0.0}; // Массив для времени пребывания в критической секции

static int lim[ 16 ] = { 1, 13, 12, 9, 7, 15, 4, 3, 2,  11, 14, 5, 6, 8, 10, 0 };

struct timespec start_time_all, end_time_all;

void *thread_function(void *arg) {
    int id = *(int *)arg;
    int n = 123156;

    for (int i = 0; i < NUM_ITERATIONS; ++i) {

        for (volatile int j = 10*lim[n & 15] + 1; j >= 0; --j) {
        }

        want[id] = 1; // Я хочу войти
        turn = 1 - id; // Теперь очередь другого потока
        while (want[1 - id] && turn == 1 - id); // Ждем, пока другой поток не закончится
        n = __atomic_fetch_add( &total_counter, 1, __ATOMIC_SEQ_CST );
        // Критическая секция
        struct timespec start_time, end_time;
        clock_gettime(CLOCK_MONOTONIC, &start_time); // Запоминаем время входа в критическую секцию
//        printf("Поток %d вошёл в критическую секцию\n", id);
//        sleep(WAIT_TIME); // Имитация работы в критической секции
//        printf("Поток %d покинул критическую секцию\n", id);

        for (volatile int j = 10*lim[n & 15] + 1; j >= 0; --j) {
        }

        if (turn == 1)
        {
            __atomic_fetch_add( &critical_section_counter_first, 1, __ATOMIC_SEQ_CST );
        }
        else
        {
            __atomic_fetch_add( &critical_section_counter_second, 1, __ATOMIC_SEQ_CST );
        }


        // Проверка на конфликты
        if ( 1 == ( 1 & n ) ) {
            __atomic_fetch_add( &conflict_counter, 1, __ATOMIC_SEQ_CST );
//            printf("Конфликт обнаружен между потоками!\n");
        }
        __atomic_fetch_add( &total_counter, 1, __ATOMIC_SEQ_CST );

        clock_gettime(CLOCK_MONOTONIC, &end_time); // Запоминаем время выхода из критической секции

        // Считаем время, проведенное в критической секции
        double time_spent = (end_time.tv_sec - start_time.tv_sec) +
                            (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
        critical_section_time[id] += time_spent;

        // Завершение ввода
        want[id] = 0; // Я вышел
    }

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS] = {0, 1};

    clock_gettime(CLOCK_MONOTONIC, &start_time_all);
    // Запуск потоков
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, thread_function, (void *)&thread_ids[i]);
    }

    // Ожидание завершения потоков
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time_all); // Запоминаем время выхода из критической секции

    // Считаем время, проведенное в критической секции
    double time_spent_all = (end_time_all.tv_sec - start_time_all.tv_sec) +
                        (end_time_all.tv_nsec - start_time_all.tv_nsec) / 1e9;

    printf("Всего конфликтов: %d\n", conflict_counter);
    printf("\n");

    printf("Поток 1\n");
    printf("Всего итераций в критической секции 1 поток: %d\n", critical_section_counter_first);
    printf("Времени в критической секции 1 поток: %f\n", critical_section_time[0]);
    printf("Относительное время в критической секции 1 поток: %f per cent\n", critical_section_time[0] / time_spent_all * 100);


    printf("\n");
    printf("Поток 2\n");
    printf("Всего итераций в критической секции 2 поток: %d\n", critical_section_counter_second);
    printf("Времени в критической секции 2 поток: %f\n", critical_section_time[1]);
    printf("Относительное время в критической секции 1 поток: %f per cent\n", critical_section_time[1] / time_spent_all * 100);

    return 0;
}
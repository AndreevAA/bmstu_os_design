#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>



#define NUM_THREADS 2 // Количество потоков
#define WAIT_TIME 10 // Время ожидания в "операциях"
#define MAX_TESTS 14 // Максимальное количество тестов
#define MAX_ITERATIONS 2500 // Максимальное количество итераций для теста

int want[NUM_THREADS] = {0, 0}; // Массив флагов для потоков
int turn; // Переменная, указывающая чей сейчас ход
volatile int conflict_counter = 0; // Счетчик конфликтов
volatile int total_counter = 0;
volatile int operation_cnt_id_zero = 0; // Счетчик операций для потока 0
volatile int operation_cnt_id_one = 0; // Счетчик операций для потока 1

static int lim[ 16 ] = { 1, 13, 12, 9, 7, 15, 4, 3, 2,  11, 14, 5, 6, 8, 10, 0 };

volatile int flag1, flag2;

void *thread_function(void *arg) {

    int id = *(int *)arg;
    int n = 1231;

    int iterations = *(int *)(arg + sizeof(int));

    __atomic_fetch_add( &flag1, 1, __ATOMIC_SEQ_CST );
    while (!flag2) {}

    for (int i = 0; i < iterations; ++i) {
        // Выполнение операций
        for (volatile int j = 10*lim[n & 15] + 1; j >= 0; --j) {
        }

        want[id] = 1; // Я хочу войти
        turn = 1 - id; // Теперь очередь другого потока

        // Ожидание, пока не пройдет очередь
        while (want[1 - id] && turn == 1 - id) {
            // Увеличиваем счетчик операций во время ожидания
            if (id == 1) {
//                __atomic_fetch_add( &operation_cnt_id_zero, 1, __ATOMIC_SEQ_CST );
                git ++;
            } else {
//                __atomic_fetch_add( &operation_cnt_id_one, 1, __ATOMIC_SEQ_CST );
                operation_cnt_id_zero++;
            }
        }

        n = __atomic_fetch_add( &total_counter, 1, __ATOMIC_SEQ_CST );

        for (volatile int j = 10*lim[n & 15] + 1; j >= 0; --j) {
        }

        if ( 1 == ( 1 & n ) ) {
            __atomic_fetch_add( &conflict_counter, 1, __ATOMIC_SEQ_CST );
//            printf("Конфликт обнаружен между потоками!\n");
        }

        // Критическая секция
//        __atomic_fetch_add( &conflict_counter, 1, __ATOMIC_SEQ_CST ); // Увеличиваем счетчик конфликтов
        // Параллельные операции
        __atomic_fetch_add( &total_counter, 1, __ATOMIC_SEQ_CST ); ;

        want[id] = 0; // Я выходил
    }

    return NULL;
}

void run_tests_1() {
    FILE *file = fopen("results.txt", "w");
//    int iterations_arr[MAX_TESTS] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    int iterations_arr[MAX_TESTS] = {100, 200, 500, 1000, 2000, 5000, 10000, 20000, 40000, 80000, 160000, 350000, 1000000, 5000000};
//    int iterations_arr[MAX_TESTS] = {1000000, 2000000, 3000000, 4000000, 5000000, 6000000, 7000000};

    for (int t = 0; t < MAX_TESTS; t++) {
        int iterations = iterations_arr[t];
        pthread_t threads[NUM_THREADS];
        operation_cnt_id_zero = 0;
        operation_cnt_id_one = 0;
        conflict_counter = 0;
        total_counter = 0;

        // Передаем количество итераций в аргументах потокам
        int thread_args[NUM_THREADS][2] = {{0, iterations}, {1, iterations}};

        flag1 = flag2 = 0;
        for (int i = 0; i < NUM_THREADS; i++) {
            pthread_create(&threads[i], NULL, thread_function, (void*)thread_args[i]);
        }

        while ( flag1 != 2 ) {}
        flag2=1;

        for (int i = 0; i < NUM_THREADS; i++) {
            pthread_join(threads[i], NULL);
        }

        // Выводим в файл количество итераций, операций для каждого потока и количество конфликтов
        fprintf(file, "%d %d %d %d\n", iterations, operation_cnt_id_zero, operation_cnt_id_one, conflict_counter);
    }

    fclose(file);
}

void run_tests_2() {
    FILE *file = fopen("results.txt", "w");
    int iterations_arr[MAX_TESTS] = {5000000, 5000000, 5000000, 5000000, 5000000, 5000000, 5000000, 5000000, 5000000, 5000000, 5000000, 5000000, 5000000, 5000000};

    for (int t = 0; t < MAX_TESTS; t++) {
        int iterations = iterations_arr[t];
        pthread_t threads[NUM_THREADS];
        operation_cnt_id_zero = 0;
        operation_cnt_id_one = 0;
        conflict_counter = 0;
        total_counter = 0;

        // Передаем количество итераций в аргументах потокам
        int thread_args[NUM_THREADS][2] = {{0, iterations}, {1, iterations}};

        for (int i = 0; i < NUM_THREADS; i++) {
            pthread_create(&threads[i], NULL, thread_function, (void*)thread_args[i]);
        }

        for (int i = 0; i < NUM_THREADS; i++) {
            pthread_join(threads[i], NULL);
        }

        // Выводим в файл количество итераций, операций для каждого потока и количество конфликтов
        fprintf(file, "%d %d %d %d\n", iterations, operation_cnt_id_zero, operation_cnt_id_one, conflict_counter);
    }

    fclose(file);
}

void plot_results() {
    FILE *pipe = popen("gnuplot -persistent", "w");
    if (pipe == NULL) {
        fprintf(stderr, "Не удалось открыть поток для gnuplot\n");
        return;
    }

    fprintf(pipe, "set title 'Количество операций ожидания'\n");
    fprintf(pipe, "set xlabel 'Количество итераций'\n");
    fprintf(pipe, "set ylabel 'Количество операций'\n");
    fprintf(pipe, "set style data linespoints\n");
    fprintf(pipe, "plot 'results.txt' using 1:2 title 'Поток 0', '' using 1:3 title 'Поток 1'\n");

    pclose(pipe);
}

int main() {
    run_tests_1();
//    run_tests_2();
//    plot_results();

    return 0;
}
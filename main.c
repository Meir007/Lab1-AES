#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

typedef struct {
    unsigned char data[4][4];
} StateMatrix;

void MixColumns(StateMatrix *state) {
    StateMatrix result;
    for (int c = 0; c < 4; c++) {
        result.data[0][c] = (unsigned char)((state->data[0][c] << 1) ^ (state->data[1][c] << 1) ^ state->data[1][c] ^ state->data[2][c] ^ state->data[3][c]);
        result.data[1][c] = (unsigned char)(state->data[0][c] ^ (state->data[0][c] << 1) ^ state->data[1][c] ^ (state->data[2][c] << 1) ^ state->data[2][c]);
        result.data[2][c] = (unsigned char)(state->data[2][c] ^ state->data[0][c] ^ state->data[1][c] ^ (state->data[1][c] << 1) ^ (state->data[3][c] << 1));
        result.data[3][c] = (unsigned char)((state->data[0][c] << 1) ^ state->data[1][c] ^ state->data[2][c] ^ (state->data[2][c] << 1) ^ (state->data[3][c] << 1));
    }
    memcpy(state->data, result.data, 16);
}

void *ThreadMixColumns(void *arg) {
    StateMatrix *matrices = (StateMatrix *)arg;
    MixColumns(matrices);
    pthread_exit(NULL);
}

int readMatricesFromFile(const char *filename, StateMatrix matrices[], int N) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Unable to open file");
        return 0;
    }

    for (int i = 0; i < N; i++) {
        if (fread(matrices[i].data, sizeof(unsigned char), 16, file) != 16) {
            fprintf(stderr, "Error reading matrix %d from file.\n", i);
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    return 1;
}

int writeMatricesToFile(const char *filename, StateMatrix matrices[], int N, double executionTime, size_t dataSizeInBytes) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Unable to open file");
        return 0;
    }

    for (int i = 0; i < N; i++) {
        if (fwrite(matrices[i].data, sizeof(unsigned char), 16, file) != 16) {
            fprintf(stderr, "Error writing matrix %d to file.\n", i);
            fclose(file);
            return 0;
        }
    }

    fseek(file, 0, SEEK_END); // Перейти в конец файла
    fprintf(file, "Execution Time: %lf seconds\n", executionTime);
    fprintf(file, "Data Size Processed: %zu bytes\n", dataSizeInBytes);

    fclose(file);
    return 1;
}

void generateRandomData(const char *filename, size_t sizeInMB) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Unable to create file");
        return;
    }

    size_t dataSizeInBytes = sizeInMB * 1024 * 1024;

    srand(time(NULL)); // Инициализация генератора случайных чисел

    for (size_t i = 0; i < dataSizeInBytes; i++) {
        unsigned char byte = rand() % 256;
        fwrite(&byte, sizeof(unsigned char), 1, file);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <size_in_MB>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    size_t sizeInMB = atoi(argv[2]);

    if (sizeInMB <= 0) {
        fprintf(stderr, "Invalid size in MB.\n");
        return 1;
    }

    const int num_experiments = 5;
    const int data_sizes[] = {1, 2, 4, 8, 16}; // Размеры данных в мегабайтах
    const int num_threads[] = {1, 2, 4, 8, 16}; // Разные степени параллелизма

    FILE *outputFile = fopen("execution_times.txt", "w");
    if (outputFile == NULL) {
        perror("Unable to open output file");
        return 1;
    }

    for (int i = 0; i < num_experiments; i++) {
        size_t sizeInMB = data_sizes[i];
        generateRandomData("input.dat", sizeInMB);

        for (int j = 0; j < sizeof(num_threads) / sizeof(num_threads[0]); j++) {
            int N = 10; // Количество матриц
            StateMatrix matrices[N];

            if (readMatricesFromFile("input.dat", matrices, N)) {
                pthread_t threads[num_threads[j]];
                clock_t start = clock();

                for (int k = 0; k < N; k++) {
                    pthread_create(&threads[k % num_threads[j]], NULL, ThreadMixColumns, &matrices[k]);
                }

                for (int k = 0; k < N; k++) {
                    pthread_join(threads[k % num_threads[j]], NULL);
                }

                clock_t end = clock();
                double executionTime = (double)(end - start) / CLOCKS_PER_SEC;

                fprintf(outputFile, "Data Size: %d MB, Threads: %d, Execution Time: %lf seconds\n", data_sizes[i], num_threads[j], executionTime);
            }
        }
    }

    fclose(outputFile);

    printf("MixColumns operation completed and saved to execution_times.txt\n");

    return 0;
}

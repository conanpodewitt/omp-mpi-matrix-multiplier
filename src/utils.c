#include "utils.h"

int** allocate_2d_array(int rows, int cols) {
    int** arr = (int**)malloc(rows * sizeof(int*));
    if (arr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    for (int i = 0; i < rows; i++) {
        arr[i] = (int*)calloc(cols, sizeof(int));
        if (arr[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    }
    return arr;
}

void free_2d_array(int **arr, int rows) {
    for (int i = 0; i < rows; i++) {
        free(arr[i]);
    }
    free(arr);
}

char* create_run_directory() {
    static char dir_name[20];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(dir_name, sizeof(dir_name), "Run%Y%m%d%H%M%S", tm);
    create_directory(dir_name);
    return dir_name;
}

void create_directory(const char *path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
    }
}

void write_matrix_to_file(const char *filename, int **matrix, int rows, int cols) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return;
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%d", matrix[i][j]);
            if (j < cols - 1) fprintf(file, ",");
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void write_compressed_matrix_to_file(const char *filename, int **matrix, int rows, int cols) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return;
    }
    for (int i = 0; i < rows; i++) {
        int count = 0;
        for (int j = 0; j < cols; j++) {
            if (matrix[i][j] != 0) {
                fprintf(file, "%d,%d", j, matrix[i][j]);
                count++;
                if (count < matrix[i][0]) fprintf(file, ";");
            }
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void write_performance_to_file(const char *filename, int num_rows, int num_cols, float prob, int schedule, int chunk_size, double *times, int num_threads) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return;
    }
    char schedule_str[16];
    switch (schedule) {
        case 1:
            strcpy(schedule_str, "Static");
            break;
        case 2:
            strcpy(schedule_str, "Dynamic");
            break;
        case 3:
            strcpy(schedule_str, "Guided");
            break;
        case 4:
            strcpy(schedule_str, "Auto");
            break;
    }
    char chunk_size_str[16];
    if (chunk_size == 0) {
        strcpy(chunk_size_str, "Default");
    } else {
        snprintf(chunk_size_str, sizeof(chunk_size_str), "%d", chunk_size);
    }
    fprintf(file, "NumRows,NumCols,NonZero,Schedule,ChunkSize,NumThreads,Time\n");
    for (int i = 0; i < num_threads; i++) {
        fprintf(file, "%d,%d,%f,%s,%s,%d,%f\n", num_rows, num_cols, prob, schedule_str, chunk_size_str, (i+1)*4, times[i]);
    }
    fclose(file);
}

void write_result_to_file(const char *filename, int **matrix, int rows, int cols) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return;
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%d", matrix[i][j]);
            if (j < cols - 1) fprintf(file, ",");
        }
        fprintf(file, "\n");
    }
    fclose(file);
}
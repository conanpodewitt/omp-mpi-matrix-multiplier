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

void write_performance_to_file(const char *filename, int schedule, int chunk_size, int num_rows, int num_cols, double *times, int num_threads) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return;
    }
    fprintf(file, "Schedule,ChunkSize,NumRows,NumCols,NumThreads,Time\n");
    for (int i = 0; i < num_threads; i++) {
        fprintf(file, "%d,%d,%d,%d,%d,%f\n", schedule, chunk_size, num_rows, num_cols, (i+1)*4, times[i]);
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
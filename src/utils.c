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

void create_directory(const char *path) {
    #ifdef _WIN32
        _mkdir(path);
    #else
        mkdir(path, 0777);
    #endif
}

void write_matrix_to_csv(const char *filename, int **matrix, int rows, int cols) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
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

void write_compressed_matrix_to_csv(const char *filename, int **values, int **cols, int *row_counts, int rows) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < row_counts[i]; j++) {
            fprintf(file, "%d,%d", cols[i][j], values[i][j]);
            if (j < row_counts[i] - 1) fprintf(file, ";");
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void write_performance_to_csv(const char *filename, int num_threads, double time, int append) {
    FILE *file = fopen(filename, append ? "a" : "w");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    if (!append) {
        fprintf(file, "Threads,Time (s)\n");
    }
    fprintf(file, "%d,%f\n", num_threads, time);

    fclose(file);
}

char* create_run_directory() {
    static int run_count = 0;
    char *dir_name = malloc(20 * sizeof(char));
    sprintf(dir_name, "Run%03d", ++run_count);
    create_directory(dir_name);
    return dir_name;
}

char* create_thread_directory(const char* run_dir, int thread_count) {
    char *dir_name = malloc(50 * sizeof(char));
    if (thread_count == 0) {
        sprintf(dir_name, "%s/Ordinary", run_dir);
    } else {
        sprintf(dir_name, "%s/Threads%d", run_dir, thread_count);
    }
    create_directory(dir_name);
    return dir_name;
}
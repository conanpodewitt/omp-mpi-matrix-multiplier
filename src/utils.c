#include "utils.h"
#include <string.h>
#include <errno.h>

int** allocate_2d_array(int rows, int cols) {
  int **arr = (int **)malloc(rows * sizeof(int *));
  if (arr == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  for (int i = 0; i < rows; i++) {
    arr[i] = (int *)malloc(cols * sizeof(int));
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

SparseMatrix* allocate_sparse_matrix(int nnz) {
  SparseMatrix *matrix = (SparseMatrix *)malloc(sizeof(SparseMatrix));
  if (matrix == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  matrix->B = (int *)malloc(nnz * sizeof(int));
  matrix->C = (int *)malloc(nnz * sizeof(int));
  matrix->row_ptr = (int *)malloc((MATRIX_SIZE + 1) * sizeof(int));

  if (matrix->B == NULL || matrix->C == NULL || matrix->row_ptr == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  matrix->nnz = nnz;

  return matrix;
}

void free_sparse_matrix(SparseMatrix *matrix) {
  free(matrix->B);
  free(matrix->C);
  free(matrix->row_ptr);
  free(matrix);
}

void write_sparse_matrix_to_file(const char *filename, SparseMatrix *matrix) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    fprintf(stderr, "Unable to open file for writing\n");
    return;
  }

  fprintf(file, "%d\n", matrix->nnz);
  
  for (int i = 0; i < matrix->nnz; i++) {
    fprintf(file, "%d %d\n", matrix->B[i], matrix->C[i]);
  }

  for (int i = 0; i <= MATRIX_SIZE; i++) {
    fprintf(file, "%d\n", matrix->row_ptr[i]);
  }

  fclose(file);
}

SparseMatrix* read_sparse_matrix_from_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "Unable to open file for reading\n");
    return NULL;
  }

  int nnz;
  fscanf(file, "%d", &nnz);

  SparseMatrix *matrix = allocate_sparse_matrix(nnz);

  for (int i = 0; i < nnz; i++) {
    fscanf(file, "%d %d", &matrix->B[i], &matrix->C[i]);
  }

  for (int i = 0; i <= MATRIX_SIZE; i++) {
    fscanf(file, "%d", &matrix->row_ptr[i]);
  }

  fclose(file);
  return matrix;
}

double get_time() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

void print_boxed_output(int num_threads, double time, int nnz, int last) {
  printf("+------------------------+------------------+----------------------+\n");
  printf("| Threads                | Time (seconds)   | Non-zero Elements    |\n");
  printf("+------------------------+------------------+----------------------+\n");
  printf("| %-22d | %-16.6f | %-20d |\n", num_threads, time, nnz);
  if (last) {
    printf("+------------------------+------------------+----------------------+\n");
  }
}

void create_directory(const char *path) {
  struct stat st = {0};
  if (stat(path, &st) == -1) {
    if (mkdir(path, 0700) == -1) {
      fprintf(stderr, "Error creating directory %s: %s\n", path, strerror(errno));
      exit(1);
    }
  }
}

void write_dense_matrix_to_csv(const char *filename, int **matrix, int rows, int cols) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    fprintf(stderr, "Unable to open file for writing: %s\n", filename);
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

void write_sparse_matrix_to_csv(const char *filename, SparseMatrix *matrix) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    fprintf(stderr, "Unable to open file for writing: %s\n", filename);
    return;
  }

  fprintf(file, "Row,Column,Value\n");
  int row = 0;
  for (int i = 0; i < matrix->nnz; i++) {
    while (i >= matrix->row_ptr[row + 1]) row++;
    fprintf(file, "%d,%d,%d\n", row, matrix->C[i], matrix->B[i]);
  }

  fclose(file);
}

void write_performance_to_csv(const char *filename, int num_threads, double time, int nnz) {
  FILE *file = fopen(filename, "a");
  if (file == NULL) {
    fprintf(stderr, "Unable to open file for writing: %s\n", filename);
    return;
  }

  fprintf(file, "%d,%.6f,%d\n", num_threads, time, nnz);

  fclose(file);
}
#include "utils.h"
#include <string.h>

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
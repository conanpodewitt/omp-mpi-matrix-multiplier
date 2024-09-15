#include "matrix_generation.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

SparseMatrix* generate_sparse_matrix(double non_zero_prob) {
  int nnz = 0;
  int *temp_B = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
  int *temp_C = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
  int *row_ptr = calloc(MATRIX_SIZE + 1, sizeof(int));

  if (!temp_B || !temp_C || !row_ptr) {
    fprintf(stderr, "Memory allocation failed in generate_sparse_matrix\n");
    exit(1);
  }

  srand(time(NULL));

  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      if ((double)rand() / RAND_MAX < non_zero_prob) {
        temp_B[nnz] = rand() % 10 + 1;  // Random integer between 1 and 10
        temp_C[nnz] = j;
        nnz++;
        row_ptr[i + 1]++;
      }
    }
  }

  // Calculate cumulative sum for row_ptr
  for (int i = 1; i <= MATRIX_SIZE; i++) {
    row_ptr[i] += row_ptr[i - 1];
  }

  SparseMatrix *matrix = allocate_sparse_matrix(nnz);
  memcpy(matrix->B, temp_B, nnz * sizeof(int));
  memcpy(matrix->C, temp_C, nnz * sizeof(int));
  memcpy(matrix->row_ptr, row_ptr, (MATRIX_SIZE + 1) * sizeof(int));

  free(temp_B);
  free(temp_C);
  free(row_ptr);

  return matrix;
}

void generate_matrix_pair(double non_zero_prob, SparseMatrix **X, SparseMatrix **Y) {
  *X = generate_sparse_matrix(non_zero_prob);
  *Y = generate_sparse_matrix(non_zero_prob);
}
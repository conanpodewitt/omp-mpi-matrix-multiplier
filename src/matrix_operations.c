#include "matrix_operations.h"
#include <omp.h>

int** ordinary_matrix_multiply(int **A, int **B) {
  int **C = allocate_2d_array(MATRIX_SIZE, MATRIX_SIZE);

  #pragma omp parallel for collapse(2)
  for (int i = 0; i < MATRIX_SIZE; i++) {
  for (int j = 0; j < MATRIX_SIZE; j++) {
    C[i][j] = 0;
    for (int k = 0; k < MATRIX_SIZE; k++) {
      C[i][j] += A[i][k] * B[k][j];
    }
  }
}

  return C;
}

SparseMatrix* sparse_matrix_multiply(SparseMatrix *A, SparseMatrix *B) {
  int *temp_B = calloc(MATRIX_SIZE * MATRIX_SIZE, sizeof(int));
  int *temp_C = calloc(MATRIX_SIZE * MATRIX_SIZE, sizeof(int));
  int *row_ptr = calloc(MATRIX_SIZE + 1, sizeof(int));

  if (!temp_B || !temp_C || !row_ptr) {
    fprintf(stderr, "Memory allocation failed in sparse_matrix_multiply\n");
    exit(1);
  }

  int nnz = 0;

  #pragma omp parallel
  {
    int *local_temp_B = calloc(MATRIX_SIZE, sizeof(int));
    int *local_temp_C = calloc(MATRIX_SIZE, sizeof(int));
    int local_nnz = 0;

    #pragma omp for schedule(dynamic)
    for (int i = 0; i < MATRIX_SIZE; i++) {
      for (int j = A->row_ptr[i]; j < A->row_ptr[i+1]; j++) {
        int col_A = A->C[j];
        int val_A = A->B[j];

        for (int k = B->row_ptr[col_A]; k < B->row_ptr[col_A+1]; k++) {
          int col_B = B->C[k];
          int val_B = B->B[k];

          local_temp_B[col_B] += val_A * val_B;
        }
      }

      for (int j = 0; j < MATRIX_SIZE; j++) {
        if (local_temp_B[j] != 0) {
          local_temp_C[local_nnz] = j;
          local_temp_B[local_nnz] = local_temp_B[j];
          local_nnz++;
          local_temp_B[j] = 0;
        }
      }

      #pragma omp critical
      {
        memcpy(&temp_B[nnz], local_temp_B, local_nnz * sizeof(int));
        memcpy(&temp_C[nnz], local_temp_C, local_nnz * sizeof(int));
        row_ptr[i+1] = nnz + local_nnz;
        nnz += local_nnz;
      }

      local_nnz = 0;
    }

    free(local_temp_B);
    free(local_temp_C);
  }

  SparseMatrix *C = allocate_sparse_matrix(nnz);
  memcpy(C->B, temp_B, nnz * sizeof(int));
  memcpy(C->C, temp_C, nnz * sizeof(int));
  memcpy(C->row_ptr, row_ptr, (MATRIX_SIZE + 1) * sizeof(int));

  free(temp_B);
  free(temp_C);
  free(row_ptr);

  return C;
}

int** sparse_to_dense(SparseMatrix *A) {
  int **dense = allocate_2d_array(MATRIX_SIZE, MATRIX_SIZE);

  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = A->row_ptr[i]; j < A->row_ptr[i+1]; j++) {
      dense[i][A->C[j]] = A->B[j];
    }
  }

  return dense;
}

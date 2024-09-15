#ifndef MATRIX_OPERATIONS_H
#define MATRIX_OPERATIONS_H

#include "utils.h"

// Perform ordinary matrix multiplication
double** ordinary_matrix_multiply(double **A, double **B);

// Perform sparse matrix multiplication
SparseMatrix* sparse_matrix_multiply(SparseMatrix *A, SparseMatrix *B);

// Convert a sparse matrix to a dense matrix (for testing purposes)
double** sparse_to_dense(SparseMatrix *A);

#endif // MATRIX_OPERATIONS_H
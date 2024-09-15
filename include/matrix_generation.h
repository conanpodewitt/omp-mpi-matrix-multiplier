#ifndef MATRIX_GENERATION_H
#define MATRIX_GENERATION_H

#include "utils.h"

// Generate a sparse matrix with given non-zero probability
SparseMatrix* generate_sparse_matrix(int non_zero_prob);

// Generate a pair of sparse matrices for multiplication
void generate_matrix_pair(int non_zero_prob, SparseMatrix **X, SparseMatrix **Y);

#endif // MATRIX_GENERATION_H
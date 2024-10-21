#ifndef MATRIX_OPERATIONS_H
#define MATRIX_OPERATIONS_H

#include "utils.h"

// Function prototypes
int** sequential_matrix_multiply(int** matrix1, int** matrix2);
int** omp_compressed_matrix_multiply(int** matrix_xb, int** matrix_xc, int* row_counts_x, 
                                     int** matrix_yb, int** matrix_yc, int* row_counts_y);
int** mpi_compressed_matrix_multiply(int** matrix_xb, int** matrix_xc, int* row_counts_x, 
                                     int** matrix_yb, int** matrix_yc, int* row_counts_y);
void compress_matrix(int** matrix, int*** compressed_values, int*** compressed_cols, int** row_counts);

#endif // MATRIX_OPERATIONS_H
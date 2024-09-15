#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define matrix size
#define MATRIX_SIZE 1000

// Structure to represent a sparse matrix in row-compressed format
typedef struct {
  int *B;  // Array to store non-zero elements
  int *C;  // Array to store column indices of non-zero elements
  int *row_ptr;  // Array to store starting index of each row in B and C
  int nnz;  // Number of non-zero elements
} SparseMatrix;

// Function prototypes

// Allocate memory for a 2D array
int** allocate_2d_array(int rows, int cols);

// Free memory for a 2D array
void free_2d_array(int **arr, int rows);

// Allocate memory for a SparseMatrix structure
SparseMatrix* allocate_sparse_matrix(int nnz);

// Free memory for a SparseMatrix structure
void free_sparse_matrix(SparseMatrix *matrix);

// Write sparse matrix to file
void write_sparse_matrix_to_file(const char *filename, SparseMatrix *matrix);

// Read sparse matrix from file
SparseMatrix* read_sparse_matrix_from_file(const char *filename);

// Timer function to measure execution time
double get_time();

// Display function to neatly format output
void print_boxed_output(int num_threads, double time, int nnz, int last);

#endif // UTILS_H

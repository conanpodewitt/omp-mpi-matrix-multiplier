#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include "utils.h"
#include "matrix_generation.h"
#include "matrix_operations.h"

#define NUM_EXPERIMENTS 3
#define MAX_THREADS 32

void run_experiment(double non_zero_prob, const char *base_dir) {
  printf("\nRunning experiment with non-zero probability: %.2f\n", non_zero_prob);

  char prob_dir[256];
  snprintf(prob_dir, sizeof(prob_dir), "%s/prob_%.2f", base_dir, non_zero_prob);
  create_directory(prob_dir);

  SparseMatrix *X, *Y;
  generate_matrix_pair(non_zero_prob, &X, &Y);

  printf("Matrix X: %d non-zero elements\n", X->nnz);
  printf("Matrix Y: %d non-zero elements\n", Y->nnz);

  // Write input matrices to CSV
  char filename[256];
  snprintf(filename, sizeof(filename), "%s/matrix_X.csv", prob_dir);
  write_sparse_matrix_to_csv(filename, X);
  snprintf(filename, sizeof(filename), "%s/matrix_Y.csv", prob_dir);
  write_sparse_matrix_to_csv(filename, Y);

  // Ordinary matrix multiplication (for small matrices only)
  if (MATRIX_SIZE <= 1000) {
    char ordinary_dir[256];
    snprintf(ordinary_dir, sizeof(ordinary_dir), "%s/ordinary", prob_dir);
    create_directory(ordinary_dir);

    int **dense_X = sparse_to_dense(X);
    int **dense_Y = sparse_to_dense(Y);

    double start_time = get_time();
    int **result_ordinary = ordinary_matrix_multiply(dense_X, dense_Y);
    double end_time = get_time();

    printf("Ordinary matrix multiplication time: %.6f seconds\n", end_time - start_time);

    // Write result to CSV
    snprintf(filename, sizeof(filename), "%s/result_ordinary.csv", ordinary_dir);
    write_dense_matrix_to_csv(filename, result_ordinary, MATRIX_SIZE, MATRIX_SIZE);

    // Write performance to CSV
    snprintf(filename, sizeof(filename), "%s/performance_ordinary.csv", ordinary_dir);
    write_performance_to_csv(filename, 1, end_time - start_time, MATRIX_SIZE * MATRIX_SIZE);

    // Free memory
    free_2d_array(dense_X, MATRIX_SIZE);
    free_2d_array(dense_Y, MATRIX_SIZE);
    free_2d_array(result_ordinary, MATRIX_SIZE);
  } else {
    printf("Skipping ordinary matrix multiplication due to large matrix size.\n");
  }

  // Sparse matrix multiplication with varying thread counts
  char sparse_dir[256];
  snprintf(sparse_dir, sizeof(sparse_dir), "%s/sparse", prob_dir);
  create_directory(sparse_dir);

  char performance_file[256];
  snprintf(performance_file, sizeof(performance_file), "%s/performance_sparse.csv", sparse_dir);
  FILE *perf_file = fopen(performance_file, "w");
  fprintf(perf_file, "Threads,Time,NonZeroElements\n");
  fclose(perf_file);

  for (int num_threads = 1; num_threads <= MAX_THREADS; num_threads *= 2) {
    omp_set_num_threads(num_threads);

    double start_time = get_time();
    SparseMatrix *result_sparse = sparse_matrix_multiply(X, Y);
    double end_time = get_time();

    // Write result to CSV
    snprintf(filename, sizeof(filename), "%s/result_sparse_threads_%d.csv", sparse_dir, num_threads);
    write_sparse_matrix_to_csv(filename, result_sparse);

    // Append performance to CSV
    write_performance_to_csv(performance_file, num_threads, end_time - start_time, result_sparse->nnz);

    // printf("Sparse matrix multiplication time with %d threads: %.6d seconds\n", 
    //         num_threads, end_time - start_time);
    // printf("Result matrix: %d non-zero elements\n", result_sparse->nnz);
    // Nicer formatting
    if (num_threads != MAX_THREADS) {
      print_boxed_output(num_threads, end_time - start_time, result_sparse->nnz, 0);
    } else {
      print_boxed_output(num_threads, end_time - start_time, result_sparse->nnz, 1);
    }

    // Free memory
    free_sparse_matrix(result_sparse);
  }

  // Free memory
  free_sparse_matrix(X);
  free_sparse_matrix(Y);
}

int main() {
  double non_zero_probs[] = {0.01, 0.02, 0.05};

  char base_dir[256];
  snprintf(base_dir, sizeof(base_dir), "data/run_%ld", time(NULL));
  create_directory("data");
  create_directory(base_dir);

  for (int i = 0; i < NUM_EXPERIMENTS; i++) {
      run_experiment(non_zero_probs[i], base_dir);
  }

  return 0;
}
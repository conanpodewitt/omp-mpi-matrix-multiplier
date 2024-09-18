#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <time.h>
#include "utils.h"
#include "matrix_generation.h"
#include "matrix_operations.h"

#define NUM_EXPERIMENTS 3
#define MAX_THREADS 32

void run_experiment(double non_zero_prob, const char *base_dir) {
  printf("\nRunning experiment with non-zero probability: %.2f\n", non_zero_prob);

  char* prob_dir = malloc(snprintf(NULL, 0, "%s/prob_%.2f", base_dir, non_zero_prob) + 1);
  sprintf(prob_dir, "%s/prob_%.2f", base_dir, non_zero_prob);
  create_directory(prob_dir);

  SparseMatrix *X, *Y;
  generate_matrix_pair(non_zero_prob, &X, &Y);

  printf("Matrix X: %d non-zero elements\n", X->nnz);
  printf("Matrix Y: %d non-zero elements\n", Y->nnz);

  // Write input matrices to CSV
  char* filename = create_path(prob_dir, "matrix_X.csv");
  write_sparse_matrix_to_csv(filename, X);
  free(filename);

  filename = create_path(prob_dir, "matrix_Y.csv");
  write_sparse_matrix_to_csv(filename, Y);
  free(filename);

  // Ordinary matrix multiplication (for small matrices only)
  if (MATRIX_SIZE <= 1000) {
    char* ordinary_dir = create_path(prob_dir, "ordinary");
    create_directory(ordinary_dir);

    int **dense_X = sparse_to_dense(X);
    int **dense_Y = sparse_to_dense(Y);

    double start_time = get_time();
    int **result_ordinary = ordinary_matrix_multiply(dense_X, dense_Y);
    double end_time = get_time();

    printf("Ordinary matrix multiplication time: %.6f seconds\n", end_time - start_time);

    // Write result to CSV
    filename = create_path(ordinary_dir, "result_ordinary.csv");
    write_dense_matrix_to_csv(filename, result_ordinary, MATRIX_SIZE, MATRIX_SIZE);
    free(filename);

    // Write performance to CSV
    filename = create_path(ordinary_dir, "performance_ordinary.csv");
    write_performance_to_csv(filename, 1, end_time - start_time, (unsigned int)MATRIX_SIZE * MATRIX_SIZE);
    free(filename);

      // Free memory
      free_2d_array(dense_X, MATRIX_SIZE);
      free_2d_array(dense_Y, MATRIX_SIZE);
      free_2d_array(result_ordinary, MATRIX_SIZE);
      free(ordinary_dir);
  } else {
      printf("Skipping ordinary matrix multiplication due to large matrix size.\n");
  }

  // Sparse matrix multiplication with varying thread counts
  char* sparse_dir = create_path(prob_dir, "sparse");
  create_directory(sparse_dir);

  char* performance_file = create_path(sparse_dir, "performance_sparse.csv");
  FILE *perf_file = fopen(performance_file, "w");
  fprintf(perf_file, "Threads,Time,NonZeroElements\n");
  fclose(perf_file);

  for (int num_threads = 1; num_threads <= MAX_THREADS; num_threads *= 2) {
    omp_set_num_threads(num_threads);

    double start_time = get_time();
    SparseMatrix *result_sparse = sparse_matrix_multiply(X, Y);
    double end_time = get_time();

    // Write result to CSV
    char* thread_filename = malloc(snprintf(NULL, 0, "result_sparse_threads_%d.csv", num_threads) + 1);
    sprintf(thread_filename, "result_sparse_threads_%d.csv", num_threads);
    filename = create_path(sparse_dir, thread_filename);
    write_sparse_matrix_to_csv(filename, result_sparse);
    free(filename);
    free(thread_filename);

    // Append performance to CSV
    write_performance_to_csv(performance_file, num_threads, end_time - start_time, result_sparse->nnz);

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
  free(prob_dir);
  free(sparse_dir);
  free(performance_file);
}

int main() {
  double non_zero_probs[] = {0.01, 0.02, 0.05};

  char* base_dir = malloc(snprintf(NULL, 0, "data/run_%ld", time(NULL)) + 1);
  sprintf(base_dir, "data/run_%ld", time(NULL));
  create_directory("data");
  create_directory(base_dir);

  for (int i = 0; i < NUM_EXPERIMENTS; i++) {
    run_experiment(non_zero_probs[i], base_dir);
  }

  free(base_dir);
  return 0;
}
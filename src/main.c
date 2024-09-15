#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "utils.h"
#include "matrix_generation.h"
#include "matrix_operations.h"

#define NUM_EXPERIMENTS 3
#define MAX_THREADS 32

void run_experiment(double non_zero_prob) {
  printf("\nRunning experiment with non-zero probability: %.2f\n", non_zero_prob);

  SparseMatrix *X, *Y;
  generate_matrix_pair(non_zero_prob, &X, &Y);

  printf("Matrix X: %d non-zero elements\n", X->nnz);
  printf("Matrix Y: %d non-zero elements\n", Y->nnz);

  // Ordinary matrix multiplication (for small matrices only)
  if (MATRIX_SIZE <= 1000) {
    int **dense_X = sparse_to_dense(X);
    int **dense_Y = sparse_to_dense(Y);

    int start_time = get_time();
    int **result_ordinary = ordinary_matrix_multiply(dense_X, dense_Y);
    int end_time = get_time();

    printf("Ordinary matrix multiplication time: %.6d seconds\n", end_time - start_time);

    // Free memory
    free_2d_array(dense_X, MATRIX_SIZE);
    free_2d_array(dense_Y, MATRIX_SIZE);
    free_2d_array(result_ordinary, MATRIX_SIZE);
  } else {
    printf("Skipping ordinary matrix multiplication due to large matrix size.\n");
  }

  // Sparse matrix multiplication with varying thread counts
  for (int num_threads = 1; num_threads <= MAX_THREADS; num_threads *= 2) {
    omp_set_num_threads(num_threads);

    double start_time = get_time();
    SparseMatrix *result_sparse = sparse_matrix_multiply(X, Y);
    double end_time = get_time();

    printf("Sparse matrix multiplication time with %d threads: %.6f seconds\n", 
            num_threads, end_time - start_time);
    printf("Result matrix: %d non-zero elements\n", result_sparse->nnz);

    // Free memory
    free_sparse_matrix(result_sparse);
  }

  // Experiment with different OpenMP scheduling strategies
  const char *schedules[] = {"static", "dynamic", "guided", "auto"};
  omp_set_num_threads(omp_get_max_threads());  // Use max available threads

  for (int i = 0; i < 4; i++) {
    double start_time = get_time();
    
    #pragma omp parallel for schedule(runtime)
    for (int j = 0; j < MATRIX_SIZE; j++) {
      // Dummy work to simulate computation
      int sum = 0;
      for (int k = 0; k < 1000000; k++) {
        sum += k;
      }
    }

    double end_time = get_time();

    printf("Time with %s scheduling: %.6f seconds\n", schedules[i], end_time - start_time);

    // Set the next scheduling strategy
    omp_set_schedule(omp_sched_auto, 0);
  }

  // Free memory
  free_sparse_matrix(X);
  free_sparse_matrix(Y);
}

int main() {
  double non_zero_probs[] = {0.01, 0.02, 0.05};

  for (int i = 0; i < NUM_EXPERIMENTS; i++) {
    run_experiment(non_zero_probs[i]);
  }

  return 0;
}
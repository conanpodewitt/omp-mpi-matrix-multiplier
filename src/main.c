
#include "utils.h"
#include "matrix_operations.h"
#include "matrix_generation.h"
#include <omp.h>

int main() {
    // Seed the random number generator
    srand(time(NULL));

    // Set the probability of non-zero elements
    float p = 0.01;
    printf("Probability of non-zero elements: %f\n", p);
    printf("Scheduling mode: %d\n", SCHEDULE);
    printf("Chunk size: %d\n", CHUNK_SIZE);

    // Generate matrix_x and matrix_y
    int** matrix_x = generate_matrix(p);
    int** matrix_y = generate_matrix(p);

    // Compress matrix_x
    int** matrix_xb;
    int** matrix_xc;
    int* row_counts_x;
    compress_matrix(matrix_x, &matrix_xb, &matrix_xc, &row_counts_x);

    // Compress matrix_y
    int** matrix_yb;
    int** matrix_yc;
    int* row_counts_y;
    compress_matrix(matrix_y, &matrix_yb, &matrix_yc, &row_counts_y);

    // Free the original matrices (matrix_x and matrix_y) as they are no longer needed
    free_2d_array(matrix_x, NUM_ROWS);
    free_2d_array(matrix_y, NUM_ROWS);

    // Multiply X and Y with different numbers of threads
    for (int i = 4; i <= 112; i += 4) {
        omp_set_num_threads(i);
        double start_time = omp_get_wtime();

        // Multiply the compressed matrices
        int** result = parallel_compressed_matrix_multiply(matrix_xb, matrix_xc, row_counts_x, 
                                                           matrix_yb, matrix_yc, row_counts_y, DEBUG);

        double end_time = omp_get_wtime();
        printf("Num Threads: %d, Time (s): %f\n", i, end_time - start_time);

        // Free the result matrix after each iteration
        free_2d_array(result, NUM_ROWS);
    }

    // Free compressed matrix_x
    free_2d_array(matrix_xb, NUM_ROWS);
    free_2d_array(matrix_xc, NUM_ROWS);
    free(row_counts_x);

    // Free compressed matrix_y
    free_2d_array(matrix_yb, NUM_ROWS);
    free_2d_array(matrix_yc, NUM_ROWS);
    free(row_counts_y);

    return 0;
}
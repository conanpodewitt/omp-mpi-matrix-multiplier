#include "utils.h"
#include "matrix_operations.h"
#include "matrix_generation.h"
#include <omp.h>

int** ordinary_matrix_multiply(int **A, int **B, int rows, int cols) {
    int **C = allocate_2d_array(rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            C[i][j] = 0;
            for (int k = 0; k < cols; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

int main() {
    srand(time(NULL));
    float p = 0.01;
    printf("Probability of non-zero elements: %f\n", p);

    int** matrix_x = generate_matrix(p);
    int** matrix_y = generate_matrix(p);

    char* run_dir = create_run_directory();
    char performance_file[100];
    sprintf(performance_file, "%s/Performance.csv", run_dir);

    // Perform ordinary matrix multiplication if matrix size <= 1000x1000
    if (NUM_ROWS <= 1000 && NUM_COLUMNS <= 1000) {
        char* ordinary_dir = create_thread_directory(run_dir, 0);  // 0 indicates ordinary multiplication

        double start_time = omp_get_wtime();
        int** ordinary_result = ordinary_matrix_multiply(matrix_x, matrix_y, NUM_ROWS, NUM_COLUMNS);
        double end_time = omp_get_wtime();
        printf("Ordinary multiplication time: %f\n", end_time - start_time);

        char filename[100];
        sprintf(filename, "%s/Result.csv", ordinary_dir);
        write_matrix_to_csv(filename, ordinary_result, NUM_ROWS, NUM_COLUMNS);

        // Write performance to the main performance file
        write_performance_to_csv(performance_file, 0, end_time - start_time, 0);  // 0 indicates first write

        // Create Dir_X and Dir_Y for ordinary multiplication
        char dir_x[100], dir_y[100];
        sprintf(dir_x, "%s/DirX", ordinary_dir);
        sprintf(dir_y, "%s/DirY", ordinary_dir);
        create_directory(dir_x);
        create_directory(dir_y);

        // Write original matrices to files
        sprintf(filename, "%s/FileX.csv", dir_x);
        write_matrix_to_csv(filename, matrix_x, NUM_ROWS, NUM_COLUMNS);
        sprintf(filename, "%s/FileY.csv", dir_y);
        write_matrix_to_csv(filename, matrix_y, NUM_ROWS, NUM_COLUMNS);

        free_2d_array(ordinary_result, NUM_ROWS);
        free(ordinary_dir);
    }

    // Compress matrices
    int** matrix_xb;
    int** matrix_xc;
    int* row_counts_x;
    compress_matrix(matrix_x, &matrix_xb, &matrix_xc, &row_counts_x);

    int** matrix_yb;
    int** matrix_yc;
    int* row_counts_y;
    compress_matrix(matrix_y, &matrix_yb, &matrix_yc, &row_counts_y);

    free_2d_array(matrix_x, NUM_ROWS);
    free_2d_array(matrix_y, NUM_ROWS);

    // Multiply with different thread counts
    for (int i = 4; i <= 112; i += 4) {
        char* thread_dir = create_thread_directory(run_dir, i);
        
        // Create Dir_X and Dir_Y
        char dir_x[100], dir_y[100];
        sprintf(dir_x, "%s/DirX", thread_dir);
        sprintf(dir_y, "%s/DirY", thread_dir);
        create_directory(dir_x);
        create_directory(dir_y);

        // Write compressed matrices to files
        char filename[100];
        sprintf(filename, "%s/FileB.csv", dir_x);
        write_compressed_matrix_to_csv(filename, matrix_xb, matrix_xc, row_counts_x, NUM_ROWS);
        sprintf(filename, "%s/FileC.csv", dir_x);
        write_compressed_matrix_to_csv(filename, matrix_xc, matrix_xb, row_counts_x, NUM_ROWS);

        sprintf(filename, "%s/FileB.csv", dir_y);
        write_compressed_matrix_to_csv(filename, matrix_yb, matrix_yc, row_counts_y, NUM_ROWS);
        sprintf(filename, "%s/FileC.csv", dir_y);
        write_compressed_matrix_to_csv(filename, matrix_yc, matrix_yb, row_counts_y, NUM_ROWS);

        omp_set_num_threads(i);
        double start_time = omp_get_wtime();

        int** result = parallel_compressed_matrix_multiply(matrix_xb, matrix_xc, row_counts_x, 
                                                           matrix_yb, matrix_yc, row_counts_y, DEBUG);

        double end_time = omp_get_wtime();
        printf("Num Threads: %d, Time (s): %f\n", i, end_time - start_time);

        // Write result to file
        sprintf(filename, "%s/Result.csv", thread_dir);
        write_matrix_to_csv(filename, result, NUM_ROWS, NUM_COLUMNS);

        // Append performance to the main performance file
        write_performance_to_csv(performance_file, i, end_time - start_time, 1);  // 1 indicates append

        free_2d_array(result, NUM_ROWS);
        free(thread_dir);
    }

    // Free compressed matrices
    free_2d_array(matrix_xb, NUM_ROWS);
    free_2d_array(matrix_xc, NUM_ROWS);
    free(row_counts_x);
    free_2d_array(matrix_yb, NUM_ROWS);
    free_2d_array(matrix_yc, NUM_ROWS);
    free(row_counts_y);

    free(run_dir);

    return 0;
}
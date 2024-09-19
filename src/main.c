#include "utils.h"
#include "matrix_operations.h"
#include "matrix_generation.h"
#include <omp.h>

int main() {
    // Seed the random number generator
    srand(time(NULL));

    printf("Scheduling mode: %d\n", SCHEDULE);
    printf("Chunk size: %d\n", CHUNK_SIZE);
    printf("Starting rows: %d\n", NUM_ROWS);
    printf("Starting cols: %d\n", NUM_COLUMNS);
    printf("Probability of non-zero elements: %f\n", PROB);

    // Generate unique run directory
    char *run_dir = generate_unique_run_directory();
    char dir_x[256], dir_y[256];
    snprintf(dir_x, sizeof(dir_x), "%s/DirX", run_dir);
    snprintf(dir_y, sizeof(dir_y), "%s/DirY", run_dir);
    create_directory(dir_x);
    create_directory(dir_y);

    // Generate matrix_x and matrix_y
    int** matrix_x = generate_matrix(PROB);
    int** matrix_y = generate_matrix(PROB);

    // Write original matrices to files
    char file_x[256], file_y[256];
    snprintf(file_x, sizeof(file_x), "%s/FileX.csv", dir_x);
    snprintf(file_y, sizeof(file_y), "%s/FileY.csv", dir_y);
    write_matrix_to_file(file_x, matrix_x, NUM_ROWS, NUM_COLUMNS);
    write_matrix_to_file(file_y, matrix_y, NUM_ROWS, NUM_COLUMNS);

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

    // Write compressed matrices to files
    char file_xb[256], file_xc[256], file_yb[256], file_yc[256];
    snprintf(file_xb, sizeof(file_xb), "%s/FileB.csv", dir_x);
    snprintf(file_xc, sizeof(file_xc), "%s/FileC.csv", dir_x);
    snprintf(file_yb, sizeof(file_yb), "%s/FileB.csv", dir_y);
    snprintf(file_yc, sizeof(file_yc), "%s/FileC.csv", dir_y);
    write_compressed_matrix_to_file(file_xb, matrix_xb, NUM_ROWS, NUM_COLUMNS);
    write_compressed_matrix_to_file(file_xc, matrix_xc, NUM_ROWS, NUM_COLUMNS);
    write_compressed_matrix_to_file(file_yb, matrix_yb, NUM_ROWS, NUM_COLUMNS);
    write_compressed_matrix_to_file(file_yc, matrix_yc, NUM_ROWS, NUM_COLUMNS);

    // Perform ordinary matrix multiplication if matrix size is less than 1000x1000
    if (NUM_ROWS < 1000 && NUM_COLUMNS < 1000) {
        double start_time = omp_get_wtime();
        int** ordinary_result = multiply_matrices(matrix_x, matrix_y);
        double end_time = omp_get_wtime();
        printf("Ordinary multiplication time: %f seconds\n", end_time - start_time);

        char ordinary_file[256];
        snprintf(ordinary_file, sizeof(ordinary_file), "%s/Ordinary.csv", run_dir);
        write_result_to_csv(ordinary_file, ordinary_result, NUM_ROWS, NUM_COLUMNS);

        free_2d_array(ordinary_result, NUM_ROWS);
    }

    // Free the original matrices (matrix_x and matrix_y) as they are no longer needed
    free_2d_array(matrix_x, NUM_ROWS);
    free_2d_array(matrix_y, NUM_ROWS);

    // Multiply X and Y with different numbers of threads
    double times[MAX_THREADS/4];
    int** first_result = NULL;
    for (int i = 4; i <= MAX_THREADS; i += 4) {
        omp_set_num_threads(i);
        double start_time = omp_get_wtime();

        // Multiply the compressed matrices
        int** result = parallel_compressed_matrix_multiply(matrix_xb, matrix_xc, row_counts_x, 
                                                           matrix_yb, matrix_yc, row_counts_y, DEBUG);

        double end_time = omp_get_wtime();
        times[(i/4)-1] = end_time - start_time;
        printf("Num Threads: %d, Time (s): %f\n", i, times[(i/4)-1]);

        // Save the first result
        if (i == 4) {
            first_result = result;
        } else {
            free_2d_array(result, NUM_ROWS);
        }
    }

    // Write the first result to CompressResults.csv
    char compress_file[256];
    snprintf(compress_file, sizeof(compress_file), "%s/CompressResults.csv", run_dir);
    write_result_to_csv(compress_file, first_result, NUM_ROWS, NUM_COLUMNS);

    // Write performance data to Performance.csv
    char performance_file[256];
    snprintf(performance_file, sizeof(performance_file), "%s/Performance.csv", run_dir);
    write_performance_to_csv(performance_file, SCHEDULE, CHUNK_SIZE, NUM_ROWS, NUM_COLUMNS, times, MAX_THREADS/4);

    // Free the first result matrix
    free_2d_array(first_result, NUM_ROWS);

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
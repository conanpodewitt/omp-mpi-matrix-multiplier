#include "utils.h"
#include "matrix_operations.h"
#include "matrix_generation.h"

int main(int argc, char **argv) {
    // Initialize MPI
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Seed the random number generator consistently across all processes
    unsigned int seed;
    if (rank == 0) {
        seed = (unsigned int)time(NULL);
    }
    MPI_Bcast(&seed, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
    srand(seed);

    // Print the configuration
    if (rank == 0) {
        printf("NumRows: \033[1;32m%d\033[0m\n", NUM_ROWS);
        printf("NumCols: \033[1;32m%d\033[0m\n", NUM_COLUMNS);
        printf("NonZero: \033[1;32m%f\033[0m\n", PROB);
        if (PARALLEL_METHOD != METHOD_MPI) {
            printf("Method: \033[1;32mOMP\033[0m\n");
            switch (SCHEDULE) {
                case 1:
                    printf("Schedule: \033[1;32mStatic\033[0m\n");
                    break;
                case 2:
                    printf("Schedule: \033[1;32mDynamic\033[0m\n");
                    break;
                case 3:
                    printf("Schedule: \033[1;32mGuided\033[0m\n");
                    break;
                case 4:
                    printf("Schedule: \033[1;32mAuto\033[0m\n");
                    break;
            }
            if (CHUNK_SIZE == 0) {
                printf("ChunkSize: \033[1;32mDefault\033[0m\n");
            } else {
                printf("ChunkSize: \033[1;32m%d\033[0m\n", CHUNK_SIZE);
            }
        }
        else {
            printf("Method: \033[1;32mMPI\033[0m\n");
        }
    }

    // Generate unique run directory (only on rank 0)
    char *run_dir = NULL;
    char dir_x[256], dir_y[256];
    if (rank == 0) {
        run_dir = create_run_directory();
        snprintf(dir_x, sizeof(dir_x), "%s/DirX", run_dir);
        snprintf(dir_y, sizeof(dir_y), "%s/DirY", run_dir);
        create_directory(dir_x);
        create_directory(dir_y);
    }

    // Generate matrix_x and matrix_y
    int** matrix_x = generate_matrix(PROB);
    int** matrix_y = generate_matrix(PROB);

    // Write original matrices to files
    char file_x[256], file_y[256];
    if (rank == 0) {
        snprintf(file_x, sizeof(file_x), "%s/FileX.csv", dir_x);
        snprintf(file_y, sizeof(file_y), "%s/FileY.csv", dir_y);
        write_matrix_to_file(file_x, matrix_x, NUM_ROWS, NUM_COLUMNS);
        write_matrix_to_file(file_y, matrix_y, NUM_ROWS, NUM_COLUMNS);
    }

    // Perform ordinary matrix multiplication if matrix size is less than 1000x1000
    if (rank == 0) {
        if (NUM_ROWS < 1000 && NUM_COLUMNS < 1000) {
            double start_time = omp_get_wtime();
            int** ordinary_result = sequential_matrix_multiply(matrix_x, matrix_y);
            double end_time = omp_get_wtime();
            printf("Ordinary mult time: \033[1;32m%f\033[0m\n", end_time - start_time);

            char ordinary_file[256];
            snprintf(ordinary_file, sizeof(ordinary_file), "%s/SequentialResults.csv", run_dir);
            write_result_to_file(ordinary_file, ordinary_result, NUM_ROWS, NUM_COLUMNS);

            free_2d_array(ordinary_result, NUM_ROWS);
        }
    }

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

    // Write compressed matrices to files
    if (rank == 0) {
        char file_xb[256], file_xc[256], file_yb[256], file_yc[256];
        snprintf(file_xb, sizeof(file_xb), "%s/FileB.csv", dir_x);
        snprintf(file_xc, sizeof(file_xc), "%s/FileC.csv", dir_x);
        snprintf(file_yb, sizeof(file_yb), "%s/FileB.csv", dir_y);
        snprintf(file_yc, sizeof(file_yc), "%s/FileC.csv", dir_y);
        write_compressed_matrix_to_file(file_xb, matrix_xb, NUM_ROWS, NUM_COLUMNS);
        write_compressed_matrix_to_file(file_xc, matrix_xc, NUM_ROWS, NUM_COLUMNS);
        write_compressed_matrix_to_file(file_yb, matrix_yb, NUM_ROWS, NUM_COLUMNS);
        write_compressed_matrix_to_file(file_yc, matrix_yc, NUM_ROWS, NUM_COLUMNS);
    }

    // Perform matrix multiplication based on the chosen method
    double times[MAX_THREADS/4];
    int** first_result = NULL;

    for (int i = 4; i <= MAX_THREADS; i += 4) {
        double start_time, end_time;
        int** result = NULL;

        switch (PARALLEL_METHOD) {
            case METHOD_OMP:
                omp_set_num_threads(i);
                start_time = omp_get_wtime();
                result = omp_compressed_matrix_multiply(matrix_xb, matrix_xc, row_counts_x, 
                                                        matrix_yb, matrix_yc, row_counts_y);
                end_time = omp_get_wtime();
                break;
            case METHOD_MPI:
                start_time = MPI_Wtime();
                result = mpi_compressed_matrix_multiply(matrix_xb, matrix_xc, row_counts_x, 
                                                        matrix_yb, matrix_yc, row_counts_y);
                end_time = MPI_Wtime();
                break;
        }

        times[(i/4)-1] = end_time - start_time;

        if (rank == 0) {  // Only the root process prints and saves results
            printf("Threads: \033[1;32m%d\033[0m, Time: \033[1;32m%f\033[0m\n", i, times[(i/4)-1]);

            // Save the first result
            if (i == 4) {
                first_result = result;
            } else {
                free_2d_array(result, NUM_ROWS);
            }
        }
    }

    if (rank == 0) {  // Only the root process writes results
        // Write the first result to ParallelResults.csv
        char compress_file[256];
        snprintf(compress_file, sizeof(compress_file), "%s/ParallelResults.csv", run_dir);
        write_result_to_file(compress_file, first_result, NUM_ROWS, NUM_COLUMNS);

        // Write performance data to Performance.csv
        char performance_file[256];
        snprintf(performance_file, sizeof(performance_file), "%s/Performance.csv", run_dir);
        write_performance_to_file(performance_file, NUM_ROWS, NUM_COLUMNS, PROB, PARALLEL_METHOD, SCHEDULE, CHUNK_SIZE, times, MAX_THREADS/4);

        // Free the first result matrix
        free_2d_array(first_result, NUM_ROWS);
    }

    // Free compressed matrix_x
    free_2d_array(matrix_xb, NUM_ROWS);
    free_2d_array(matrix_xc, NUM_ROWS);
    free(row_counts_x);

    // Free compressed matrix_y
    free_2d_array(matrix_yb, NUM_ROWS);
    free_2d_array(matrix_yc, NUM_ROWS);
    free(row_counts_y);

    // Finalize parallelization
    finalize_parallelization();
    
    return 0;
}
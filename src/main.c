#include "utils.h"
#include "matrix_operations.h"
#include "matrix_generation.h"

int main(int argc, char **argv) {
    // Initialize parallelization
    initialize_parallelization(argc, argv);

    int rank = 0, size = 1;
    if (PARALLEL_METHOD == METHOD_MPI) {
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
    }

    // Seed the random number generator
    unsigned int seed = (unsigned int)time(NULL);
    if (PARALLEL_METHOD == METHOD_MPI) {
        MPI_Bcast(&seed, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
    }
    srand(seed);

    // Print the configuration
    if (rank == 0) {
        printf("NumRows: \033[1;32m%d\033[0m\n", NUM_ROWS);
        printf("NumCols: \033[1;32m%d\033[0m\n", NUM_COLUMNS);
        printf("NonZero: \033[1;32m%f\033[0m\n", PROB);
        printf("Method: \033[1;32m%s\033[0m\n", PARALLEL_METHOD == METHOD_OMP ? "OMP" : "MPI");
        if (PARALLEL_METHOD == METHOD_OMP) {
            printf("Schedule: \033[1;32m%s\033[0m\n", 
                SCHEDULE == SCHEDULE_STATIC ? "Static" :
                SCHEDULE == SCHEDULE_DYNAMIC ? "Dynamic" :
                SCHEDULE == SCHEDULE_GUIDED ? "Guided" : "Auto");
            if (CHUNK_SIZE == 0) {
                printf("ChunkSize: \033[1;32mDefault\033[0m\n");
            } else {
                printf("ChunkSize: \033[1;32m%d\033[0m\n", CHUNK_SIZE);
            }
        }
    }

    // Generate unique run directory
    char *run_dir = NULL;
    if (rank == 0) {
        run_dir = create_run_directory();
    }

    // Generate and compress matrices
    int** matrix_xb = NULL;
    int** matrix_xc = NULL;
    int* row_counts_x = NULL;
    int** matrix_yb = NULL;
    int** matrix_yc = NULL;
    int* row_counts_y = NULL;

    // Only root process generates the initial matrices
    if (rank == 0 || PARALLEL_METHOD == METHOD_OMP) {
        int** matrix_x = generate_matrix(PROB);
        int** matrix_y = generate_matrix(PROB);

        if (rank == 0) {
            // Write original matrices to files
            char file_x[256], file_y[256];
            snprintf(file_x, sizeof(file_x), "%s/FileX.csv", run_dir);
            snprintf(file_y, sizeof(file_y), "%s/FileY.csv", run_dir);
            write_matrix_to_file(file_x, matrix_x, NUM_ROWS, NUM_COLUMNS);
            write_matrix_to_file(file_y, matrix_y, NUM_ROWS, NUM_COLUMNS);

            // Perform sequential multiplication for small matrices
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

        // Compress matrices
        compress_matrix(matrix_x, &matrix_xb, &matrix_xc, &row_counts_x);
        compress_matrix(matrix_y, &matrix_yb, &matrix_yc, &row_counts_y);

        // Free original matrices
        free_2d_array(matrix_x, NUM_ROWS);
        free_2d_array(matrix_y, NUM_ROWS);

        if (rank == 0) {
            // Write compressed matrices to files
            char file_xb[256], file_xc[256], file_yb[256], file_yc[256];
            snprintf(file_xb, sizeof(file_xb), "%s/FileXB.csv", run_dir);
            snprintf(file_xc, sizeof(file_xc), "%s/FileXC.csv", run_dir);
            snprintf(file_yb, sizeof(file_yb), "%s/FileYB.csv", run_dir);
            snprintf(file_yc, sizeof(file_yc), "%s/FileYC.csv", run_dir);
            write_compressed_matrix_to_file(file_xb, matrix_xb, NUM_ROWS, NUM_COLUMNS);
            write_compressed_matrix_to_file(file_xc, matrix_xc, NUM_ROWS, NUM_COLUMNS);
            write_compressed_matrix_to_file(file_yb, matrix_yb, NUM_ROWS, NUM_COLUMNS);
            write_compressed_matrix_to_file(file_yc, matrix_yc, NUM_ROWS, NUM_COLUMNS);
        }
    }

    // MPI specific setup and broadcast
    if (PARALLEL_METHOD == METHOD_MPI) {
        // Broadcast row_counts
        if (rank != 0) {
            row_counts_x = (int*)malloc(NUM_ROWS * sizeof(int));
            row_counts_y = (int*)malloc(NUM_ROWS * sizeof(int));
            if (!row_counts_x || !row_counts_y) {
                fprintf(stderr, "Failed to allocate row counts memory\n");
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
        MPI_Bcast(row_counts_x, NUM_ROWS, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(row_counts_y, NUM_ROWS, MPI_INT, 0, MPI_COMM_WORLD);

        // Allocate memory for non-root processes
        if (rank != 0) {
            // Allocate array of pointers
            matrix_xb = (int**)malloc(NUM_ROWS * sizeof(int*));
            matrix_xc = (int**)malloc(NUM_ROWS * sizeof(int*));
            matrix_yb = (int**)malloc(NUM_ROWS * sizeof(int*));
            matrix_yc = (int**)malloc(NUM_ROWS * sizeof(int*));
            
            if (!matrix_xb || !matrix_xc || !matrix_yb || !matrix_yc) {
                fprintf(stderr, "Failed to allocate matrix pointer memory\n");
                MPI_Abort(MPI_COMM_WORLD, 1);
            }

            // Allocate each row
            for (int i = 0; i < NUM_ROWS; i++) {
                matrix_xb[i] = (int*)malloc(row_counts_x[i] * sizeof(int));
                matrix_xc[i] = (int*)malloc(row_counts_x[i] * sizeof(int));
                matrix_yb[i] = (int*)malloc(row_counts_y[i] * sizeof(int));
                matrix_yc[i] = (int*)malloc(row_counts_y[i] * sizeof(int));
                
                if (!matrix_xb[i] || !matrix_xc[i] || !matrix_yb[i] || !matrix_yc[i]) {
                    fprintf(stderr, "Failed to allocate matrix row memory\n");
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
            }
        }

        // Broadcast matrix data
        for (int i = 0; i < NUM_ROWS; i++) {
            MPI_Bcast(matrix_xb[i], row_counts_x[i], MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(matrix_xc[i], row_counts_x[i], MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(matrix_yb[i], row_counts_y[i], MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(matrix_yc[i], row_counts_y[i], MPI_INT, 0, MPI_COMM_WORLD);
        }
    }

    // Perform matrix multiplication
    double times[MAX_THREADS/4];
    int** first_result = NULL;

    for (int i = 4; i <= MAX_THREADS; i += 4) {
        double start_time, end_time;
        int** result = NULL;

        if (PARALLEL_METHOD == METHOD_OMP) {
            omp_set_num_threads(i);
            start_time = omp_get_wtime();
            result = omp_compressed_matrix_multiply(matrix_xb, matrix_xc, row_counts_x, 
                                                    matrix_yb, matrix_yc, row_counts_y);
            end_time = omp_get_wtime();
        } else if (PARALLEL_METHOD == METHOD_MPI) {
            MPI_Barrier(MPI_COMM_WORLD);
            start_time = MPI_Wtime();
            result = mpi_compressed_matrix_multiply(matrix_xb, matrix_xc, row_counts_x, 
                                                    matrix_yb, matrix_yc, row_counts_y);
            MPI_Barrier(MPI_COMM_WORLD);
            end_time = MPI_Wtime();
        }

        if (rank == 0) {
            times[(i/4)-1] = end_time - start_time;
            printf("Threads: \033[1;32m%d\033[0m, Time: \033[1;32m%f\033[0m\n", i, times[(i/4)-1]);

            if (i == 4) {
                first_result = result;
            } else if (result != NULL) {
                free_2d_array(result, NUM_ROWS);
            }
        }
    }

    // Write results to files (root process only)
    if (rank == 0) {
        char compress_file[256], performance_file[256];
        snprintf(compress_file, sizeof(compress_file), "%s/ParallelResults.csv", run_dir);
        snprintf(performance_file, sizeof(performance_file), "%s/Performance.csv", run_dir);
        
        write_result_to_file(compress_file, first_result, NUM_ROWS, NUM_COLUMNS);
        write_performance_to_file(performance_file, NUM_ROWS, NUM_COLUMNS, PROB, 
                                PARALLEL_METHOD, SCHEDULE, CHUNK_SIZE, times, MAX_THREADS/4);

        free_2d_array(first_result, NUM_ROWS);
    }

    // Clean up memory
    if (PARALLEL_METHOD == METHOD_MPI && rank != 0) {
        // Clean up non-root process memory
        for (int i = 0; i < NUM_ROWS; i++) {
            free(matrix_xb[i]);
            free(matrix_xc[i]);
            free(matrix_yb[i]);
            free(matrix_yc[i]);
        }
        free(matrix_xb);
        free(matrix_xc);
        free(matrix_yb);
        free(matrix_yc);
        free(row_counts_x);
        free(row_counts_y);
    } else if (rank == 0 || PARALLEL_METHOD == METHOD_OMP) {
        // Clean up root process memory
        free_2d_array(matrix_xb, NUM_ROWS);
        free_2d_array(matrix_xc, NUM_ROWS);
        free(row_counts_x);
        free_2d_array(matrix_yb, NUM_ROWS);
        free_2d_array(matrix_yc, NUM_ROWS);
        free(row_counts_y);
    }

    // Finalize parallelization
    finalize_parallelization();
    
    return 0;
}
#include "matrix_operations.h"

int** sequential_matrix_multiply(int** matrix1, int** matrix2) {
    int** result = allocate_2d_array(NUM_ROWS, NUM_COLUMNS);

    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_COLUMNS; j++) {
            result[i][j] = 0;
            for (int k = 0; k < NUM_COLUMNS; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    return result;
}

int** omp_compressed_matrix_multiply(int** matrix_xb, int** matrix_xc, int* row_counts_x, 
                                     int** matrix_yb, int** matrix_yc, int* row_counts_y) {
    // Allocate memory for the result matrix
    int** result = allocate_2d_array(NUM_ROWS, NUM_COLUMNS);

    // Initialize scheduling type
    switch (SCHEDULE) {
        case 1: // Static
            if (CHUNK_SIZE > 0) {
                omp_set_schedule(omp_sched_static, CHUNK_SIZE);
            } else {
                omp_set_schedule(omp_sched_static, 1);  // Default chunk size
            }
            break;
        case 2: // Dynamic
            if (CHUNK_SIZE > 0) {
                omp_set_schedule(omp_sched_dynamic, CHUNK_SIZE);
            } else {
                omp_set_schedule(omp_sched_dynamic, 1);  // Default chunk size
            }
            break;
        case 3: // Guided
            if (CHUNK_SIZE > 0) {
                omp_set_schedule(omp_sched_guided, CHUNK_SIZE);
            } else {
                omp_set_schedule(omp_sched_guided, 1);  // Default chunk size
            }
            break;
        case 4: // Auto (no chunk size needed)
            omp_set_schedule(omp_sched_auto, 0);  // Chunk size is ignored for auto
            break;
        default: // Default to static
            if (CHUNK_SIZE > 0) {
                omp_set_schedule(omp_sched_static, CHUNK_SIZE);
            } else {
                omp_set_schedule(omp_sched_static, 1);  // Default chunk size
            }
            break;
    }

    // Parallelize the outermost loop
    #pragma omp parallel for schedule(runtime)
    for (int i = 0; i < NUM_ROWS; i++) {
        // For each non-zero element in row i of matrix_xb
        for (int x_idx = 0; x_idx < row_counts_x[i]; x_idx++) {
            int x_value = matrix_xb[i][x_idx];     // Non-zero element in matrix X
            int x_col = matrix_xc[i][x_idx];       // Corresponding column index in X
            // Multiply with non-zero elements in row x_col of matrix Y
            for (int y_idx = 0; y_idx < row_counts_y[x_col]; y_idx++) {
                int y_value = matrix_yb[x_col][y_idx]; // Non-zero element in matrix Y
                int y_col = matrix_yc[x_col][y_idx];   // Corresponding column index in Y
                // Accumulate the result in the appropriate position
                #pragma omp atomic // Ensure atomic addition to avoid race conditions
                result[i][y_col] += x_value * y_value;
            }
        }
    }
    return result;
}

int** mpi_compressed_matrix_multiply(int** matrix_xb, int** matrix_xc, int* row_counts_x, 
                                     int** matrix_yb, int** matrix_yc, int* row_counts_y) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Calculate local workload
    int rows_per_process = NUM_ROWS / size;
    int start_row = rank * rows_per_process;
    int end_row = (rank == size - 1) ? NUM_ROWS : start_row + rows_per_process;
    int local_rows = end_row - start_row;

    // Allocate memory only for the local portion of the result
    int** local_result = allocate_2d_array(local_rows, NUM_COLUMNS);

    // Perform local matrix multiplication
    for (int i = 0; i < local_rows; i++) {
        int global_row = start_row + i;
        for (int x_idx = 0; x_idx < row_counts_x[global_row]; x_idx++) {
            int x_value = matrix_xb[global_row][x_idx];
            int x_col = matrix_xc[global_row][x_idx];
            for (int y_idx = 0; y_idx < row_counts_y[x_col]; y_idx++) {
                int y_value = matrix_yb[x_col][y_idx];
                int y_col = matrix_yc[x_col][y_idx];
                local_result[i][y_col] += x_value * y_value;
            }
        }
    }

    // Gather results using MPI_Gatherv for flexibility with non-uniform distributions
    int* recvcounts = NULL;
    int* displs = NULL;
    int** result = NULL;

    if (rank == 0) {
        result = allocate_2d_array(NUM_ROWS, NUM_COLUMNS);
        recvcounts = (int*)malloc(size * sizeof(int));
        displs = (int*)malloc(size * sizeof(int));

        for (int i = 0; i < size; i++) {
            recvcounts[i] = (i == size - 1) ? (NUM_ROWS - i * rows_per_process) * NUM_COLUMNS 
                                            : rows_per_process * NUM_COLUMNS;
            displs[i] = i * rows_per_process * NUM_COLUMNS;
        }
    }

    MPI_Gatherv(local_result[0], local_rows * NUM_COLUMNS, MPI_INT,
                result ? result[0] : NULL, recvcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    // Free local memory
    free_2d_array(local_result, local_rows);
    if (rank == 0) {
        free(recvcounts);
        free(displs);
    }

    return result;
}

void compress_matrix(int** matrix, int*** compressed_values, int*** compressed_cols, int** row_counts) {
    *compressed_values = allocate_2d_array(NUM_ROWS, NUM_COLUMNS);
    *compressed_cols = allocate_2d_array(NUM_ROWS, NUM_COLUMNS);
    *row_counts = (int*)calloc(NUM_ROWS, sizeof(int));

    for (int i = 0; i < NUM_ROWS; i++) {
        int count = 0;
        for (int j = 0; j < NUM_COLUMNS; j++) {
            if (matrix[i][j] != 0) {
                (*compressed_values)[i][count] = matrix[i][j];
                (*compressed_cols)[i][count] = j;
                count++;
            }
        }
        (*row_counts)[i] = count;
    }
}
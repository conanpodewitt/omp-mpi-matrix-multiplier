#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

// Define matrix size
#define NUM_ROWS 100
#define NUM_COLUMNS 100
// Set the probability of non-zero elements
#define PROB 0.01
// Select maximum number of threads
#define MAX_THREADS 128
// Choose a scheduling type: 1 for static, 2 for dynamic, 3 for guided, 4 for auto
#define SCHEDULE 1
// Select chunk size for scheduling. Use 0 for default chunk size in static, dynamic, and guided schedules (ignored in auto)
#define CHUNK_SIZE 0
// Set as 0 or 1 to enable debugging, prints result matrix to terminal
#define DEBUG 0

// Function prototypes
int** allocate_2d_array(int rows, int cols);
void free_2d_array(int **arr, int rows);
double omp_get_wtime();

void create_directory(const char *path);
void write_matrix_to_file(const char *filename, int **matrix, int rows, int cols);
void write_compressed_matrix_to_file(const char *filename, int **matrix, int rows, int cols);
void write_performance_to_csv(const char *filename, int schedule, int chunk_size, int num_rows, int num_cols, double *times, int num_threads);
void write_result_to_csv(const char *filename, int **matrix, int rows, int cols);
char* generate_unique_run_directory();

#endif // UTILS_H
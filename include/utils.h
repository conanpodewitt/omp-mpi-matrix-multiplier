#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

// Define matrix size
#define NUM_ROWS 1000
#define NUM_COLUMNS 1000
// Choose a scheduling type: 1 for static, 2 for dynamic, 3 for guided
#define SCHEDULE 2
#define CHUNK_SIZE 10
// Set as 0 or 1 to enable debugging, prints result matrix to terminal
#define DEBUG 0

// Function prototypes
int** allocate_2d_array(int rows, int cols);
void free_2d_array(int **arr, int rows);
double omp_get_wtime();

void create_directory(const char *path);
void write_matrix_to_csv(const char *filename, int **matrix, int rows, int cols);
void write_compressed_matrix_to_csv(const char *filename, int **values, int **cols, int *row_counts, int rows);
void write_performance_to_csv(const char *filename, int num_threads, double time, int append);
char* create_run_directory();
char* create_thread_directory(const char* run_dir, int thread_count);

#endif // UTILS_H
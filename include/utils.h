#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ftw.h>
#include <omp.h>
#include <mpi.h>

// Matrix generation parameters
// Define matrix size
#define NUM_ROWS 100
#define NUM_COLUMNS 100
// Set the probability of non-zero elements
#define PROB 0.01

// Select the parallelization method
#define PARALLEL_METHOD METHOD_OMP

// Enable file generation
#define GENERATE_FILES 1

// OMP Performance parameters
// Select maximum number of threads
#define MAX_THREADS 128
// Select a scheduling type:
#define SCHEDULE SCHEDULE_STATIC
// Select chunk size for scheduling: Use 0 for default chunk size in static, dynamic, and guided schedules (ignored in auto)
#define CHUNK_SIZE 0

// Don't change these
#define METHOD_OMP 1
#define METHOD_MPI 2
#define SCHEDULE_STATIC 1
#define SCHEDULE_DYNAMIC 2
#define SCHEDULE_GUIDED 3
#define SCHEDULE_AUTO 4

// Function prototypes
void initialize_parallelization(int argc, char **argv);
void finalize_parallelization();

int** allocate_2d_array(int rows, int cols);
void free_2d_array(int **arr, int rows);

char* create_run_directory(int gen_files);
void create_directory(const char *path);
void delete_directory(const char *path);
void delete_temp();

void write_matrix_to_file(const char *filename, int **matrix, int rows, int cols);
void write_compressed_matrix_to_file(const char *filename, int **matrix, int rows, int cols);
void write_performance_to_file(const char *filename, int num_rows, int num_cols, float prob, int method, int schedule, int chunk_size, double *times, int num_threads);

void check_mpi_error(int error_code, const char* message);

#endif // UTILS_H
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// Define matrix size
#define NUM_ROWS 100
#define NUM_COLUMNS 100
// Choose a scheduling type: 1 for static, 2 for dynamic, 3 for guided
#define SCHEDULE 2
#define CHUNK_SIZE 10
// Set as 0 or 1 to enable debugging, prints result matrix to terminal
#define DEBUG 0

// Function prototypes
int** allocate_2d_array(int rows, int cols);
void free_2d_array(int **arr, int rows);
double omp_get_wtime();

#endif // UTILS_H
#include "matrix_generation.h"

int** generate_matrix(float non_zero_prob) {
    int** matrix = allocate_2d_array(NUM_ROWS, NUM_COLUMNS);
    
    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_COLUMNS; j++) {
            if ((float)rand() / RAND_MAX < non_zero_prob) {
                matrix[i][j] = rand() % 100 + 1;  // Random non-zero value between 1 and 100
            }
        }
    }
    
    return matrix;
}
# omp-matrix-multiplication
This is a C based project that has been designed to test different OpenMP scheduling methods and how different thread count impacts its ability perform matrix multiplication upon two randomly generated, sparse matrices. The matrices are initialised with a given probability of non-zero elements and then repeatedly multiplied through the use of row compression with an increasing amount of threads each time.

![Matrix](https://upload.wikimedia.org/wikipedia/commons/thumb/1/18/Matrix_multiplication_qtl1.svg/2880px-Matrix_multiplication_qtl1.svg.png)

## Features
- **Row and Column Initialisation:** The program allows users to specify the number of rows; `NUM_ROWS`, and columns; `NUM_COLS`, for the two input matrices (Matrix X and Matrix Y).
- **Matrix Initialisation:** These matrices are initialised with random values. A probability parameter, `PROB`, controls their sparsity.
- **Result Directory:** The program produces a unique directory; `RunXXX`, after each run. Within are two nested directories, `DirX` and `DirY`, each containing all information pertaining to the input matrices, whilst a row compresed matrix is presented as csv file; `CompressedResults.csv`. The overall performance of the run is also presented within a file named `Performance.csv`. 

## Requirements
- Standard C compiler
- OpenMP library
- Super Computer for matrix sizes above 10,000 by 10,000

## Usage
### Running Locally
1. **Compiling:** Within the root directory, run:
   ```sh
   make
   ```
2. **Running:** Within the root directory, run:
   ```sh
   bin/sparse_matrix_mult
   ```
3. **Cleaning:** Within the root directory, run:
   ```sh
   make clear
   ```

### Running Remotely (i.e., batched jobs)
1. **Running:** Within the root directory, run:
   ```sh
   sbatch run.sh
   ```
   This shell script automates the above, and also allows some tuning in terms of cores, threads, and memory.

### Adjusting Parameters
All starting parameters can be adjusted through the `#define` definitions at the top of `utils.h`:
```c
// utils.h
...

// Define matrix size
#define NUM_ROWS 1000
#define NUM_COLUMNS 1000
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

...
```

### Generating the B and C Matrices
The function responsible for generating the B and C matrices is called `compress_matrix()` can be found within the file `matrix_operations.c`:
```c
// matrix_operations.c
...

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

...
```

### Ordinary Matrix Multiplication
This project also includes a function named `multiply_matrices()` that multiplies ordinary matrices by the standard method. However, this function is only called upon if the specified input matrices are equal or less than 1,000 by 1,000. The results from the ordinary multiplication are returned to the output directory, and as such can be verified alongside the sparse multiplication results.
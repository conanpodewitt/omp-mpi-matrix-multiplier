# omp-mpi-matrix-multiplier

**Conan Dewitt (22877792) [GitHub](https://github.com/conanpodewitt)**

This is a C-based project designed to test different processing methods and how they impact the performance of matrix multiplication on two randomly generated, sparse matrices. The matrices are initialised with a given probability of non-zero elements and then repeatedly multiplied using row compression with an increasing number of threads each time.

## Features

- **Row and Column Initialisation:** The program allows users to specify the number of rows (`NUM_ROWS`) and columns (`NUM_COLUMNS`) for the two input matrices (Matrix X and Matrix Y).
- **Matrix Initialisation:** These matrices are initialized with random values. A probability parameter (`PROB`) controls their sparsity.
- **Execution Modes:** The program can be run in three different modes: Sequential, OpenMP, and MPI. The mode can be selected by setting the (`EXECUTION_MODE`).
  - Note that sequential processing is always executed beforehand if the matrix size is below 1000 by 1000, otherwise it is skipped.

## Requirements

- Standard C compiler
- OpenMP library
- MPI library
- Supercomputer for matrix sizes above 10,000 by 10,000

## Usage

### Running Locally

1. **Compiling:** Within the root directory, run:
   ```sh
   make
   ```
2. **Running:** Within the root directory, run:
   ```sh
   ./bin/sparse_matrix_mult
   ```
3. **Cleaning:** Within the root directory, run:
   ```sh
   make clean
   ```

### Adjusting Parameters

All starting parameters can be adjusted through the `#define` definitions at the top of [`include/utils.h`](include/utils.h):

```c
// utils.h
...
// Matrix generation parameters
// Define matrix size
#define NUM_ROWS 100
#define NUM_COLUMNS 100
// Set the probability of non-zero elements
#define PROB 0.01

// Select the parallelization method
#define PARALLEL_METHOD METHOD_OMP

// Enable file generation
#define GENERATE_FILES 0

// OMP Performance parameters
// Select maximum number of threads
#define MAX_THREADS 128
// Select a scheduling type:
#define SCHEDULE SCHEDULE_STATIC
// Select chunk size for scheduling: Use 0 for default chunk size in static, dynamic, and guided schedules (ignored in auto)
#define CHUNK_SIZE 0
...
```

## Code Structure

- **`src/`**: Contains the source code files.
  - [`main.c`](src/main.c): The main entry point of the program.
  - [`matrix_generation.c`](src/matrix_generation.c): Functions for generating matrices.
  - [`matrix_operations.c`](src/matrix_operations.c): Functions for matrix operations.
  - [`utils.c`](src/utils.c): Utility functions for parallelization, file operations, and memory management.
- **`include/`**: Contains the header files.
  - [`matrix_generation.h`](include/matrix_generation.h): Header file for matrix generation functions.
  - [`matrix_operations.h`](include/matrix_operations.h): Header file for matrix operations functions.
  - [`utils.h`](include/utils.h): Header file for utility functions.
- **`bin/`**: Contains the compiled binary.
- **`obj/`**: Contains the object files generated during compilation.
- **`.vscode/`**: Contains Visual Studio Code configuration files.
- **`Makefile`**: The makefile for building the project.
- **`README.md`**: This file.

## Functions

### Utility Functions

- [`initialize_parallelization`](src/utils.c): Initializes parallelization based on the selected method (OpenMP or MPI).
- [`finalize_parallelization`](src/utils.c): Finalizes parallelization.
- [`allocate_2d_array`](src/utils.c): Allocates a 2D array.
- [`free_2d_array`](src/utils.c): Frees a 2D array.
- [`create_run_directory`](src/utils.c): Creates a unique run directory.
- [`create_directory`](src/utils.c): Creates a directory.
- [`delete_directory`](src/utils.c): Deletes a directory.
- [`delete_temp`](src/utils.c): Deletes the temporary directory.
- [`write_matrix_to_file`](src/utils.c): Writes a matrix to a file.
- [`write_compressed_matrix_to_file`](src/utils.c): Writes a compressed matrix to a file.
- [`write_performance_to_file`](src/utils.c): Writes performance data to a file.
- [`check_mpi_error`](src/utils.c): Checks for MPI errors.

### Matrix Generation Functions

- [`generate_matrix`](src/matrix_generation.c): Generates a sparse matrix with a given probability of non-zero elements.

### Matrix Operations Functions

- [`sequential_matrix_multiply`](src/matrix_operations.c): Performs sequential matrix multiplication.
- [`omp_compressed_matrix_multiply`](src/matrix_operations.c): Performs matrix multiplication using OpenMP.
- [`mpi_compressed_matrix_multiply`](src/matrix_operations.c): Performs matrix multiplication using MPI.

## Example Output

The program prints the configuration and performance metrics to the console and writes the results to files in the run directory. Example output includes:

- Number of rows and columns
- Probability of non-zero elements
- Parallelization method (OMP or MPI)
- Scheduling type and chunk size (for OMP)
- Execution time for each thread count

```sh
NumRows: 100
NumCols: 100
NonZero: 0.010000
Method: OMP
Schedule: Static
ChunkSize: Default
Ordinary mult time: 0.001259
Threads: 4, Time: 0.000325
Threads: 8, Time: 0.000404
Threads: 12, Time: 0.000288
Threads: 16, Time: 0.000319
Threads: 20, Time: 0.000348
Threads: 24, Time: 0.005448
Threads: 28, Time: 0.000653
Threads: 32, Time: 0.000334
Threads: 36, Time: 0.000340
Threads: 40, Time: 0.000352
Threads: 44, Time: 0.000428
Threads: 48, Time: 0.000718
Threads: 52, Time: 0.000477
Threads: 56, Time: 0.000792
Threads: 60, Time: 0.000454
Threads: 64, Time: 0.000473
Threads: 68, Time: 0.000477
Threads: 72, Time: 0.000452
Threads: 76, Time: 0.000443
Threads: 80, Time: 0.000356
Threads: 84, Time: 0.000483
Threads: 88, Time: 0.000462
Threads: 92, Time: 0.000406
Threads: 96, Time: 0.000707
Threads: 100, Time: 0.000543
Threads: 104, Time: 0.000569
Threads: 108, Time: 0.000607
Threads: 112, Time: 0.000629
Threads: 116, Time: 0.000553
Threads: 120, Time: 0.000680
Threads: 124, Time: 0.000395
Threads: 128, Time: 0.000461
```
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
#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks-per-node=28
#SBATCH --partition=work
#SBATCH --account=courses0101
#SBATCH --mem=64
#SBATCH --time=00:10:00

make clean
make

srun bin/sparse_matrix_multi
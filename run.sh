#!/bin/bash

#SBATCH --job-name=matrix_multiply              # Job name
#SBATCH --output=matrix_multiply_%j.out         # Standard output log
#SBATCH --error=matrix_multiply_%j.err          # Error log
#SBATCH --ntasks=1                              # Single task
#SBATCH --cpus-per-task=28                      # 28 CPU cores
#SBATCH --time=02:00:00                         # Time limit
#SBATCH --mem=200G                              # Memory allocation
#SBATCH --partition=work                        # Partition name (adjust to match your system)
#SBATCH --exclusive                             # Ensure exclusive access to the node
#SBATCH --mail-type=ALL                         # Send email notifications on job start, finish, fail
#SBATCH --mail-user=22877792@student.uwa.edu.au

# Load necessary modules
module load gcc
module load openmp

# Set the number of OpenMP threads to use
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

# Start timer
start_time=$(date +%s)

# Build the program using the provided Makefile
make clear
make all

# Check if the executable exists and is built successfully
if [ ! -f ./bin/sparse_matrix_mult ]; then
    echo "Executable not found. Compilation failed."
    exit 1
fi

# Run the matrix multiplication program
./bin/sparse_matrix_mult

# End timer and calculate runtime
end_time=$(date +%s)
runtime=$((end_time - start_time))

echo "Matrix multiplication completed in $runtime seconds."
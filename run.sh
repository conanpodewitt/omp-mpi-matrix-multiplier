#!/bin/bash

#SBATCH --job-name=matrix_multiply            # Job name
#SBATCH --output=matrix_multiply_%j.out       # Standard output log (%j = Job ID)
#SBATCH --error=matrix_multiply_%j.err        # Error log (%j = Job ID)
#SBATCH --ntasks=1                            # Single task (1 node)
#SBATCH --cpus-per-task=32                    # 32 CPU cores (adjust as needed)
#SBATCH --time=00:58:00                       # Time limit (40 minutes)
#SBATCH --mem=200G                            # Memory allocation (250 GB, adjust based on matrix size)
#SBATCH --partition=work                 # Partition name (adjust to match your system)
#SBATCH --exclusive                           # Ensure exclusive access to the node
#SBATCH --mail-type=ALL                       # Send email notifications on job start, finish, fail
#SBATCH --mail-user=your-email@example.com    # Replace with your email

# Load necessary modules
module load gcc/10.2.0          # Adjust based on the available version of GCC
module load openmp              # Load OpenMP if necessary for your system

# Set the number of OpenMP threads to use
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

# Start timer
start_time=$(date +%s)

# Build the program using the provided Makefile
make clean
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
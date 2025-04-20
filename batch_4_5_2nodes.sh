#!/bin/bash
#SBATCH --time=00:20:00
#SBATCH --nodes=2
#SBATCH --ntasks 32
#SBATCH -o slurmjob-%j.out-%N
#SBATCH -e slurmjob-%j.err-%N
#SBATCH --account=usucs5030
#SBATCH --partition=kingspeak
#SBATCH --mail-user=nick.waddoups@usu.edu
#SBATCH --mail-type=FAIL,BEGIN,END

## This batch compares 4 and 5 with two nodes

# load in required modules
module load cmake/3.26.0 gcc/11.2.0 cuda openmpi/5.0.3

# move into the awanncu directory
cd ~/awannacu_cs6300

# build the project
make clean
make

# run cpu
mpiexec -n 16 ./build/src/distributed_cpu/dist_cpu input.raw output.raw 6000 6000 36
mpiexec -n 32 ./build/src/distributed_cpu/dist_cpu input.raw output.raw 6000 6000 36

# run gpu
mpiexec -n 16 ./build/src/distributed_gpu/dist_gpu input.raw output.raw 6000 6000 36
mpiexec -n 32 ./build/src/distributed_gpu/dist_gpu input.raw output.raw 6000 6000 36
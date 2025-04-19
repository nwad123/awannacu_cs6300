#!/bin/bash
#SBATCH --time=00:01:00
#SBATCH --nodes=1
#SBATCH --ntasks 32
#SBATCH -o slurmjob-%j.out-%N
#SBATCH -e slurmjob-%j.err-%N
#SBATCH --account=u0420314
#SBATCH --partition=kingspeak
#SBATCH --mail-user=nick.waddoups@usu.edu
#SBATCH --mail-type=FAIL,BEGIN,END

## This batch compares 1 and 2

# load in required modules
module load cmake/3.26.0 gcc/11.2.0 cuda openmpi/5.0.3

# move into the awanncu directory
cd ~/awannacu_cs6300

# build the project
make clean && make

# run serial
./build/src/parallel_cpu/par_cpu input.raw output.raw 6000 6000 36 1

# run parallel
./build/src/parallel_cpu/par_cpu input.raw output.raw 6000 6000 36 1
./build/src/parallel_cpu/par_cpu input.raw output.raw 6000 6000 36 2
./build/src/parallel_cpu/par_cpu input.raw output.raw 6000 6000 36 4
./build/src/parallel_cpu/par_cpu input.raw output.raw 6000 6000 36 8
./build/src/parallel_cpu/par_cpu input.raw output.raw 6000 6000 36 16
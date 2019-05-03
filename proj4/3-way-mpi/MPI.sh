#!/bin/bash
#SBATCH --constraint=elves 
#SBATCH --mem=2G 
#SBATCH --time=00:30:00
#--nodes=2 --ntasks-per-node=1
#SBATCH --mail-user=pc6@ksu.edu 
i=$1
j=$2
k=$3

module load OpenMPI

#mpicc --enable-debug -o mpi Proj4_mpi.c

mpirun --mca osc_base_verbose 100 -np ${i} ./proj4-mpi.o ${j} ${k}

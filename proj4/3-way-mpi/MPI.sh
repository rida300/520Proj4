#!/bin/bash
i=$1

#mpicc --enable-debug -o mpi Proj4_mpi.c

mpirun -np ${i} ./proj4-mpi.o

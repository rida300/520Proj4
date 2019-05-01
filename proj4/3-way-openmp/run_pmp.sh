#!/bin/bash

gcc -fopenmp -o omp Proj4_openmp.c
gcc -pthread -o p Proj4_pthreads.c

for i in 1 2 4 8 16
do
	for j in 1 2 4 8 16 32
	do
		sbatch --constraint=elves --nodes=1 --ntasks-per-node=${i} --time=24:00:00 --mem=6G omp.sh ${j}
	done
done

for i in 1 2 4 8 16
do
	for j in 1 2 4 8 16 32
	do
		sbatch --constraint=elves --nodes=1 --ntasks-per-node=${i} --time=24:00:00 --mem=6G pthread.sh ${j}
	done
done

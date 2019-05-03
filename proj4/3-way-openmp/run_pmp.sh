#!/bin/bash

gcc -fopenmp -std=c99 -o omp Proj4_openmp.c
#number of cores
for i in 1 2 4 8 16
do
	#number of threads
	for j in 1 2 4 8 16 32
	do
		#input size
		for k in 250000 500000 750000 1000000
		do
			#number of trials
			for l in 1 2 3
			do
				sbatch --constraint=elves --nodes=1 --ntasks-per-node=${i} --time=24:00:00 --mem=6G omp.sh ${k} ${j} ${i}
			done		
		done
	done
done


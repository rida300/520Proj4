#!/bin/bash

gcc -pthread -std=c99 -o p Proj4_pthreads.c
#core counts
for i in 1 2 4 8 16
do
	#thread counts
	for j in 1 2 4 8 16 32 
	do
		#trials for each configuration
		for k in 1 2 3
		do
		sbatch --constraint=elves --nodes=1 --ntasks-per-node=${i} --time=24:00:00 --mem=6G pthread.sh ${j}
		done
	done
done

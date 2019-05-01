#!/bin/bash

gcc -pthread -std=c99 -o p Proj4_pthreads.c

for i in 1
do
	for j in 2 
	do
		for k in 1
		do
		sbatch --constraint=elves --nodes=1 --ntasks-per-node=${i} --time=24:00:00 --mem=6G pthread.sh ${j}
		done
	done
done

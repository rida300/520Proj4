#!/bin/csh
##############################################################################
# FILE: batchscript.c
# DESCRIPTION:
#   Simple Slurm job command script for OMP exercise
#pthreads
# AUTHOR: Blaise Barney
# LAST REVISED: 05/23/17
##############################################################################

##### These lines are for Slurm
#SBATCH -N 1                      #Number of nodes to use
#SBATCH -p pReserved              #Workshop queue
#SBATCH -t 5:00                   #Maximum time required
#SBATCH -o output.%j              #Output file name

#SBATCH --constraint=elves
#SBATCH --mem-per-cpu=1G
### Job commands start here
### Display some diagnostic information
echo '=====================JOB DIAGNOTICS========================'
date
NUMNODES=$(echo 1)
NUMLINES=$(echo 4000)
NUMTHREADS=$(echo 8)
echo -n 'This machine is ';hostname
echo -n 'My jobid is '; echo $SLURM_JOBID
echo -n 'Number of nodes requested'; echo $NUMNODES
echo -n 'Number of lines to read'; echo $NUMLINES
echo 'My path is:'
echo $PATH
echo 'My job info:'
squeue -j $SLURM_JOBID
echo 'Machine info'
sinfo -s

echo '=====================JOB STARTING=========================='

### CHANGE THE LINES BELOW TO SELECT DIFFERENT MPI CODES AND/OR COMPILERS
cd ~/Project_4/omp/

#Compile an exercise code
gcc -fopenmp -Wall -o omp_exec LSS_omp.c
#Run the code
#srun -N1 ./omp_exec /homes/dan/625/wiki_dump.txt $NUMTHREADS $NUMLINES
srun -N1 ./omp_exec /homes/dan/625/wiki_dump.txt $NUMLINES $NUMTHREADS
### Issue the sleep so we have time to see the job actually running
sleep 120
echo ' '
echo '========================ALL DONE==========================='

#!/bin/bash
for i in 1 2 4 8 16
do
        MEM=4
        LETTER='G'
        SIZ=expr $MEM/$i

        echo $SIZ
        sbatch --constraint=dwarves --time=60:00:00 --mem-per-cpu=$SIZ --output=omp_$i.o%j --ntasks-per-node=$i  --nodes=1 2run_omp.sh
done


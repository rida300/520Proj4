#!/bin/bash -l
##$ -l h_rt=0:48:00
HEADER="User_Time,System_Time,Status,CPU_Percentage,CPU_Seconds,Elapsed_Real_Time_Seconds,command line arguments of the command being timed\n"
printf  $HEADER &>>/homes/ridaa/520Proj4/proj4/3-way-openmp/results/omp.csv 
for i in 2 4 8 16 32 64
do
#       echo 'enterting first for loop'
    for j in 1000 10000 100000 500000 1000000
       do
        #echo 'Problem Size ' $j 
        #echo 'task size ' $i

        touch /homes/ridaa/520Proj4/proj4/3-way-openmp/results/omp.csv 
        #/homes/ridaa/520Proj4/proj4/pthreads/pthreads_exec /homes/dan/625/wiki_dump.txt $j $i


#       HEADER="User_Time,System_Time,Status,CPU_Percentage,CPU_Seconds,Elapsed_Real_Time_Seconds,command line arguments of the command being timed\n"
#       printf "ProblemSize: %g" $j &>>/homes/ridaa/520Proj4/proj4/pthreads/results/pthreads.csv
#       printf "TaskSize: %g\n"$i &>>/homes/ridaa/520Proj4/proj4/pthreads/results/pthreads.csv

        #printf "TaskSize %g\n" $i | paste -sd ','&>>/homes/ridaa/520Proj4/proj4/pthreads/results/pthreads.csv
        # printf  $HEADER &>>/homes/ridaa/520Proj4/proj4/pthreads/results/pthreads.csv
/usr/bin/time -f "%U ,%S ,%x ,%P , %S, %e,%C\n" -a -o /homes/ridaa/520Proj4/proj4/3-way-openmp/omp/results/omp.csv /homes/ridaa/520Proj4/proj4/3-way-openmp/omp_exec /homes/dan/625/wiki_dump.txt $j $i

        done
#       echo 'done with first for loop'

done
        #echo 'done with second for loop'

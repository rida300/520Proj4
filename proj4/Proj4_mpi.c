#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define ARRAY_SIZE 5
#define ARTICLE_SIZE 100000
#define STRING_SIZE 1000
#define NUM_THREADS 2

char File_Contents[ARRAY_SIZE][ARTICLE_SIZE];
void *  find_longest_substring(int  id, char ** LCS, int linesPT);
int init_array(FILE *);
void print_results(char **);

int main() {

    int rank, number_of_proc, i, start_index, end_index;
	double start_time, end_time;
	
 
	// initialize each process
	MPI_Init ();
	// get number of running processes
	MPI_Comm_size (MPI_COMM_WORLD, &number_of_proc);
	// get number of this process
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
 
	FILE * fp;
  char * LCS[ARRAY_SIZE-1];
  bool root;
  MPI_Status status;
  
  root = rank == 0;
	
	if (root)
		start_time = MPI_Wtime ();
   // if this process is root process
	if (root)
	{
  
  fp= fopen ("testLorem.txt", "r");
	for(int i = 0; i<ARRAY_SIZE-1; i++)
		LCS[i] = (char *)malloc(sizeof(char) * (STRING_SIZE));
	
    
	int i = init_array(fp);
	const int LINES_PER_THREAD = i % NUM_THREADS == 0 ? i / NUM_THREADS : i / NUM_THREADS + 1;
  
  find_longest_substring(omp_get_thread_num(), LCS, LINES_PER_THREAD);

  // collect results from child processes
		for (i = 1; i < number_of_proc; i++)
		{
			int num_of_results, ye_ol_index, worker_num, p, j, k;
			int results_array_sizes[LINES_PER_THREAD];
			char *worker_msg = (char *)malloc (LINES_PER_THREAD * MAX_STRING_SIZE * sizeof (char));
			
			// receive number of results being sent by worker
			MPI_Recv (&num_of_results, 1, MPI_INT, i, send_tag*i+i, MPI_COMM_WORLD, &status);
			// receive worker's results_array_sizes array
			MPI_Recv (results_array_sizes, LINES_PER_THREAD, MPI_INT, i, send_tag*i+i+1, MPI_COMM_WORLD, &status);
			// receive worker's results array
			MPI_Recv (worker_msg, LINES_PER_THREAD * MAX_STRING_SIZE, MPI_CHAR, i, send_tag*i+i+2, MPI_COMM_WORLD, &status);
			
			ye_ol_index = i * LINES_PER_THREAD;
			
			// add result sizes to array
			for (k = 0; k < num_of_results; k++)
				result_sizes[ye_ol_index+k] = results_array_sizes[k];
			
			// add worker msg to results array
			for (p = 0; p < num_of_results; p++)
			{
				for (j = 0; j < results_array_sizes[p]-1; j++)
					results[ye_ol_index+p][j] = worker_msg[(p*MAX_STRING_SIZE)+j];
			}
			
			free (worker_msg);
		}

	print_results(LCS);
	printf("Main: program completed. Exiting.\n");
}


else
{

}

}





int init_array(FILE * fp)
{
	int i = 0;
	if(fp != NULL)
	{
		char line [ARTICLE_SIZE]; 
    		while ( fgets ( line, sizeof line, fp ) != NULL && i < ARRAY_SIZE) 
      		{
			strncpy(File_Contents[i++], line, strlen(line));

      		}
      		fclose ( fp );
	}
	return i;

}

void print_results(char ** LCS)
{
	int j;	// then print out the totals
	for ( int i = 0; i < ARRAY_SIZE-1; i++ ) 
	{
		j = i+1;
     		printf(" %d & %d - %s\n",i,j, LCS[i]);
  	}
}

void *  find_longest_substring(int  id, char ** LCS, int linesPT)//id is 0,1,2,3
{
  
 

 
	int startPos, endPos;
	char local_LCS[ARRAY_SIZE/NUM_THREADS][STRING_SIZE];
	char substring[STRING_SIZE];
	int i,j,x,y,maxlen,len, currPos = 0;
	int length1 = 0;
	int length2=0;
	int comp = 0;
	
	{
		startPos = (id) * linesPT;
		endPos = startPos + linesPT;
		for(currPos = startPos; currPos < endPos; currPos++)
		{
			maxlen = 0;
			length1 = strlen(File_Contents[currPos]);
			length2 = strlen(File_Contents[currPos+1]);
			for(i=0; i<=length1; i++)
			{
				for(j=0; j<=length2; j++)
				{
					if(File_Contents[currPos][i] == File_Contents[currPos+1][j])
					{
						substring[0] = File_Contents[currPos][i];
						len = 1;
						x = i+1;
						y = j+1;
						while(File_Contents[currPos][x] == File_Contents[currPos+1][y] && x < length1 && y<length2 && len < STRING_SIZE)
						{
							substring[len] = File_Contents[currPos][x];
							len++;
							x++;
							y++;
						}
						if(len>maxlen)
						{
							maxlen = len;
							strcpy(local_LCS[comp], substring);
						}
					}
				}
	   		 }
			comp++;
		}		//put substring in global array
		#pragma omp critical
		{
			int z = 0;
			for(currPos = startPos; currPos < endPos; currPos++)
			{ 
				strcpy(LCS[currPos], local_LCS[z]);
				z++;
			}
		}
	}
}

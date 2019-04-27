#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mpi.h"

#define MAX_ENTRIES 1000000		// longest file size will be 1000000 entries long
#define MAX_STRING_SIZE 10000
#define send_tag 1000

/* holds each line of the file */
char File_Contents[MAX_ENTRIES][MAX_STRING_SIZE];

void print (char x[], int i, int j, char **b, int *iptr, char *r)
{
	if (i == 0 || j == 0)
  	return;
	if (b[i][j] == 'c')
	{
    print (x, i-1, j-1, b, iptr, r);
    r[(*iptr)++] = x[i-1];
	}
	else if (b[i][j] == 'u')
  	print (x, i-1, j, b, iptr, r);
	else
    print (x, i, j-1, b, iptr, r);
}

void lcs (char x[], char y[], char **b, int **c)
{
	int i, j, m, n;
	
	m = strlen (x);
	n = strlen (y);
	
	for (i = 0; i <= m; i++)
  	c[i][0] = 0;
	for (i = 0; i <= n; i++)
    c[0][i] = 0;
	              
	//c, u and l denotes cross, upward and downward directions respectively
	for (i = 1; i <= m; i++)
	{
		for (j = 1; j <= n; j++)
		{
		  if (x[i-1] == y[j-1])
		  {
		    c[i][j] = c[i-1][j-1] + 1;
		    b[i][j] = 'c';
		  }
		  else if (c[i-1][j] >= c[i][j-1])
		  {
		    c[i][j] = c[i-1][j];
		    b[i][j] = 'u';
		  }
		  else
		  {
		    c[i][j] = c[i][j-1];
		    b[i][j] = 'l';
		  }
		}
	}
}

/* reads file, returns lines read */
int read_file (const char filename[], const int INPUT_SIZE)
{
	FILE *file;
	char x[MAX_STRING_SIZE];
	int index;
	
	file = fopen (filename, "r");
	
	if (file == NULL)
	{
		printf ("Bad file\n");
		return -1;
	}
	
	index = 0;
	while (fgets (x, MAX_STRING_SIZE, file) != NULL && index < INPUT_SIZE)
	{
		strcpy (File_Contents[index], x);
		index++;
	}
	
	fclose (file);
	return index; 
}

int main (int argc, char **argv)
{
	// no longer using third arg in program
	// but need it to run the program anyway
	if (argc != 4)
	{
		printf ("Usage: %s file input_size\n", argv[0]);
		return -1;
	}
	
	int rank, number_of_proc, i, start_pos, end_pos;
	double start_time, end_time;
	
	// Give each process the args
	MPI_Init (&argc, &argv);
	// get number of running processes
	MPI_Comm_size (MPI_COMM_WORLD, &number_of_proc);
	// get number of this process
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	
	const int INPUT_SIZE = atoi (argv[2]);
	const int NUM_OF_THREADS = number_of_proc;
	int lines_read = read_file (argv[1], INPUT_SIZE);
	
	if (lines_read < 0)
		return -1;
		
	const int LCS_SIZE = lines_read - 1;
	const int LINES_PER_THREAD = lines_read % NUM_OF_THREADS == 0 ? lines_read/NUM_OF_THREADS : lines_read/NUM_OF_THREADS + 1;
	
	char **LCS;
	int *result_sizes;
	bool root;
	MPI_Status status;
	
	root = rank == 0;
	
	if (root)
		start_time = MPI_Wtime ();
	
	// if this process is root process
	if (root)
	{
		// allocate memory for LCS buffer
		LCS = (char **)malloc (LCS_SIZE * sizeof (char *));
		for (i = 0; i < LCS_SIZE; i++)
			LCS[i] = (char *)malloc (MAX_STRING_SIZE * sizeof (char));
		// allocate memory for LCS sizes
		result_sizes = (int *)malloc (LCS_SIZE * sizeof (int));
		
		start_pos = rank * LINES_PER_THREAD;
		end_pos = start_pos + LINES_PER_THREAD;
		
		// get the lcs with parent process
		int o = start_pos;
		while (o < end_pos && o < lines_read-1)
		{
			int j, m, n, k, index;
			int xlen = strlen (File_Contents[o]);
			int ylen = strlen (File_Contents[o+1]);
			char *x = (char *)malloc ((xlen+1) * sizeof (char)); 			// first string
			char *y = (char *)malloc ((ylen+1) * sizeof (char));			// second string
		
			char **b = (char **)malloc ((xlen+1) * sizeof (char *));
			int **c = (int **)malloc ((xlen+1) * sizeof (int *));
		
			// initialize b and c
			for (j = 0; j <= xlen; j++)
			{
				b[j] = (char *)malloc ((ylen+1) * sizeof (char));
				c[j] = (int *)malloc ((ylen+1) * sizeof (int));
			}
		
			strncpy (x, File_Contents[o], xlen+1);
			strncpy (y, File_Contents[o+1], ylen+1);
		
			lcs (x, y, b, c);
		
			// get lcs lengths
			m = strlen (x);
			n = strlen (y);		
		
			index = 0;
			print (x, m, n, b, &index, LCS[o]);
		
			// save the size of the LCS string
			result_sizes[o] = index;
		
			for (k = 0; k < xlen; k++)
			{
				free (b[k]);
				free (c[k]);
			}
			
			free (b);
			free (c);
			free (x);
			free (y);
			o++;
		}
		
		// collect LCS from child processes
		for (i = 1; i < number_of_proc; i++)
		{
			int num_of_LCS, ye_ol_index, worker_num, p, j, k;
			int LCS_array_sizes[LINES_PER_THREAD];
			char *worker_msg = (char *)malloc (LINES_PER_THREAD * MAX_STRING_SIZE * sizeof (char));
			
			// receive number of LCS being sent by worker
			MPI_Recv (&num_of_LCS, 1, MPI_INT, i, send_tag*i+i, MPI_COMM_WORLD, &status);
			// receive worker's LCS_array_sizes array
			MPI_Recv (LCS_array_sizes, LINES_PER_THREAD, MPI_INT, i, send_tag*i+i+1, MPI_COMM_WORLD, &status);
			// receive worker's LCS array
			MPI_Recv (worker_msg, LINES_PER_THREAD * MAX_STRING_SIZE, MPI_CHAR, i, send_tag*i+i+2, MPI_COMM_WORLD, &status);
			
			ye_ol_index = i * LINES_PER_THREAD;
			
			// add result sizes to array
			for (k = 0; k < num_of_LCS; k++)
				result_sizes[ye_ol_index+k] = LCS_array_sizes[k];
			
			// add worker msg to LCS array
			for (p = 0; p < num_of_LCS; p++)
			{
				for (j = 0; j < LCS_array_sizes[p]-1; j++)
					LCS[ye_ol_index+p][j] = worker_msg[(p*MAX_STRING_SIZE)+j];
			}
			
			free (worker_msg);
		}
		
		int s;
		printf ("LCS:\n");
		for (i = 0; i < LCS_SIZE; i++)
		{
			printf ("\tLines %d-%d:\t", i+1, i+2);
			for (s = 0; s < result_sizes[i]-1; s++)
				printf ("%c", LCS[i][s]);
			printf ("\n");
		}
	}
	else
	{
		char *worker_LCS;
		int LCS_array_sizes[LINES_PER_THREAD];
		start_pos = rank * LINES_PER_THREAD;
		end_pos = start_pos + LINES_PER_THREAD;
		
		
		// allocate memory for worker result array
		// **must be contiguous for mpi**
		worker_LCS = (char *)malloc (LINES_PER_THREAD * MAX_STRING_SIZE * sizeof (char));
			
		// worker variable for worker result size
		int worker_result_size = 0;
		
		// do the thing
		int o = start_pos;
		int index = 0;
		i = 0;
		while (o < end_pos && o < lines_read-1)
		{
			int j, m, n, k, offset;
			int xlen = strlen (File_Contents[o]);
			int ylen = strlen (File_Contents[o+1]);
			char *x = (char *)malloc ((xlen+1) * sizeof (char)); 			// first string
			char *y = (char *)malloc ((ylen+1) * sizeof (char));			// second string
		
			char **b = (char **)malloc ((xlen+1) * sizeof (char *));
			int **c = (int **)malloc ((xlen+1) * sizeof (int *));
		
			// initialize b and c
			for (j = 0; j <= xlen; j++)
			{
				b[j] = (char *)malloc ((ylen+1) * sizeof (char));
				c[j] = (int *)malloc ((ylen+1) * sizeof (int));
			}
		
			strncpy (x, File_Contents[o], xlen+1);
			strncpy (y, File_Contents[o+1], ylen+1);
		
			lcs (x, y, b, c);
		
			// get lcs lengths
			m = strlen (x);
			n = strlen (y);		
			
			index += (i * MAX_STRING_SIZE) - index;
			print (x, m, n, b, &index, worker_LCS);
			LCS_array_sizes[i] = index - (i * MAX_STRING_SIZE);
			
			for (k = 0; k < xlen; k++)
			{
				free (b[k]);
				free (c[k]);
			}
			
			free (b);
			free (c);
			free (x);
			free (y);
			o++, i++, worker_result_size++;
		}
		
		// send stuff to parent process
		MPI_Send (&worker_result_size, 1, MPI_INT, 0, send_tag*rank+rank, MPI_COMM_WORLD);
		MPI_Send (LCS_array_sizes, LINES_PER_THREAD, MPI_INT, 0, send_tag*rank+rank+1, MPI_COMM_WORLD);
		MPI_Send (worker_LCS, LINES_PER_THREAD * MAX_STRING_SIZE, MPI_CHAR, 0, send_tag*rank+rank+2, MPI_COMM_WORLD);
		
		free (worker_LCS);
	}
	
	if (root)
	{
		end_time = MPI_Wtime ();
		printf ("\nTime elapsed: %f\n", end_time - start_time);
	}
	
	MPI_Finalize ();
	return 0;
}

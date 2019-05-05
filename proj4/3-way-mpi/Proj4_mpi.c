#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "sys/types.h"
#include "sys/sysinfo.h"
#include <mpi.h>
#include <math.h>
#define ARTICLE_SIZE 2000
#define STRING_SIZE 2000
#define LINES 250000

typedef struct {
  uint32_t virtualMem;
  uint32_t physicalMem;
} processMem_t;


void find_longest_substring(char* line1_file, char* line2_file, int length1, int length2, int line1, int line2);

void undoMalloc(int **file, int num_line);
void print_results();
void GetProcessMemory(processMem_t*);
int parseLine(char *);

int init_Array(FILE *);


int NUM_THREADS;
char File_Contents[LINES][ARTICLE_SIZE];

char  local_LCS[LINES-1][STRING_SIZE];
char  LCS [LINES-1][STRING_SIZE];




void LCS_intermediate(int id)//, char File_ContentsSub[LINES/NUM_THREADS][ARTICLE_SIZE]) 
{
	int startPos = (id) * (LINES / NUM_THREADS);
	int endPos =  startPos + (LINES / NUM_THREADS)-1;
	int currPos = 0;
	int length1 = 0;
	int length2 = 0;
	char substring[STRING_SIZE];
	int i, j, x, y, maxlen, len;
	int comp = 0;
	for (currPos = startPos; currPos < endPos; currPos++)
	{
			printf("node-%d Processing from-%d to %d\n", id, currPos, endPos);
			fflush(stdout);		
			maxlen = 0;
			length1 = strlen(File_Contents[currPos]);
			length2 = strlen(File_Contents[currPos + 1]);
			for (i = 0; i <= length1; i++)
			{

				for (j = 0; j <= length2; j++)
				{
					if (File_Contents[currPos][i] == File_Contents[currPos + 1][j])
					{
	
						substring[0] = File_Contents[currPos][i];
						len = 1;
						x = i + 1;
						y = j + 1;
						while (File_Contents[currPos][x] == File_Contents[currPos + 1][y] && x < length1 && y < length2 && len < STRING_SIZE)
						{
							substring[len] = File_Contents[currPos][x];
							len++;
							x++;
							y++;
						}
						if (len > maxlen)
						{	
							substring[len] = '\0';	
							maxlen = len;
							strcpy(local_LCS[comp], substring);
						
						}
					}
				}
			}
			
			comp++;
		}		//put substring in global array

if(id == 0){
		int z = 0;
		for (currPos = startPos; currPos < endPos; currPos++)
		{
			strcpy(LCS[currPos], local_LCS[z]);
			z++;

		}
		//free(local_LCS);
	}

		
}


void array_init(char *** array, int row, int col) {
	int i;
	char * temp_array = (char *) malloc(row * col * sizeof(char));
	(* array) = (char **) malloc(row * sizeof(char *));
	for (i = 0; i < row; i++) {
		(*array)[i] = &(temp_array[i * col]);
	}
}

void print_results()
{
	int j;	// then print out the totals
	for (int i = 0; i < LINES - 1; i++)
	{
		j = i + 1;
		printf(" %d & %d - %s\n", i, j, &LCS[i]);
	}
}

int init_Array(FILE * fp)
{
	int i = 0;
	if (fp != NULL)
	{
		char line[ARTICLE_SIZE];
		while (fgets(line, ARTICLE_SIZE, fp) != NULL  && i < LINES)
		{
			strcpy(File_Contents[i], line);
      			i++;
 	

		}
		fclose(fp);
	}
	return i;

}

int main(int argc, char *argv[])
{

	int process_number, rank;

	MPI_Status Status;

	//intialize mpi program; master/root is rank 0
	int root = MPI_Init(&argc, &argv);
	if (root != MPI_SUCCESS) {
		printf("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, root);
	}

	MPI_Comm_size(MPI_COMM_WORLD, &process_number);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//determine the number of threads and set it
	NUM_THREADS = process_number;

	struct timeval t1, t2;
	double elapsedTime;
	gettimeofday(&t1, NULL);
	processMem_t myMemory;
	
	FILE * fp = fopen("/homes/dan/625/wiki_dump.txt", "r");       	//argv[1], "r");
	if (fp == NULL) {
		printf("File not found \n");
		return 0;
	}

	//process array in lcs function	
	

	if(rank ==0)
		init_Array(fp);
	

	int out = MPI_Bcast(File_Contents, (LINES)*(ARTICLE_SIZE), MPI_CHAR, 0, MPI_COMM_WORLD);


	LCS_intermediate(rank);//, File_ContentsSub);
	MPI_Barrier(MPI_COMM_WORLD);
	//free(File_Contents);
	int start = rank * LINES/NUM_THREADS;
	int end = start + LINES/NUM_THREADS-1;


	if (rank != 0) {
		
		for (int i = start; i < end; i++) {
			printf("sending\n");
			fflush(stdout);
			MPI_Send(local_LCS[i - start], sizeof(char)*STRING_SIZE, MPI_CHAR, 0, i, MPI_COMM_WORLD);
			
		}
	}
	else{
		
		if (NUM_THREADS > 1) {
			for (int i = 1; i < NUM_THREADS; i++) {
				printf("receiving\n");
				fflush(stdout);
				start = i * LINES/NUM_THREADS;
				end = start + LINES/NUM_THREADS-1;
				for (int j = start; j < end; j++) {
				
					
						// may need to allocate memory
						MPI_Recv((LCS[j]), sizeof(char)*STRING_SIZE, MPI_CHAR, i, j, MPI_COMM_WORLD, &Status);
					
				}
			}
		}
	}
	GetProcessMemory(&myMemory);	
	
	if (rank == 0) {
		print_results();
		gettimeofday(&t2, NULL);
		elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
		elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
		printf("%s, %f\n", getenv("SLURM_CPUS_ON_NODE"), elapsedTime);
	  	printf("DATA, %d, %u, %u\n", NUM_THREADS, myMemory.virtualMem, myMemory.physicalMem);
		printf("Main: program completed. Exiting.\n");
	}
	
  	MPI_Finalize();
	return 0;
}

int parseLine(char *line) {
	// This assumes that a digit will be found and the line ends in " Kb".
	int i = strlen(line);
	const char *p = line;
	while (*p < '0' || *p > '9') p++;
	line[i - 3] = '\0';
	i = atoi(p);
	
	return i;
}// end parseLine

void undoMalloc(int **file, int num_line)
{
	for (int i = 0; i < num_line; i++)
	{
		free(file[i]);
	}
	free(file);
}

void GetProcessMemory(processMem_t* processMem) {
	FILE *file = fopen("/proc/self/status", "r");
	char line[128];

	while (fgets(line, 128, file) != NULL) {
		//printf("%s", line);
		if (strncmp(line, "VmSize:", 7) == 0) {
			processMem->virtualMem = parseLine(line);
		}

		if (strncmp(line, "VmRSS:", 6) == 0) {
			processMem->physicalMem = parseLine(line);
		}
	}
	fclose(file);
}// end GetProcessMemory

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "sys/types.h"
#include "sys/sysinfo.h"
#include <mpi.h>
#include <math.h>
#define ARTICLE_SIZE 2100
#define STRING_SIZE 2100


typedef struct {
  uint32_t virtualMem;
  uint32_t physicalMem;
} processMem_t;


void find_longest_substring(char* line1_file, char* line2_file, int length1, int length2, int line1, int line2);
void LCS_intermediate(int id, char ** LCS);
void undoMalloc(int **file, int num_line);
void print_results(char ** LCS);
void GetProcessMemory(processMem_t*);
int parseLine(char *);

int init_Array(FILE *);


int NUM_THREADS;
char** File_Contents;
char ** local_LCS;
char ** LCS;
int Lines_Read;



void LCS_intermediate(int id, char ** LCS ) 
{
printf("%d\n", id);
fflush(stdout);
	int startPos = (id) * (Lines_Read / NUM_THREADS);
	int endPos = startPos + (Lines_Read / NUM_THREADS);
	//int first_line = startPos;
	//int second_line = first_line + 1;
	int currPos = 0;
	int length1 = 0;
	int length2 = 0;
//	char local_LCS[Lines_Read / NUM_THREADS][STRING_SIZE];
	char substring[STRING_SIZE];
	int i, j, x, y, maxlen, len;
	int comp = 0;
if(id == NUM_THREADS-1)
		{
			endPos = Lines_Read-1;
}	
	for (currPos = startPos; currPos < endPos; currPos++)
	{
		printf("%d\n", currPos);
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
							printf("%s\n", local_LCS[comp]);
							fflush(stdout);	
						}
					}
				}
			}
			comp++;
		}		//put substring in global array

/*
		int z = 0;
		for (currPos = startPos; currPos < endPos; currPos++)
		{
			strcpy(LCS[currPos], local_LCS[z]);
			z++;
		}
		
	for (currPos = startPos; (currPos < endPos) && (currPos + 1 <Lines_Read); currPos++)
	{
		length1 = strlen(File_Contents[currPos]);
		length2 = strlen(File_Contents[currPos + 1]);

		find_longest_substring(File_Contents[currPos], File_Contents[currPos +1], length1, length2, first_line, second_line);
		first_line = second_line;
		second_line++;
	}
	*/	
		
}


void array_init(char *** array, int row, int col) {
	int i;
	char * temp_array = (char *) malloc(row * col * sizeof(char));
	(* array) = (char **) malloc(row * sizeof(char *));
	for (i = 0; i < row; i++) {
		(*array)[i] = &(temp_array[i * col]);
	}
}

void print_results(char ** LCS)
{
	int j;	// then print out the totals
	for (int i = 0; i < Lines_Read - 1; i++)
	{
		j = i + 1;
		printf(" %d & %d - %s\n", i, j, LCS[i]);
	}
}

int init_Array(FILE * fp)
{
	int i = 0;
	if (fp != NULL)
	{
		char line[ARTICLE_SIZE];
		while (fgets(line, ARTICLE_SIZE, fp) != NULL  && i < Lines_Read)
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

	struct timeval t1, t2;
	double elapsedTime;
	gettimeofday(&t1, NULL);

	if (argc != 3)
	{
		printf("Enter the filename followed by the number of lines\n");
		return 0;
	}
	processMem_t myMemory;
	FILE * fp = fopen("/homes/dan/625/wiki_dump.txt", "r");       	//argv[1], "r");
	if (fp == NULL) {
		printf("File not found \n");
		return 0;
	}
	int input_lines = atoi(argv[2]);
	Lines_Read = input_lines;

	array_init(&File_Contents, input_lines, ARTICLE_SIZE);

	int i=0;
	init_Array(fp);

	array_init(&LCS, input_lines-1, STRING_SIZE);
	//process array in lcs function	
	
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

	array_init(&local_LCS, (input_lines/NUM_THREADS), STRING_SIZE);
	
	MPI_Bcast(File_Contents, input_lines*ARTICLE_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);	

	LCS_intermediate(rank, LCS);
	//print_results(LCS);
	//When root process ends, terminate MPI
printf("out of the alg");
fflush(stdout);
	MPI_Reduce(local_LCS, LCS, (input_lines-1)*500 , MPI_CHAR, MPI_SUM, 0, MPI_COMM_WORLD);	
	printf("i got through the functions");
	fflush(stdout);

	GetProcessMemory(&myMemory);	
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0) {
		print_results(LCS);
		gettimeofday(&t2, NULL);
		elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
		elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
		printf("%s, %f\n", getenv("SLURM_CPUS_ON_NODE"), elapsedTime);
  	printf("DATA, %d, %u, %u\n", NUM_THREADS, myMemory.virtualMem, myMemory.physicalMem);
		printf("Main: program completed. Exiting.\n");
	}
	//free the array
//	undoMalloc((int **)File_Contents, Lines_Read);
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
	printf("in parse: %d", i);
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

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
#define LINES 100

typedef struct {
  uint32_t virtualMem;
  uint32_t physicalMem;
} processMem_t;


void find_longest_substring(char* line1_file, char* line2_file, int length1, int length2, int line1, int line2);
//void LCS_intermediate(int id);
void undoMalloc(int **file, int num_line);
void print_results();
void GetProcessMemory(processMem_t*);
int parseLine(char *);

int init_Array(FILE *);


int NUM_THREADS;
char File_Contents[LINES][ARTICLE_SIZE];
//char File_ContentsSub[LINES][ARTICLE_SIZE];
char ** local_LCS;
char  LCS [LINES-1][STRING_SIZE];




void LCS_intermediate(int id, char File_ContentsSub[LINES/NUM_THREADS][ARTICLE_SIZE]) 
{
printf("%d\n", id);
fflush(stdout);
	int startPos = 0;//(id) * (LINES / NUM_THREADS);
	int endPos =  (LINES / NUM_THREADS)-1;
	//int first_line = startPos;

	//int second_line = first_line + 1;
	int currPos = 0;
	int length1 = 0;
	int length2 = 0;
//	char local_LCS[Lines_Read / NUM_THREADS][STRING_SIZE];
	char substring[STRING_SIZE];
	int i, j, x, y, maxlen, len;
	int comp = 0;
//			printf("node-%d processing from-%d to %d\n",id, currPos, endPos);
//			fflush(stdout);	
	for (currPos = startPos; currPos < endPos; currPos++)
	{
		
			maxlen = 0;
			printf("before the strlen");fflush(stdout);
			length1 = strlen(File_ContentsSub[currPos]);
//			printf("Read the file contents");
//			fflush(stdout);
			length2 = strlen(File_ContentsSub[currPos + 1]);
			for (i = 0; i <= length1; i++)
			{
//				printf("node-%d processing from-%d to %d\n", id, currPos, endPos);
//				fflush(stdout);
				for (j = 0; j <= length2; j++)
				{
		//			printf("i=%d    j=%d\n", i, j);
		//			fflush(stdout);
					if (File_ContentsSub[currPos][i] == File_ContentsSub[currPos + 1][j])
					{
					//	printf("in comparison\n");
					//	fflush(stdout);
						substring[0] = File_ContentsSub[currPos][i];
						len = 1;
						x = i + 1;
						y = j + 1;
						while (File_ContentsSub[currPos][x] == File_ContentsSub[currPos + 1][y] && x < length1 && y < length2 && len < STRING_SIZE)
						{
							substring[len] = File_ContentsSub[currPos][x];
							len++;
							x++;
							y++;
						}
						if (len > maxlen)
						{	
							printf("copying\n");
							fflush(stdout);
							substring[len] = '\0';
							printf("after null char\n");
							fflush(stdout);	
						maxlen = len;
							strcpy(local_LCS[currPos], substring);
							printf("%s\n", local_LCS[currPos]);
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

void print_results()
{
	int j;	// then print out the totals
	for (int i = 0; i < LINES - 1; i++)
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
	int input_lines = atoi(argv[2]);
	//process array in lcs function	
	

	if(rank ==0)
		init_Array(fp);
	
	char  File_ContentsSub[LINES/NUM_THREADS][ARTICLE_SIZE];


	array_init(&local_LCS, (LINES/NUM_THREADS)-1,STRING_SIZE);
//	int out = MPI_Bcast(File_Contents, (Lines_Read)*(ARTICLE_SIZE), MPI_CHAR, 0, MPI_COMM_WORLD);
//printf("%d - %d\n", out, rank);
//fflush(stdout);

	MPI_Scatter(File_Contents, (LINES/(NUM_THREADS))*ARTICLE_SIZE, MPI_CHAR, File_ContentsSub, (LINES/(NUM_THREADS))*ARTICLE_SIZE,MPI_CHAR, 0, MPI_COMM_WORLD);
//printf("node-%d line1=%s\n", rank, File_ContentsSub[0]);
//fflush(stdout);


if(rank != 0)
	LCS_intermediate(rank, File_ContentsSub);


	//print_results(LCS);
	//When root process ends, terminate MPI
printf("out of the alg");
fflush(stdout);
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Gather(local_LCS, (LINES/NUM_THREADS)*ARTICLE_SIZE, MPI_CHAR, LCS, (LINES/NUM_THREADS)*ARTICLE_SIZE , MPI_CHAR, 0, MPI_COMM_WORLD);//change this to gather	
	printf("%s\n%s\n", LCS[50],LCS[75]);
	fflush(stdout);

	GetProcessMemory(&myMemory);	
	//MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0) {
		print_results();
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

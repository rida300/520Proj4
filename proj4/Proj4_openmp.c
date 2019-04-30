
#include <sys/time.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "sys/types.h"
#include "sys/sysinfo.h"
#define ARRAY_SIZE 5
#define ARTICLE_SIZE 10000
#define STRING_SIZE 200

typedef struct {
  uint32_t virtualMem;
  uint32_t physicalMem;
} processMem_t;

int NUM_THREADS;
int INPUT_LINES;
 
char File_Contents[ARRAY_SIZE][ARTICLE_SIZE];
void * find_longest_substring(int id, char **);
void GetProcessMemory(processMem_t*);
int init_Array(FILE * fp);
void print_results(char **);
int parseLine(char *);

int main(int argc,  const char ** argv) {

  if (argc != 3)
	{
		printf ("%s is not a valid inputs\n", argv[0]);
		return -1;
	}
	int numSlots, myVersion = 1;
	processMem_t myMemory;
	INPUT_LINES = atoi(argv[1]);
	NUM_THREADS = atoi(argv[2]);
	struct timeval t1, t2;
	double elapsedTime;
	gettimeofday(&t1, NULL);

	FILE * fp = fopen("testLorem.txt", "r");
  int linesRead = init_Array(fp);
  if(linesRead<0) return -1;
  
	char * LCS[ARRAY_SIZE - 1];
	for (int i = 0; i < ARRAY_SIZE - 1; i++)
		LCS[i] = (char *)malloc(sizeof(char) * (STRING_SIZE));
	omp_set_num_threads(NUM_THREADS);



#pragma omp parallel 
	{
		find_longest_substring(omp_get_thread_num(), LCS);
		
	}
	print_results(LCS);
	GetProcessMemory(&myMemory);
	gettimeofday(&t2, NULL);
	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms

	printf("Total time to run: %f\n", elapsedTime);
	printf("DATA, %d, %u, %u\n", NUM_THREADS, myMemory.virtualMem, myMemory.physicalMem);
	printf("Main: program completed. Exiting.\n");
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

int init_Array(FILE * fp)
{
	int i = 0;
	if (fp != NULL)
	{
		char line[ARTICLE_SIZE];
		while (fgets(line, ARTICLE_SIZE, fp) != NULL  && i < INPUT_LINES)
		{
			strcpy(File_Contents[i], line);
      			i++;
 	

		}
		fclose(fp);
	}
	return i;

}

void print_results(char ** LCS)
{
	int j;	// then print out the totals
	for (int i = 0; i < ARRAY_SIZE - 1; i++)
	{
		j = i + 1;
		printf(" %d & %d - %s\n", i, j, LCS[i]);
	}
}

void *  find_longest_substring(int  id, char ** LCS)//id is 0,1,2,3
{
	int startPos, endPos;
	char local_LCS[ARRAY_SIZE / NUM_THREADS+2][STRING_SIZE];
	char substring[STRING_SIZE];
	int i, j, x, y, maxlen, len, currPos = 0;
	int length1 = 0;
	int length2 = 0;
	int comp = 0;
#pragma omp private(id, startPos, endPos, currPos, local_LCS, i, j, x, y, maxlen, len, substring, comp)
	{
		startPos = (id) * (INPUT_LINES/NUM_THREADS);
		endPos = startPos + (INPUT_LINES/NUM_THREADS);
		if(id == NUM_THREADS-1)
		{
			endPos = INPUT_LINES-1;
		}
		
		
		printf("%d - start, %d - end \n", startPos, endPos);
		for (currPos = startPos; currPos < endPos; currPos++)
		{
			printf("%d", currPos);
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
							maxlen = len;
              						substring[maxlen]='\0';
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
			for (currPos = startPos; currPos < endPos; currPos++)
			{
				strcpy(LCS[currPos], local_LCS[z]);
				z++;
			}
		}
	}
}

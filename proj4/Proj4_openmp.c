
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ARRAY_SIZE 5
#define ARTICLE_SIZE 1000
#define STRING_SIZE 15
#define NUM_THREADS 4

char File_Contents[ARRAY_SIZE][ARTICLE_SIZE];
void * find_longest_substring(int  id, char **);
void init_array(FILE *);
void print_results(char **);

int main() {

	FILE * fp = fopen ("testLorem.txt", "r");
	char * LCS[ARRAY_SIZE-1];
	for(int i = 0; i<ARRAY_SIZE-1; i++)
		LCS[i] = (char *)malloc(sizeof(char) * (STRING_SIZE));
	omp_set_num_threads(NUM_THREADS);

	init_array(fp);
	#pragma omp parallel 
	{
		find_longest_substring(omp_get_thread_num(), LCS);
	}
	print_results(LCS);
	printf("Main: program completed. Exiting.\n");
}

void init_array(FILE * fp)
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

void *  find_longest_substring(int  id, char ** LCS)//id is 0,1,2,3
{
	int startPos, endPos;
	char local_LCS[ARRAY_SIZE/NUM_THREADS][STRING_SIZE];
	char substring[STRING_SIZE];
	int i,j,x,y,maxlen,len, currPos = 0;
	int length1 = 0;
	int length2=0;
	int comp = 0;
	#pragma omp private(id, startPos, endPos, currPos, local_LCS, i, j, x, y, maxlen, len, substring, comp)
	{
		startPos = (id) * (ARRAY_SIZE / NUM_THREADS);
		endPos = startPos + (ARRAY_SIZE / NUM_THREADS);
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

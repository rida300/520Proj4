
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ARRAY_SIZE 7
#define ARTICLE_SIZE 100
#define STRING_SIZE 15
#define NUM_THREADS 3

//char LCS[ARRAY_SIZE][STRING_SIZE+1];
char File_Contents[ARRAY_SIZE][ARTICLE_SIZE];
void * find_longest_substring(int  id, char **);
void init_array(FILE *);
void print_results(char **);

int main() {

	FILE * fp = fopen ("testFile.txt", "r");
	char * LCS[ARRAY_SIZE];
	for(int i = 0; i<ARRAY_SIZE; i++)
		LCS[i] = (char *)malloc(sizeof(char) * (STRING_SIZE+3));
	omp_set_num_threads(NUM_THREADS);

	init_array(fp);
//	strcpy(File_Contents[0], "This is what I am doing instead of using the stupid fopen function");
//	strcpy(File_Contents[1], "This is how I am testing instead of using the stupid fopen function");
	#pragma omp parallel 
	{
		find_longest_substring(omp_get_thread_num(), LCS);
	}
//	int id = 0;
//	find_longest_substring(&id);
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
		strncpy(File_Contents[i++], line, strlen(line)-1);

      }
      fclose ( fp );
	  }
	
}

void print_results(char ** LCS)
{
int j;
  					// then print out the totals
  for ( int i = 0; i < ARRAY_SIZE - 1; i++ ) {
  j = i+1;
     printf(" %d & %d - %s\n",i,j, LCS[i]);
  }
}

void *  find_longest_substring(int  id, char ** LCS)//id is 0,1,2,3
{
	int tempCount = 0;
	int startPos, endPos;
	char local_LCS[ARRAY_SIZE/NUM_THREADS][STRING_SIZE+2];
	char substring[STRING_SIZE+1];
	int i,j,x,y,maxlen,len, currPos = 0;
	int length1 = 0;
	int length2=0;
	int comp = 0;
		#pragma omp private(id, startPos, endPos, currPos, local_LCS, i, j, x, y, maxlen, len, substring)
		{
		startPos = (id) * (ARRAY_SIZE / NUM_THREADS);
		endPos = startPos + (ARRAY_SIZE / NUM_THREADS);
//		if(id == NUM_THREADS-1)
//		{
//		   endPos--; //want to stop before currpos++ is out of bounds
//		}
		
		for(currPos = startPos; currPos < endPos; currPos++)
		{
		//printf("%d", currPos);
		maxlen = 0;
		length1 = strlen(File_Contents[currPos]);
		length2 = strlen(File_Contents[currPos+1]);
		printf("%d & %d\n", length1, length2);
		for(i=0; i< length1; i++)
		{
			for(j=0; j<length2; j++)
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
						substring[maxlen] = '\0';
						strcpy(local_LCS[comp], substring);
						//local_LCS[currPos][maxlen] = '\0';
						tempCount++;
						printf("%d - %d - %s\n", tempCount, currPos, local_LCS[comp]);
						//for(int q = 0; q < STRING_SIZE; q++){
						//	substring[q] = 0;}

					}
				}
			}
		    }
		comp++;
		}		//put substring in global array
		#pragma omp critical
		{	int z = 0;
			for(currPos = startPos; currPos < endPos; currPos++)
			{
			//printf("%s\n", local_LCS[currPos]); 
			strcpy(LCS[currPos], local_LCS[z]);//, strlen(local_LCS[currPos]));
			printf("%s\n", LCS[currPos]);
			z++;
			}
//printf("%s", local_LCS[3]);
		}


	}
}

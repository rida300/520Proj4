#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ARRAY_SIZE 4
#define ARTICLE_SIZE 100
#define STRING_SIZE 100
#define NUM_THREADS 1

char LCS[ARRAY_SIZE][STRING_SIZE];
char File_Contents[ARRAY_SIZE][ARTICLE_SIZE];
void * find_longest_substring(int  id);
void init_array(FILE *);
void print_results();

int main() {

	FILE * fp = fopen ("testFile.txt", "r");
	
	omp_set_num_threads(NUM_THREADS);

	init_array(fp);
//	strcpy(File_Contents[0], "This is what I am doing instead of using the stupid fopen function");
//	strcpy(File_Contents[1], "This is how I am testing instead of using the stupid fopen function");
//	#pragma omp parallel 
//	{
		find_longest_substring(0);
//	}
//	int id = 0;
//	find_longest_substring(&id);
	print_results();

	printf("Main: program completed. Exiting.\n");
}

void init_array(FILE * fp)
{
int i = 0;
if(fp != NULL)
{
 char line [1000]; 
      while ( fgets ( line, sizeof line, fp ) != NULL ) 
      {
		strcpy(File_Contents[i++], line);
      }
      fclose ( fp );
	  }
	
}

void print_results()
{
int j;
  					// then print out the totals
  for ( int i = 0; i < ARRAY_SIZE - 1; i++ ) {
  j = i+1;
     printf(" %d & %d - %s\n",i,j, LCS[i]);
  }
}

void *  find_longest_substring(int  id)//id is 0,1,2,3
{
	int startPos, endPos;
	char local_LCS[ARRAY_SIZE/NUM_THREADS][STRING_SIZE];
	char substring[STRING_SIZE];
	int i,j,x,y,maxlen,len, currPos = 0;
		//#pragma omp private(id, startPos, endPos, currPos, longestSS, i, j, x, y, maxlen, len)
		//{
		startPos = (id) * (ARRAY_SIZE / NUM_THREADS);
		endPos = startPos + (ARRAY_SIZE / NUM_THREADS);
		if(id == NUM_THREADS-1)
		{
		   endPos--; //want to stop before currpos++ is out of bounds
		}
		
		for(currPos = startPos; currPos < endPos; currPos++)
		{
		printf("%d", currPos);
		maxlen = 0;
		for(i=0; i<strlen(File_Contents[currPos]); i++)
		{
			for(j=0; j<strlen(File_Contents[currPos+1]); j++)
			{
				if(File_Contents[currPos][i] == File_Contents[currPos+1][j])
				{
				
					substring[0] = File_Contents[currPos][i];
					len = 1;
					x = i;
					y = j;
					while(File_Contents[currPos][++x] == File_Contents[currPos+1][++y])
					{
						substring[len++] = File_Contents[currPos][x];
					}
					if(len>maxlen)
					{
						maxlen = len;
						strcpy(local_LCS[currPos], substring);
					}
				}
			}
		    }

		}		//put substring in global array
//		#pragma omp critical
		//{
			for(currPos = startPos; currPos < endPos; currPos++)
			{
			strcpy(LCS[currPos], local_LCS[currPos]);
			}
//		}


	//}
}

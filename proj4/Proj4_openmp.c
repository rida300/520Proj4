#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ARRAY_SIZE 1200000
#define ARTICLE_SIZE 1000
#define STRING_SIZE 100
#define NUM_THREADS 4

char LCS[ARRAY_SIZE][STRING_SIZE];
char File_Contents[ARRAY_SIZE][ARTICLE_SIZE];

main() {

	File * fp = fopen ("wiki_dump.txt", "r");
	

	omp_set_num_threads(NUM_THREADS);

	init_arrays(fp);

	#pragma omp parallel 
	{
		find_longest_substring(omp_get_thread_num());
	}

	print_results(LCS);

	printf("Main: program completed. Exiting.\n");
}

void init_array(File * fp)
{
int i = 0;
if(fp != NULL)
{
 char line [1000]; 
      while ( fgets ( line, sizeof line, file ) != NULL ) 
      {
		strcpy(File_Contents[i++], line);
      }
      fclose ( file );
	  }
	
}

void print_results(int LCS[])
{
int j;
  					// then print out the totals
  for ( int i = 0; i < ARRAY_SIZE - 1; i++ ) {
  j = i+1;
     printf(" %d & %d - %s\n", ),i,j, LCS[i]);
  }
}

void * find_longest_substring(int id)//id is 0,1,2,3
{
	int startPos, endPos;
	char local_LCS[ARRAY_SIZE/NUM_THREADS][STRING_SIZE];
	char substring[STRING_SIZE];
	int i,j,x,y,maxlen,len, currPos = 0;
		#pragma omp private(startPos, endPos, currPos, longestSS, i, j, x, y, maxlen, len)
		{
		startPos = id * (ARRAY_SIZE / NUM_THREADS);
		endPos = startPos + (ARRAY_SIZE / NUM_THREADS);
		if(id == 3)
		{
		   endPos--; //want to stop before currpos++ is out of bounds
		}
		
		for(currPos = startPos; currPos < endPos; currpos++)
		{
		for(i=0; i<strlen(File_Contents[currPos]); i++)
		{
			for(j=0; j<strlen(File_Contents[currPos+1]); j++)
			{
				if(File_Contents[id][i] == File_Contents[id2][j])
				{
				
					substring[0] = File_Contents[id][i];
					len = 1;
					x = i;
					y = j;
					while(File_Contents[id][++x] == File_Contents[id2][++y])
					{
						substring[len++] = File_Contents[id][x];
					}
					if(len>maxlen)
					{
						maxlen = len;
						strcpy(local_LCS[currPos], substring);
					}
				}
			}
		}
		}
				//put substring in global array
		#pragma omp critical
		{
			for(currPos = startPos; currPos < endPos; currpos++)
			{
			strcpy(LCS[currPos], local_LCS[currPos]);
			}
		}
		
		
		
		
		}


}
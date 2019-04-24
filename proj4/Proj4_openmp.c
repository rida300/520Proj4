#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ARRAY_SIZE 5
#define ARTICLE_SIZE 100
#define STRING_SIZE 10
#define NUM_THREADS 1

//char LCS[ARRAY_SIZE][STRING_SIZE];
char File_Contents[ARRAY_SIZE][ARTICLE_SIZE];
void * find_longest_substring(int, char **);
void init_array(FILE *);
void print_results();

int main() {

	FILE * fp = fopen ("testFile.txt", "r");
	char ** Lcs = malloc(sizeof(char)*ARRAY_SIZE*STRING_SIZE);
	omp_set_num_threads(NUM_THREADS);

	init_array(fp);
//	strcpy(File_Contents[0], "This is what I am doing instead of using the stupid fopen function");
//	strcpy(File_Contents[1], "This is how I am testing instead of using the stupid fopen function");
	#pragma omp parallel 
	{
		find_longest_substring(omp_get_thread_num(), Lcs);
	}
//	int id = 0;
//	find_longest_substring(&id);
	print_results(Lcs);

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
		strncpy(File_Contents[i], line, strlen(line)-1);

      		i++;
      }
      fclose ( fp );
	  }
	
}

void print_results(char ** Lcs)
{
int j;
  					// then print out the totals
  for ( int i = 0; i < ARRAY_SIZE - 1; i++ ) {
  j = i+1;
     printf(" %d & %d - %s\n",i,j, *(Lcs+i*STRING_SIZE));
  }
}

void *  find_longest_substring(int  id, char ** Lcs)//id is 0,1,2,3
{
	int tempCount = 0;
	int startPos, endPos;
	char local_LCS[(ARRAY_SIZE/NUM_THREADS)][STRING_SIZE];
	char * substring = malloc(sizeof(char)*STRING_SIZE);
	int i,j,x,y,maxlen,len, currPos = 0;
	int length1 = 0;
	int length2=0;
		#pragma omp private(id, startPos, endPos, currPos, local_LCS, i, j, x, y, maxlen, len, substring)
		{
		startPos = (id) * (ARRAY_SIZE / NUM_THREADS);
		endPos = startPos + (ARRAY_SIZE / NUM_THREADS);
		if(id == NUM_THREADS-1)
		{
		   endPos--; //want to stop before currpos++ is out of bounds
		}
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
					while(File_Contents[currPos][x] == File_Contents[currPos+1][y] && len < STRING_SIZE - 1 && x < length1 && y < length2)
					{
						x++;
						y++;
						len++;
					}
					if(len>maxlen && len < STRING_SIZE)
					{
						maxlen = len;
						strncpy(local_LCS[currPos], &File_Contents[currPos][i], len-1);
						tempCount++;
						printf("%d - %d - %s\n", tempCount, currPos, local_LCS[currPos]);
						//for(int q = 0; q < STRING_SIZE; q++){
						//	substring[q] = 0;}

					}
				}
			}
		    }

		}
		free(substring);

		//put substring in global array
		#pragma omp critical
		{
			for(currPos = 0; currPos < endPos+1; currPos++)
			{
			//printf("%s\n", local_LCS[currPos]); 
			strcpy(*(Lcs+currPos*STRING_SIZE), local_LCS[currPos]);
			//printf("%s\n", LCS[currPos]);
			}
		}
	//	free(local_LCS);

	}
}

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ARRAY_SIZE 600000
#define File_Size 1200000

#define NUM_THREADS 4

char LCS[ARRAY_SIZE][50];
char File_Contents[File_Size][1000];

main() {

	File fp = fopen ("wiki_dump.txt", "r");
	

	omp_set_num_threads(NUM_THREADS);

	init_arrays();

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
  for ( i = 0; i < ARRAY_SIZE; i++ ) {
  j = i+1;
     printf(" %d & %d - %s\n", ),i,j, LCS[i]);
  }
}

void * find_longest_substring(int id)
{
	int id2 = id +1;
		#pragma omp private()
		{
		
		
		
				#pragma omp critical
				{
				
				
				}

		
		
		
		
		}


}
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ARRAY_SIZE 1200000
#define ARTICLE_SIZE 1000
#define STRING_SIZE 100
#define NUM_THREADS 4

pthread_mutex_t mutexsum;// mutex for LCS

char LCS[ARRAY_SIZE][STRING_SIZE];
char File_Contents[ARRAY_SIZE][ARTICLE_SIZE];

main() {
	int i, rc;
	File * fp = fopen ("wiki_dump.txt", "r");
	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;
	void *status;


	/* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	init_arrays(fp);

	for (i = 0; i < NUM_THREADS; i++ ) {
	      rc = pthread_create(&threads[i], &attr, find_longest_substring, (void *)i);
	      if (rc) {
	        printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	      }
	}
/* Free attribute and wait for the other threads */
	pthread_attr_destroy(&attr);
	for(i=0; i<NUM_THREADS; i++) {
	     rc = pthread_join(threads[i], &status);
	     if (rc) {
		   printf("ERROR; return code from pthread_join() is %d\n", rc);
		   exit(-1);
	     }
	}

	pthread_mutex_destroy(&mutexsum);

	print_results(LCS);

	printf("Main: program completed. Exiting.\n");
	pthread_exit(NULL);

}

void init_array(File * fp)
{
int i = 0;
pthread_mutex_init(&mutexsum, NULL);

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
		pthread_mutex_lock (&mutexsum);
			for(currPos = startPos; currPos < endPos; currpos++)
			{
			strcpy(LCS[currPos], local_LCS[currPos]);
			}
		pthread_mutex_unlock (&mutexsum);
		pthread_exit(NULL);


}
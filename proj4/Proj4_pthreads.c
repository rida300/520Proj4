#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define ARRAY_SIZE  5
#define ARTICLE_SIZE 1000000
#define STRING_SIZE 1000000
static int NUM_THREADS;

pthread_mutex_t mutexsum;// mutex for LCS

char LCS [ARRAY_SIZE][STRING_SIZE];
char File_Contents[ARRAY_SIZE][ARTICLE_SIZE];
void * find_longest_substring(void * id);
void init_array(FILE *);
void print_results();

void array_init(char *** array, int row, int col) {
	int i;
	char * temp_array = (char *) malloc(row * col * sizeof(char));
	(* array) = (char **) malloc(row * sizeof(char *));

	for (i = 0; i < row; i++) {
		(*array)[i] = &(temp_array[i * col]);
	}
}

typedef struct {
  uint32_t virtualMem;
  uint32_t physicalMem;
} processMem_t;

/* Parses through a pointer or file line-by-line */
int parseLine(char *line) {
	// This assumes that a digit will be found and the line ends in " Kb".
	int i = strlen(line);
	const char *p = line;
	while (*p < '0' || *p > '9') p++;
	line[i - 3] = '\0';
	i = atoi(p);
	return i;
}// end parseLine

/* Gets the Process Memory */
void GetProcessMemory(processMem_t* processMem) {
	FILE *file = fopen("/proc/self/status", "r");
	char line[128];

	while (fgets(line, 128, file) != NULL) {
		
		if (strncmp(line, "VmSize:", 7) == 0) {
			processMem->virtualMem = parseLine(line);
		}

		if (strncmp(line, "VmRSS:", 6) == 0) {
			processMem->physicalMem = parseLine(line);
		}
	}
	fclose(file);
}

int main(int argc, char ** argv) {
	//ARRAY_SIZE = atoi(argv[1]);
	NUM_THREADS = atoi(argv[1]);
	//array_init(&LCS, ARRAY_SIZE-1, STRING_SIZE);
	//array_init(&File_Contents, ARRAY_SIZE, ARTICLE_SIZE);
	int myVersion = 2; //base = 1, pthreads = 2, openmp = 3, mpi = 4
	processMem_t myMem;
	struct timeval t1, t2;
	double elapsedTime;
	gettimeofday(&t1, NULL);
	int i= 0, rc;
	FILE * fp = fopen ("/homes/dan/625/wiki_dump.txt", "r");
	pthread_t * threads = malloc(sizeof(pthread_t) * NUM_THREADS);
	pthread_attr_t attr;
	void *status;


	/* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	init_array(fp);
	for (i = 0; i < NUM_THREADS; i++ ) {
	      rc = pthread_create(&threads[i], &attr, find_longest_substring, (void *)(intptr_t)i);
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
	gettimeofday(&t2, NULL);
	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
	GetProcessMemory(&myMem);
	printf("DATA, %d, %s, %f, %d, %d, %d\n", myVersion, getenv("NSLOTS"), elapsedTime, NUM_THREADS, myMem.virtualMem, myMem.physicalMem);
	printf("Main: program completed. Exiting.\n");
	pthread_exit(NULL);

}

void init_array(FILE * fp)
{
int i = 0;
pthread_mutex_init(&mutexsum, NULL);

if(fp != NULL)
{
 char line [ARTICLE_SIZE]; 
      while ( fgets ( line, sizeof line, fp ) != NULL && i < ARRAY_SIZE )
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

void * find_longest_substring(void * id)//id is 0,1,2,3
{
	int startPos, endPos;
	char local_LCS[ARRAY_SIZE/NUM_THREADS][STRING_SIZE];
	char substring[STRING_SIZE];
	int myid = (intptr_t)id;
	int i,j,x,y,maxlen,len,length1,length2, currPos = 0;
		startPos = myid * (ARRAY_SIZE / NUM_THREADS);
		endPos = startPos + (ARRAY_SIZE / NUM_THREADS);
		int comp = 0;
		for(currPos = startPos; currPos < endPos; currPos++)
		{
			maxlen = 0;
			length1 = strlen(File_Contents[currPos]);
			length2 = strlen(File_Contents[currPos+1]);
			for(i=0; i<= length1; i++)
			{
				for(j=0; j<= length2; j++)
				{
					if(File_Contents[currPos][i] == File_Contents[currPos+1][j])
					{
						substring[0] = File_Contents[currPos][i];
						len = 1;
						x = i+1;
						y = j+1;
						while(File_Contents[currPos][x] == File_Contents[currPos+1][y] && x<=length1 && y <= length2 && len < STRING_SIZE)
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
		}				//put substring in global array
			pthread_mutex_lock (&mutexsum);
			int z = 0;
			for(currPos = startPos; currPos < endPos; currPos++)
			{
			strcpy(LCS[currPos], local_LCS[z]);
			z++;
			}
		pthread_mutex_unlock (&mutexsum);
		pthread_exit(NULL);


}

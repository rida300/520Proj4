#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
int ARTICLE_SIZE =100;
int STRING_SIZE =15;

int  NUM_THREADS;
int ARRAY_SIZE;
pthread_mutex_t mutexsum;// mutex for LCS

typedef struct {
  uint32_t virtualMem;
  uint32_t physicalMem;
} processMem_t;

char ** LCS;//[ARRAY_SIZE][STRING_SIZE];
char ** File_Contents;//[ARRAY_SIZE][ARTICLE_SIZE];
void * find_longest_substring(void * id);
void init_array(FILE *);
void print_results();
void GetProcessMemory(processMem_t*);
int parseLine(char *); 
void array_init(char *** array, int row, int col);


int main(int argc, char ** argv) {
	  if (argc != 3)
	{
		printf ("%s is not a valid inputs\n", argv[0]);
		return -1;
	}
	int myVersion = 2; //base = 1, pthreads = 2, openmp = 3, mpi = 4
	printf("im in");
	ARRAY_SIZE = atoi(argv[1]);
	printf("%d", ARRAY_SIZE);
	NUM_THREADS = atoi(argv[2]);
	printf("%d, %d", ARRAY_SIZE, NUM_THREADS);
	fflush(stdout);
	struct timeval t1, t2;
	double elapsedTime;
	processMem_t procMem;
	gettimeofday(&t1, NULL);
	array_init(&LCS, ARRAY_SIZE, STRING_SIZE);
	//LCS = (char *)malloc(ARRAY_SIZE*STRING_SIZE*sizeof(char));
	if(LCS == NULL)
		exit(0);
	//File_Contents = (char *) malloc(ARRAY_SIZE *ARTICLE_SIZE*sizeof(char));
	array_init(&File_Contents, ARRAY_SIZE, ARTICLE_SIZE);
	if(File_Contents == NULL)
		exit(0);

	printf("i wanna die");
	fflush(stdout);
	int i= 0, rc;
	FILE * fp = fopen ("testFile.txt", "r");
	pthread_t *threads= malloc(sizeof(pthread_t)*NUM_THREADS);
	pthread_attr_t attr;
	void *status;


	/* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
//	exit(0);
	init_array(fp);
//	exit(0);
	for (i = 0; i < NUM_THREADS; i++ ) {
	      rc = pthread_create(&threads[i], &attr, find_longest_substring, (void *)(intptr_t)i);
		if (rc) {
	        printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	      }
	}
free(File_Contents);

fflush(stdout);
/* Free attribute and wait for the other threads */
	pthread_attr_destroy(&attr);
	for(i=0; i<NUM_THREADS; i++) {
	     rc = pthread_join(threads[i], &status);
	     if (rc) {
		   printf("ERROR; return code from pthread_join() is %d\n", rc);
		   exit(-1);
	     }
//	printf("%d", i);
//	fflush(stdout);
	}

	pthread_mutex_destroy(&mutexsum);
printf("destroyed threads");
fflush(stdout);
	print_results(LCS);

	gettimeofday(&t2, NULL);
	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
	GetProcessMemory(&procMem);
	printf("DATA, %d, %s, %f, %d, %u, %u\n", myVersion, getenv("NSLOTS"), elapsedTime, NUM_THREADS, procMem.virtualMem, procMem.physicalMem);
	printf("Main: program completed. Exiting.\n");
	pthread_exit(NULL);
	return(0);
}


void array_init(char *** array, int row, int col) {
	int i;
	char * temp_array = (char *) malloc(row * col * sizeof(char));
	(* array) = (char **) malloc(row * sizeof(char *));

	for (i = 0; i < row; i++) {
		(*array)[i] = &(temp_array[i * col]);
	}
}

int parseLine(char *line) {
	// This assumes that a digit will be found and the line ends in " Kb".
	int i = strlen(line);
	const char *p = line;
	while (*p < '0' || *p > '9') p++;
	line[i - 3] = '\0';
	i = atoi(p);
	return i;
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
		strcpy(File_Contents[i], line);
//		printf("%d\n", i);
//		fflush(stdout);
		i++;
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
     fflush(stdout);
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
		}
				//put substring in global array
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

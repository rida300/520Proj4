#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <math.h>
#define ARTICLE_SIZE 2200



int NUM_THREADS;
char** File_Contents;
int Lines_Read;



void find_longest_substring(char* X, char* Y, int m, int n,int l1,int l2)
{
    
	int **local_LCS = malloc((m+1) * sizeof(int *));;
	for(int k=0; k<(m+1);k++)
		local_LCS[k] =(int*)malloc((n+1)*sizeof(int));
    
    int len = 0;
 
    int row, col;
 
   for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            if (i == 0 || j == 0)
                local_LCS[i][j] = 0;
        
	    else if (X[i - 1] == Y[j - 1]) {
                local_LCS[i][j] = local_LCS[i - 1][j - 1] + 1;
                if (len < local_LCS[i][j]) {
                    len = local_LCS[i][j];
                    row = i;
                    col = j;
                }
            } else
                local_LCS[i][j] = 0;
        }
    }
 
    if (len == 0) {
        printf("%d-%d: No Common Substring\n",l1,l2);
        return;
    }
 
    char* resultingArr = (char*)malloc((len + 1) * sizeof(char));
    resultingArr[len] = '\0';
 
   while (local_LCS[row][col] != 0) {
        resultingArr[--len] = X[row - 1]; 
        row--;
        col--;
    }
	undo_Malloc(local_LCS, m);
    
	size_t length;
    if((length = strlen(resultingArr)) >0){
		if(resultingArr[length-1] == '\n')
			resultingArr[length-1] ='\0';
	 }
	
   printf("%d-%d: %s\n",l1,l2,resultingArr);
   free(resultingArr);
   
}

void LCS_intermediate(int id){
	int s1,s2;
	int startPos =  id * (Lines_Read / NUM_THREADS);
	int endPos = startPos +( Lines_Read / NUM_THREADS);	
	int line1 = startPos;
	int line2 = line1 + 1;
	int i;
	
	for( i =startPos; (i < endPos) &&(i+1 < Lines_Read); i++){
		s1 = strlen(File_Contents[i]);
		s2 = strlen(File_Contents[i+1]);
			
		find_longest_substring(File_Contents[i], File_Contents[i+1], s1, s2, line1, line2);
	
		line1 = line2;
		line2++;
	}
}

int main(int argc, char *argv[])
{
	
	struct timeval t1, t2;
	double elapsedTime;
	int myVersion = 1;
	

	int i;	
	gettimeofday(&t1, NULL);
								

	
	FILE *fp;
	int problem_size;
	char* filename;
	
		problem_size = strtol(argv[2], NULL, 10);
	

	
    fp = fopen("testLorem.txt", "r");
    if (fp == NULL){
        printf("Could not open file: %s\n",filename);
		printf("Usage: ./program <file> | <problem size>\n");
        return 1;
    }
    
	int ch = 0;
	while(!feof(fp))
	{
	  ch = fgetc(fp);
	  if(ch == '\n')
	  {
		Lines_Read++;
	  }
	}
	if((argc > 2) && (problem_size < Lines_Read)){
		Lines_Read = problem_size;
	}
	rewind(fp);

	File_Contents = malloc(sizeof(char*) * Lines_Read);	
	
	for(i =0; i < Lines_Read; i++){
		if(ferror(fp) || feof(fp)){
			break;			
		}
		File_Contents[i] = malloc(sizeof(char) * ARTICLE_SIZE);
		fgets(File_Contents[i], ARTICLE_SIZE, fp);
	
	}	
	int rc;
	int numtasks, rank;
	
	MPI_Status Status;
	
	rc = MPI_Init(&argc,&argv);
	if (rc != MPI_SUCCESS) {
	  printf ("Error starting MPI program. Terminating.\n");
          MPI_Abort(MPI_COMM_WORLD, rc);
        }
		
        MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	NUM_THREADS = numtasks;
	
		
	LCS_intermediate(rank);

	
	

	MPI_Finalize();
	if (rank == 0){
		gettimeofday(&t2, NULL);
		elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; 
		elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; 
		printf("DATA, %d, %s, %f\n", myVersion, getenv("SLURM_CPUS_ON_NODE"),  elapsedTime);
	}
	
	
	undo_Malloc((int **)File_Contents,Lines_Read);

    fclose(fp);
	

  return 0;
}


void undo_Malloc(int **arr, int length){
	for (int i = 0; i<length;i++){
	 free(arr[i]);
	}
	free(arr);
}

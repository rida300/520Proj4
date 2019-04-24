#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define ARRAY_SIZE 1000000
#define STRING_SIZE 1000


char File_Contents[ARRAY_SIZE][STRING_SIZE];

void print_results(char x[], int i, int j, char **b, int *iptr, char *r);
void lcs(char x[], char y[], char **b, int **c);
//void * find_longest_substring(int  id);
//void init_array(FILE *);
//void print_results();

int main(int argc, const char ** argv) {


	const int INPUT_SIZE = atoi(argv[2]);
	const int NUM_OF_THREADS = atoi(argv[3]);
	FILE * fp = fopen(argv[1], "r");
	//omp_set_num_threads(NUM_THREADS);
	int i = 0;
	if (fp != NULL)
	{
		char line[STRING_SIZE];
		while (fgets(line, STRING_SIZE, fp) != NULL && i < INPUT_SIZE)
		{
			//strncpy(File_Contents[i++], line, strlen(line)-1);
			strcpy(File_Contents[i++], line);

		}
		fclose(fp);
		if (i < 0)
			return -1;
	}

	const int RESULTS_SIZE = i - 1;//one less job than the number of lines
	const int LINES_PER_THREAD = i % NUM_OF_THREADS == 0 ? i / NUM_OF_THREADS : i / NUM_OF_THREADS + 1;

	int result_array_sizes[RESULTS_SIZE];

	//allocate memory for results array

	char **results = (char **)malloc(RESULTS_SIZE * sizeof(char *));//because this is also a 2d array
	for (int i = 0; i < RESULTS_SIZE; i++)
	{
		results[i] = (char *)malloc(STRING_SIZE * sizeof(char));
	}

	omp_set_num_threads(NUM_OF_THREADS);//relocated

#pragma omp parallel 
	{
		//find_longest_substring(omp_get_thread_num());
		int o;
		int thread_id = omp_get_thread_num();
		int start_index = thread_id * LINES_PER_THREAD;
		int end_index = start_index + LINES_PER_THREAD;
		thread_id++;

		o = start_index;
		while (o < end_index && o < i - 1)
		{
			int j, m, n, k, index;
			int xlen = strlen(File_Contents[o]);//first line
			int ylen = strlen(File_Contents[o + 1]);//next line

			//allocate memory for these 2 lines to store them separately
			char *x = (char *)malloc((xlen + 1) * sizeof(char)); 			// first string
			char *y = (char *)malloc((ylen + 1) * sizeof(char));			// second string


			char **b = (char **)malloc((xlen + 1) * sizeof(char *));
			int **c = (int **)malloc((xlen + 1) * sizeof(int *));

			// initialize b and c
			for (j = 0; j <= xlen; j++)
			{
				b[j] = (char *)malloc((ylen + 1) * sizeof(char));
				c[j] = (int *)malloc((ylen + 1) * sizeof(int));
			}

			strncpy(x, File_Contents[o], xlen + 1);//strlen does not include the null character but it does count the enwline character 
			strncpy(y, File_Contents[o + 1], ylen + 1);

			lcs(x, y, b, c);

			// get lcs lengths
			m = strlen(x);
			n = strlen(y);

			index = 0;
			print_results(x, m, n, b, &index, results[o]);

			// save the size of the results string
			result_array_sizes[o] = index;

			for (k = 0; k < xlen; k++)
			{
				free(b[k]);
				free(c[k]);
			}

			free(b);
			free(c);
			free(x);
			free(y);
			o++;
		}
	}
	for (i = 0; i < RESULTS_SIZE; i++)
	{
		printf("\tLines %d-%d:\t", i + 1, i + 2);
		for (int s = 0; s < result_array_sizes[i]; s++)
			printf("%c", results[i][s]);
		printf("\n");
	}


	for (i = 0; i < RESULTS_SIZE; i++)
		free(results[i]);
	free(results);

	printf("Main: program completed. Exiting.\n");
	return 0;
}
void print_results(char x[], int i, int j, char **b, int *iptr, char *r)
{
	if (i == 0 || j == 0)
		return;
	if (b[i][j] == 'm')
	{
		print_results(x, i - 1, j - 1, b, iptr, r);
		r[(*iptr)++] = x[i - 1];
	}
	else if (b[i][j] == 'u')
		print_results(x, i - 1, j, b, iptr, r);
	else
		print_results(x, i, j - 1, b, iptr, r);
}



void lcs(char x[], char y[], char **b, int **c)
{
	int i, j, m, n;

	m = strlen(x);
	n = strlen(y);

	for (i = 0; i <= m; i++)
		c[i][0] = 0;//initialize first column
	for (i = 0; i <= n; i++)
		c[0][i] = 0;//initialize first row

		//dynamic programming approach
	for (i = 1; i <= m; i++)
	{
		for (j = 1; j <= n; j++)
		{
      //diagonally same so the previous character matched
			if (x[i - 1] == y[j - 1])
			{
				c[i][j] = c[i - 1][j - 1] + 1;
				b[i][j] = 'm';//matched
			}
      //finds the maximum of the cell one row or one column below the current cell
			else if (c[i - 1][j] >= c[i][j - 1])
			{
				c[i][j] = c[i - 1][j];
				b[i][j] = 'u';//unmatched
			}
      //cell in previous column but same row had a bigger value than current cell 
			else
			{
				c[i][j] = c[i][j - 1];
				b[i][j] = 'l';
			}
		}
	}
}

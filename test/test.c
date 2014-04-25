#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "elr_mpl.h"
#include "time.h"
#include "cunit.h"

unsigned long my_clock()
{
     __asm RDTSC
}


int  test_initialzer();

int  test_destory();

int  test_tree_destory();

int  test_mem_alloc();

/* generate memory fragments */
char *fragment_stack[100000];
void make_fragments(int mem_size);
void clear_fragments();

/* test memory allocation, freeing, access speed */
void bench();

/* test memory allocation, freeing, access speed of elr_memory_pool */
void mpl_alloc_free_access(size_t alloc_size,
						   int *alloc_times,
						   unsigned long *alloc_clocks,
						   int *free_times,
						   unsigned long *free_clocks,
						   int *access_times,
						   unsigned long *access_clocks);

/* test memory allocation, freeing, access speed of C standard library */
void clib_alloc_free_access(size_t alloc_size,
						   int *alloc_times,
						   unsigned long *alloc_clocks,
						   int *free_times,
						   unsigned long *free_clocks,
						   int *access_times,
						   unsigned long *access_clocks);

/* For machine with multi core CPU or multi CPUs,*/
/* this test program should bind to a core.*/
int main()
{
	elr_mpl_init();

	RUN_TEST_BOOLEAN(test_initialzer,"ELR_MPL_INITIALIZER is invalid pool.");
	RUN_TEST_BOOLEAN(test_destory,"When a pool been destoryed, return value of elr_mpl_avail should be zero.");
    RUN_TEST_BOOLEAN(test_tree_destory,"When a pool been destoryed, its child pool should be also destoryed.");
    RUN_TEST_BOOLEAN(test_mem_alloc,"Allocate memory of the same size be declared.");

	getchar();

	bench();

	elr_mpl_finalize();
	getchar();
	return 0;
}

int  test_initialzer()
{
	elr_mpl_t pool = ELR_MPL_INITIALIZER;

	return (elr_mpl_avail(&pool) == 0);
}

int  test_destory()
{
	elr_mpl_t pool = elr_mpl_create(NULL,256);
	elr_mpl_destroy(&pool);
	return (elr_mpl_avail(&pool) == 0);
}

int  test_mem_alloc()
{
	int i = 0;
	void* p[128] = {NULL}; 
	elr_mpl_t pool = elr_mpl_create(NULL,256);

	srand((unsigned)time(NULL)); 

	while(i < 128)
	{
		p[i] = elr_mpl_alloc(&pool);

		if(p[i] != NULL)
			memset(p[i],0,256);
		i++;
	}

	do
	{
		i--;
		elr_mpl_free(p[i]);
		//if(rand()%3 == 0)
		//	elr_mpl_alloc(&pool);
	}
	while(i > 0);

	return 1;
}

int  test_tree_destory()
{
	elr_mpl_t parent_pool = elr_mpl_create(NULL,256);
	elr_mpl_t child_pool = elr_mpl_create(&parent_pool,256);
	elr_mpl_destroy(&parent_pool);

	return (elr_mpl_avail(&child_pool) == 0);
}

void clear_fragments()
{
	int j = 0;
	for (j = 0; j < 100000; j++)
	{
		if(fragment_stack[j] != NULL)
			free(fragment_stack[j]);
	}
}

void make_fragments(int mem_size)
{
	int   sm32 = mem_size-32+1;
	int   alloc_size = 32;
	int   i = 0,j = 0;

	memset(fragment_stack,0,100000*sizeof(char*));
	srand((unsigned)time(NULL)); 

	for (i = 0; i < 100000; i++)
	{
		alloc_size = rand()%sm32+32;
		fragment_stack[i] = (char*)malloc(alloc_size);
		if(fragment_stack[i] != NULL)
		{
			*(fragment_stack[i]+(alloc_size-1)) = 0;
		}

		if(i%10 == 0)
		{
			j = rand()%100000;
			if(j >= 0 && j <= i && fragment_stack[j] != NULL)
			{
				free(fragment_stack[j]);
				fragment_stack[j] = NULL;
			}
		}
	}	

	return;
}

void bench()
{
	int i = 0;
	int test_size[11] = 
	{16,32,64,128,256,512,1024,2048,4096,8192,16384};
	int alloc_times = 0;
	unsigned long alloc_clocks = 0;
	int free_times = 0;
	unsigned long free_clocks = 0;
	int access_times = 0;
	unsigned long access_clocks = 0;
	float  alloc_time;
	float  free_time;
	float  access_time;

	printf("\nallocation, freeing, access time consumption of elr_memory_pool.\n");
	printf("|size           |alloc          |free           |access         |\n");

	for (i = 0; i < 11; i++)
	{
		alloc_times = 0;
		alloc_clocks = 0;
		free_times = 0;
		free_clocks = 0;
		access_times = 0;
		access_clocks = 0;

		mpl_alloc_free_access(test_size[i],
			&alloc_times,
			&alloc_clocks,
			&free_times,
			&free_clocks,
			&access_times,
			&access_clocks);

		alloc_time = (float)alloc_clocks/(float)alloc_times;
		free_time = (float)free_clocks/(float)free_times;
		access_time = (float)access_clocks/(float)access_times;

		printf("|%-15d|%-15d|%-15d|%-15d|\n",test_size[i],alloc_times,free_times,access_times);
		printf("|%-15d|%-15.3f|%-15.3f|%-15.3f|\n",test_size[i],alloc_time,free_time,access_time);
	}

	//puts("\nMaking memory fragments. Please wait.\n");
	//make_fragments(1048576);

	printf("\nallocation, freeing, access time consumption of C stdlib.\n");
	printf("|size           |alloc          |free           |access         |\n");

	for (i = 0; i < 11; i++)
	{
		alloc_times = 0;
		alloc_clocks = 0;
		free_times = 0;
		free_clocks = 0;
		access_times = 0;
		access_clocks = 0;

		clib_alloc_free_access(test_size[i],
			&alloc_times,
			&alloc_clocks,
			&free_times,
			&free_clocks,
			&access_times,
			&access_clocks);

		alloc_time = (float)alloc_clocks/(float)alloc_times;
		free_time = (float)free_clocks/(float)free_times;
		access_time = (float)access_clocks/(float)access_times;

		printf("|%-15d|%-15d|%-15d|%-15d|\n",test_size[i],alloc_times,free_times,access_times);
		printf("|%-15d|%-15.3f|%-15.3f|%-15.3f|\n",test_size[i],alloc_time,free_time,access_time);
	}
	//clear_fragments();
}

/* alloc_size  the memory block size for allocating, access, freeing.*/
/* alloc_times total times of  memory allocation operation. Need to be initialized to zero. */
/* alloc_clocks total time consumption of  memory allocation operations. Need to be initialized to zero. */
/* other parameters ditto.*/

void mpl_alloc_free_access(size_t alloc_size,
						   int *alloc_times,
						   unsigned long *alloc_clocks,
						   int *free_times,
						   unsigned long *free_clocks,
						   int *access_times,
						   unsigned long *access_clocks)
{
	int i = 0, j = 0, ri = 0;
	unsigned long alloc_clks;
	unsigned long free_clks;
	unsigned long access_clks;
	char *mem_stack[1000];
	char  stub =  0;

	elr_mpl_t pool = elr_mpl_create(NULL,alloc_size);

	if(elr_mpl_avail(&pool) != 0)
	{
		srand((unsigned)time(NULL)); 
		for (j = 0; j < 1000; j++)
		{
			ri = rand()%100;
			if(ri < 50)
			{
				alloc_clks = my_clock();
				mem_stack[i] = (char*)elr_mpl_alloc(&pool);
				*alloc_clocks += (my_clock()-alloc_clks);
				(*alloc_times)++;				

				if(mem_stack[i] != NULL)
				{
					stub =  0;
					access_clks = my_clock();
					*(mem_stack[i]+(alloc_size-1)) = 0;
					*access_clocks += (my_clock()-access_clks);
					(*access_times)++;
				}

				i++;
			}
			else
			{
				if (i > 0)
				{
					i--;
					free_clks = my_clock();
					elr_mpl_free(mem_stack[i]);
					*free_clocks += (my_clock()-free_clks);
					(*free_times)++;
				}
			}
		}

		elr_mpl_destroy(&pool);
	}
}

void clib_alloc_free_access(size_t alloc_size,
						   int *alloc_times,
						   unsigned long *alloc_clocks,
						   int *free_times,
						   unsigned long *free_clocks,
						   int *access_times,
						   unsigned long *access_clocks)
{
	int i = 0, j = 0, ri = 0;
	unsigned long alloc_clks;
	unsigned long free_clks;
	unsigned long access_clks;
	char *mem_stack[1000];
	char  stub =  0;

	memset(mem_stack,0,1000*sizeof(char*));

	srand((unsigned)time(NULL)); 
	for (j = 0; j < 1000; j++)
	{
		ri = rand()%100;
		if(ri < 50)
		{
			alloc_clks = my_clock();
			mem_stack[i] = (char*)malloc(alloc_size);
			*alloc_clocks += (my_clock()-alloc_clks);
			(*alloc_times)++;				

			if(mem_stack[i] != NULL)
			{
				stub =  0;
				access_clks = my_clock();
				*(mem_stack[i]+(alloc_size-1)) = 0;
				*access_clocks += (my_clock()-access_clks);
				(*access_times)++;
			}

			i++;
		}
		else
		{
			if (i > 0)
			{
				i--;
				free_clks = my_clock();
				free(mem_stack[i]);
				*free_clocks += (my_clock()-free_clks);
				mem_stack[i] = NULL;
				(*free_times)++;
			}
		}
	}

	for (j = 0; j < 1000; j++)
	{
		if(mem_stack[j] != NULL)
			free(mem_stack[j]);
	}
}


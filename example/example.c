#include <stdio.h>
#include <stdlib.h>
#include "elr_mpl.h"

int main()
{
	elr_mpl_t mypool = ELR_MPL_INITIALIZER;
	elr_mpl_t mysubpool = ELR_MPL_INITIALIZER;
    void*  mem = NULL; 
	size_t len = 0;

	elr_mpl_init();	

	mypool = elr_mpl_create(NULL,256);
	printf("%s\n","create a memory pool: mypool.");

	mysubpool = elr_mpl_create(&mypool,128);
	printf("%s\n","create a sub memory pool of mypool, name is mysubpool.");

	mem = elr_mpl_alloc(&mysubpool);
	printf("%s\n","alloc a memory block form mysubpool.");

	len = elr_mpl_size(mem);
	printf("the memory block size is %d.\n",len);

	elr_mpl_free(mem);
	printf("give back the memory block to mysubpool.\n",len);

	mem = elr_mpl_alloc(&mypool);
	printf("%s\n","alloc a memory block form mypool.");

	len = elr_mpl_size(mem);
	printf("the memory block size is %d.\n",len);

	elr_mpl_free(mem);
	printf("give back the memory block to mypool.\n",len);

	elr_mpl_destroy(&mypool);
	printf("destroy mypool.\n",len);

	printf("when mypool has destoryed, it`s sub pool, mysubpool, did %s destoryed.\n",
		elr_mpl_avail(&mysubpool) == 0?"also":"not");

	elr_mpl_finalize();

	getchar();
	return 0;
}


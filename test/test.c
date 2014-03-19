#include "elr_mpl.h"
#include <stdlib.h>

int main()
{
	elr_mpl_t pool1 = ELR_MPL_INITIALIZER;
	elr_mpl_t pool2 = ELR_MPL_INITIALIZER;
	elr_mpl_t pool3 = ELR_MPL_INITIALIZER;
	int i = 0;
	void  *p;
	elr_mpl_init();

	pool1 = elr_mpl_create(NULL,256);

	pool2 = elr_mpl_create(&pool1,256);

	for (i = 0 ;  i < 62; i++)
	{
		elr_mpl_create(NULL,128);
	}	

	for (i = 0; i < 100; i++)
	{
		p = elr_mpl_alloc(&pool1);
	}

	elr_mpl_free(p);

	elr_mpl_destroy(&pool1);
	pool3 = elr_mpl_create(NULL,256);
	pool3 = elr_mpl_create(NULL,256);
	elr_mpl_destroy(&pool2);
	elr_mpl_destroy(&pool3);

	elr_mpl_finalize();
	return 0;
}


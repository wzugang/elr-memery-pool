# introduce #

this is a high performance, flexible, cross-platform memory pool published under MIT Licence. it is  free for personal or commercial use and had been used in manay production environment.



memory is managed with node and slice of the memory pool. node is big memory block, slice is small memory block from node. each memory block allocated from this memory pool is belong to a slice.



all slices from a memory pool is of the same size. so this memory pool is more like a object pool. but we can implement a more flexible memory pool by use many memory pool instance of different slice size.



node is linked as list, available slice also linked as list. when allocate memory form the memory pool. it first check whether a free slice is exist. if exist take one. if not, check whether the newly allocated node has a never used slice. if has, take a slice form the node. if does not, move the node to the head of node list and allocate a node and take a slice from it. when free a memory block, just move it to the head of available slice list.



this memory pool is organized in tree structure. when create a memory pool, we can specify its parent memory pool by use a pointer to the parent pool as the first parameter for _elr\_mpl\_create_ function. when a memory pool is destroyed its child memory pool also be destroyed, so we do not need destroy the memory pool and its child memory pool when we do not need those memory pools any more, just destory the parent memory pool. if we do not specify the parent memory pool when create a memory pool, the memory pools parent memory pool is the global memory pool. it is created at the first time _elr\_mpl\_init_ be invoked, and be destoryed at the last time _elr\_mpl\_finalize_ be invoked. all memory usage of memory pools' control structure is a allocated from the global memory pool. we can see that in this memory pool realization all memory pool instance is the child or sub-child of the global memory pool. this means all memory pools will be destroyed when the last time _elr\_mpl\_finalize_ been called. this reduce the chance of memory leak to the minimum degree.




this memory pool also support muti-threading using. if we want it works in muti-theading entironment, we must implement all the six interface defined in file elr\_mtx.h and define **ELR\_USE\_THREAD**. fortunately, it is a very easy job, and the implementation of windows platform is already provided.



when i make the windows implementation, i take linux into consideration. so the **atomic counter(interger) type** and **counter value type** is defined separately. under windows there are not a atomic type, just a LONG illuminated by volatile. we can make assignment between LONG and volatile LONG. but under linux **atomic counter(interger) type** is defined as follow. `typedef struct  {  volatile int counter;  }  atomic_t; `. assignment between `int` and `atomic_t` is against the C language syntax.



this memory pool had been used in manay production environment and its ability had been proved. event so, there is still plenty of space for improvement. now each memory pool has its own mutex, under many circumstance this is not a must. so there are at least two points need to be improved. first, reduce the mutex consume. second, make it to be a all-purpose memory pool, like appache memory pool, many regular size of memory can be be allocated from it.

# usage #
```
#include <stdio.h>
#include <stdlib.h>
#include "elr_mpl.h"

int main()
{
    elr_mpl_t mypool = ELR_MPL_INITIALIZER;
    elr_mpl_t mysubpool = ELR_MPL_INITIALIZER;
    void*  mem = NULL; 
    int    len = 0;

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
```
#ifndef __CUNIT_H__
#define __CUNIT_H__

#include "stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RUN_TEST_BOOLEAN(func, msg)  \
	do\
	{\
	    if(func())\
	    {\
		    printf("Passed ------ Running %s: %s\n",#func,msg);\
        }\
		else\
	    {\
		    printf("Failed ------ Running %s: %s\n",#func,msg);\
        }\
    }\
    while (0)


#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif

#endif
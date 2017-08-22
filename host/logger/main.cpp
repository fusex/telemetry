/*
* =====================================================================================
*
*       Filename:  logger.cpp
*
*    Description:  
*
*        Version:  1.0
*        Created:  05/15/2017 11:38:33 AM
*       Revision:  none
*       Compiler:  gcc
*
*
* =====================================================================================
*/
#include <unistd.h>
#include "logger.h"

int main(int argc, char** argv)
{
    logger* l = new logger("/tmp/log1");
    unsigned int count = 0;

    while (count<4000000) {
        char string[] = "test";
        l->lprintf("%d %s\n", count++, string);
        sleep(0.1);
    }

    delete l;
    return EXIT_SUCCESS;
}

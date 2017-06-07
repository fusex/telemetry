/*
* =====================================================================================
*
*       Filename:  main.c
*
*    Description:  
*
*        Version:  1.0
*        Created:  02/06/2017 01:58:44
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Zakaria ElQotbi (zakariae), zakaria.elqotbi@derbsellicon.com
*        Company:  Derb.io 
*
* =====================================================================================
*/
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "trame.h"

int main(int argc,char** argv)
{
    FILE*   in;
    uint8_t buf[512]; 
    size_t  rb=0;

    if(argc<2){
	printf("please provide file\n");
	exit(1);
    }
    in = fopen(argv[1],"ro");

    while(fread(buf,1,512,in)) {
	fxtm_data_t* tm   = (fxtm_data_t*) buf;
	int32_t accel[3] = {0,0,0};
	int32_t magn[3]  = {0,0,0};
	int32_t gyr[3]   = {0,0,0};

	printf("ts: %u gps: %f,%f\n",tm->timestamp,(float)tm->gpslt/10000,(float)tm->gpslg/10000);

	GETT(accel, tm, accel[0], accel[1], accel[2]);
	GETT(magn,  tm, magn[0],  magn[1],  magn[2]);
	GETT(gyr,   tm, gyr[0],   gyr[1],   gyr[2]);

	printf("\taccel[0]: %6d, accel[1]: %6d, accel[2]: %6d\n", accel[0], accel[1], accel[2]);
	printf("\t magn[0]: %6d,  magn[1]: %6d,  magn[2]: %6d\n", magn[0], magn[1], magn[2]);
	printf("\t  gyr[0]: %6d,   gyr[1]: %6d,   gyr[2]: %6d\n", gyr[0], gyr[1], gyr[2]);
   
	rb +=512;
    }
    printf("%ld bytes readed\n",rb);

    fclose(in);
}

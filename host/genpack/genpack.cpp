/*
 * =====================================================================================
 *
 *       Filename:  fxhost.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/06/2017 00:34:59
 *       Revision:  none
 *       Compiler:  gcc
 *
*         Author:  Zakaria ElQotbi (zakariae), zakaria.elqotbi@derbsellicon.com
*        Company:  Derb.io 
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h> 
#include <signal.h>
#include <time.h>

#include "trame.h"

#define MAX_PRESSURE_AT_SEALEVEL 1013

void do_usage (char** argv)
{
    printf ("\tUsage: %s [-h] outputfile [NR] \n",argv[0]);
    exit(0);
}

int main (int argc, char** argv)
{
    FILE*          file = NULL;
    int32_t        nr_packets = 1;
    uint32_t       wb = 0;
    const uint32_t bs = fxtm_getblocksize();

    if (argc < 2) {
	printf("please provide file\n");
	exit(1);
    }
    if (!strncmp("-h",argv[1],2))
 	do_usage(argv);	
    if (argc > 1)
	file = fopen(argv[1],"wo"); 
    if (argc>2)
    	nr_packets = strtol(argv[2],NULL,10);

    if (!file) {
        printf("Error opening %s: %s\n", argv[1], strerror(errno));
	exit(-1);
    }

    for (int32_t i=0; i<nr_packets; i++) {
	uint8_t*     buf = (uint8_t*) fxtm_getblock();
	fxtm_data_t* tm  = fxtm_getdata(); 

        //memset(buf,0,bs);
	
        float x = (float)((i*10)+i)/10;
	float y = (float)((i*10)-i)/10;
	float z = (float)i;

	float accel[3] = {x,y,z};
	float magn[3]  = {y,x,z};
	float gyr[3]   = {z,y,x};
	float accel2[3]= {y,z,x};

	tm->id = i;

	fxtm_setimu(accel, magn, gyr); 
	fxtm_setimu2(accel2); 

	fxtm_settemperature(i%MAX_TEMPERATURE);
	fxtm_setpressure(MAX_PRESSURE_AT_SEALEVEL - i);
	fxtm_setdiffpressure(MAX_PRESSURE_AT_SEALEVEL + i);
	fxtm_sethumidity(i%100);
	fxtm_setflightstatus(i%(FXTM_FLIGHTSTATUS_TOUCHDOWN+1));

	fxtm_setgps(x, y);

	fxtm_dump(NULL);

	if (fwrite(buf, bs, 1, file) == 0) {
	    printf("Error writing in %s %s\n", argv[1], strerror(errno));
	    break;
	}
	wb += bs;
	usleep(1000); // a tempo to make the timestamp progress
    }

    printf("%d bytes writed\n", wb);

    fclose(file);
}

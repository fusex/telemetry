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
//#include <assert.h>

#include <time.h>

#include "fusexutil.h"
#include "trame.h"
#include "logger.h"

size_t readsize = 0;

int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void set_mincount(int fd, int mcount)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error tcgetattr: %s\n", strerror(errno));
        return;
    }

    tty.c_cc[VMIN] = mcount ? 1 : 0;
    tty.c_cc[VTIME] = 5;        /* half second timer */

    if (tcsetattr(fd, TCSANOW, &tty) < 0)
        printf("Error tcsetattr: %s\n", strerror(errno));
}

int openregular(int argc,char** argv)
{
    int fd;
    if(argc<3){
	printf("please provide file\n");
	exit(1);
    }
    fd = open(argv[2], O_RDONLY);
    if (fd < 0) {
        printf("Error opening %s: %s\n", argv[1], strerror(errno));
        return -1;
    }

    readsize = fxtm_getblocksize();

    return fd;
}

#define SERIALBAUD B115200

int openserial(int argc, char** argv)
{
    int  fd;
    const char *portname = "/dev/ttyACM1";
    if(argc > 1)
	portname = argv[1];

    fd = open(portname, O_RDONLY | O_NOCTTY);
    if (fd < 0) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }
    set_interface_attribs(fd, SERIALBAUD);

    readsize = fxtm_getdatasize();

    return fd;
}

void do_usage(char** argv)
{
    printf ("\tUsage: %s [-l] file\n",argv[0]);
    exit(-2);
}

#if 1
# define HEX_DUMP   0
# define TRAME_DUMP 1
#elif 0
# define HEX_DUMP   1
# define TRAME_DUMP 0
#else
# define HEX_DUMP   0
# define TRAME_DUMP 0
#endif

int getlogfile(char* filename)
{
    struct tm *t;
    time_t now = time(NULL);
    t = gmtime(&now);
    strftime(filename, 128, "fusexlog-%Y-%m-%d-%H-%M-%S", t);
}

void thread_acquisition(int fd, logger* l)
{
    size_t rb = 0;
    int    finish = 0;
    FILE*  file = fdopen(fd,"ro");
    do {
        int rdlen;

        rdlen = fread(fxtm_getdata(), 1, readsize, file);
        if (rdlen > 0) {
	    l->wlog((uint8_t*)fxtm_getdata(),fxtm_getdatasize());
        } else if (rdlen < 0) {
            printf("Error from read: %d: %s\n", rdlen, strerror(errno));
	    finish = 1;
        }

#if 0
/* ZSK DEBUG TOREMOVE */
	printf("rdlen: %d expected:%ld\n", rdlen, fxtm_getdatasize());
/* ZSK END*/
#endif
	//rb += fxtm_getblocksize();
	rb += readsize;
    } while (!finish);
}

void thread_dumper(logger* l)
{
    uint8_t buf[512];
    int    finish = 0;
    do {
	int rd = l->rlog(buf, fxtm_getdatasize());
	if(rd == fxtm_getdatasize()) {
#if HEX_DUMP
	    unsigned char *p;
	    rdlen = rd;
	    for (p = buf; rdlen-- > 0; p++)
		printf(" 0x%x", *p);
	    printf("\n");
#elif TRAME_DUMP
	    fxtm_dump((fxtm_data_t*)buf);
#endif
	}
    } while (!finish);
}

int main(int argc, char** argv)
{
    int  fd;
    char logfilename[128];
    std::thread acqtask;
    std::thread dumptask;

    if(argc > 1 && !strncmp("-l",argv[1],2))
	fd = openregular(argc,argv);
    else if(argc > 1 && !strncmp("-h",argv[1],2))
 	do_usage(argv);	
    else
	fd = openserial(argc,argv);

    if(fd<0) return -1;

    memset(logfilename,0,128);
    getlogfile(logfilename);
    //assert(strlen(logfilename)>0);

    logger* l = new logger(logfilename);

    acqtask = std::thread(thread_acquisition,fd,l);
    dumptask = std::thread(thread_dumper,l);
    //thread_acquisition(fd, l);
    //thread_dumper(l);

    dumptask.join();
    acqtask.join();

    close(fd);
}

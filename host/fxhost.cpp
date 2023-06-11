#define TAG "HOST"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "fusexutilpc.h"
#include "trame.h"
#include "logger.h"

#if 0
# define HEX_DUMP    1
#endif

#if 1
# define TRAME_DUMP  1
#endif

#if 1
# define TRAME_CHECK 1
#endif

#ifdef TRAME_CHECK
# define fxhost_check(b) do { \
    uint32_t lastts; \
    uint16_t lastid; \
    uint16_t ret = fxtm_check((fxtm_data_t*)buf, &lastid, &lastts); \
    if (ret != 0) { \
        uint32_t ts; \
        uint16_t id; \
        fxtm_getrxts((fxtm_block_t*)buf, &ts); \
        fxtm_getid((fxtm_data_t*)buf, &id); \
        printf("Discontinuation at id:%u at ts:%u, lastid:%u lastts:%u\r\n", \
               id, ts, lastid, lastts); \
    } \
} while(0);
#else
# define fxhost_check(b) do { (void) b; } while(0)
#endif

#ifdef HEX_DUMP
# define fxhost_dump(b, l) hexdump(b, l)
#elif defined(TRAME_DUMP)
# define fxhost_dump(b, l) do { \
    (void)l; \
    char out[1024]; \
    fxtm_dumprxdata(b, out, 1024); \
    printf("%s", out); \
} while(0);
#else
# define fxhost_dump(b, l) do { (void)b; (void)l; } while(0)
#endif

#define SERIALBAUD      B115200
#define SERVER_UDPPORT  54321
#define SERVER_IPADDR   "127.0.0.1"
#define BGC_ACQ_PERIOD  (100*1000)

typedef struct {
    int         fd;
    int         sockfd;
    std::thread acqtask;
    std::thread consotask;
    logger*     log;
    bool        simu;
} fxhost_t;

static fxhost_t           fxh;
static size_t             chunksize = 0;
static bool               asktoterm = false;
static struct sockaddr_in server;

void hexdump(const uint8_t* buf, size_t len)
{
    unsigned char *p;
    for (p = (unsigned char*) buf; len-- > 0; p++) {
            printf(" 0x%x", *p);
    }
    printf("\n");
}

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

    if (tcsetattr(fd, TCSANOW, &tty) < 0) {
        printf("Error tcsetattr: %s\n", strerror(errno));
    }
}

int openregular(int argc,char** argv)
{
    int fd;
    if (argc<3) {
        printf("please provide file\n");
        exit(1);
    }
    fd = open(argv[2], O_RDONLY);
    if (fd < 0) {
        printf("Error opening %s: %s\n", argv[1], strerror(errno));
        return -1;
    }

    if (chunksize == 0) {
        chunksize = fxtm_getblocksize();
    }

    return fd;
}

int openserial(int argc, char** argv)
{
    int  fd;
    const char *portname = "/dev/ttyACM1";

    if (argc > 1) {
        portname = argv[1];
    }

    fd = open(portname, O_RDONLY | O_NOCTTY);
    if (fd < 0) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }
    set_interface_attribs(fd, SERIALBAUD);

    if (chunksize == 0) {
	chunksize = fxtm_getrxdatasize();
    }

    return fd;
}

int opensocket(struct sockaddr_in* servaddr)
{
    int sockfd;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Error socket opening : %s\n", strerror(errno));
        return -2;
    }

    memset(servaddr, 0, sizeof(struct sockaddr_in));

    servaddr->sin_family = AF_INET;
    servaddr->sin_port = htons(SERVER_UDPPORT);
    servaddr->sin_addr.s_addr = inet_addr(SERVER_IPADDR);

    return sockfd;
}

void do_usage(char** argv)
{
    printf("\tUsage: %s [-r] file\n",argv[0]);
    exit(-2);
}

void getlogfile(char* filename)
{
    struct tm *t;
    time_t now = time(NULL);
    t = gmtime(&now);
    strftime(filename, 128, "fusexlog-%Y-%m-%d", t);
}

static uint32_t chunkcounter = 0;

void thread_acquisition(int fd, logger* log)
{
    size_t rb = 0;
    bool   finish = false;
    FILE*  file = fdopen(fd,"ro");

    assert(chunksize>0);

    printf("chunksize is set to:%ld\n", chunksize);

    do {
        size_t rdlen = 0;
        rdlen = fread(fxtm_getdata(), chunksize, 1, file);
        if (rdlen > 0) {
            log->wlog((uint8_t*)fxtm_getdata(), fxtm_getrxdatasize());
            printf("%d\r", chunkcounter++);
        } else {
#if 0
            printf("Error from read:%s rdlen:%ld\n", strerror(errno), rdlen);
#endif
            finish = true;
        }

        rb += chunksize;
        if (fxh.simu) usleep(BGC_ACQ_PERIOD*9/10);
    } while (!finish && !asktoterm);
    log->flush();
    printf("end of acquisition\n");
}

void thread_conso(logger* log)
{
    uint8_t buf[512];
    char    bufJSON[1024];
    bool    finish = false;

    do {
        size_t rd = log->rlog(buf, fxtm_getrxdatasize());
        if (rd > 0) {
            fxhost_dump(buf, rd);
            fxhost_check(buf);

            size_t jsize = fxtm_tojson(buf, bufJSON, sizeof(bufJSON));
#if 0
            printf("wrote json size of:%ld\n",jsize);
#endif
            if (sendto(fxh.sockfd, (const uint8_t *)bufJSON, jsize, 0,
                    (const struct sockaddr *) &server, sizeof(server)) < 0) {
                printf("Error from sendto %s\n", strerror(errno));
                finish = true;
            }
        }
    } while (!finish && !asktoterm);
    printf("end of consumer\n");
}

void sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("SigInt catched! terminating ...\n");
        asktoterm = true;
        delete fxh.log;
        close(fxh.fd);
        close(fxh.sockfd);
    }
}

int main(int argc, char** argv)
{
    char logfilename[128];
    fxh.simu = false;

    if (argc > 1 && !strncmp("-r",argv[1],2)) {
        fxh.fd = openregular(argc,argv);
        fxh.simu = true;
    } else if (argc > 1 && !strncmp("-h",argv[1],2)) {
        do_usage(argv);
    } else {
        fxh.fd = openserial(argc,argv);
    }

    if (fxh.fd<0) return -1;

    memset(logfilename,0,128);
    getlogfile(logfilename);
    assert(strlen(logfilename)>0);

    fxh.sockfd = opensocket(&server);
    if (fxh.sockfd<0) return -2;

#if 0
    if (signal(SIGINT, sig_handler)==SIG_ERR) {
        printf("cannot catch SIGINT\n");
    }
#endif

    /* Instantiate logger that will store data within a thread */
    fxh.log = new logger(logfilename);

    /* Start consumer thread */
    fxh.consotask = std::thread(thread_conso,fxh.log);

    /* Start producer thread */
    fxh.acqtask = std::thread(thread_acquisition, fxh.fd, fxh.log);

    fxh.acqtask.join();
    fxh.log->join();
    fxh.consotask.join();
}

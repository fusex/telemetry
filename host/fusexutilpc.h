#ifndef FUSEXUTIL_H
#define FUSEXUTIL_H
#include <stdio.h>

#define PCSERIALBAUD     115200

#define MYTRACE(x, ...) do { \
    printf(":" TAG "\t: " x, ##__VA_ARGS__); \
} while(0)

#define TTRACE(x, ...) do { \
    printf(":" TAG "\t: " x, ##__VA_ARGS__); \
} while(0)

#endif // FUSEXUTIL_H

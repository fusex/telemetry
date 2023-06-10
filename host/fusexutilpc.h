#ifndef FUSEXUTILPC_H
#define FUSEXUTILPC_H
#include <stdio.h>

#define PCSERIALBAUD     115200

#define MYTRACE(x, ...) do { \
    printf(":" TAG "\t: " x, ##__VA_ARGS__); \
} while(0)

#define TTRACE(x, ...) do { \
    printf(":" TAG "\t: " x, ##__VA_ARGS__); \
} while(0)

#endif // FUSEXUTILPC_H

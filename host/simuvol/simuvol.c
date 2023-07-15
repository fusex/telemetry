#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define FXTM_FLIGHTSTATUS_REG           0
#define FXTM_FLIGHTSTATUS_MASK          7
#define FXTM_FLIGHTSTATUS(x)            (x&FXTM_FLIGHTSTATUS_MASK)

#define FXTM_ERROR_REG            (3)
#define FXTM_ERROR_MASK           (0x1f)

#define FXTM_FLIGHTSTATUS_LAUNCHPAD     0
#define FXTM_FLIGHTSTATUS_LIFTOFF       1
#define FXTM_FLIGHTSTATUS_BURNOUT       2
#define FXTM_FLIGHTSTATUS_SEPARATION    3
#define FXTM_FLIGHTSTATUS_APOGEE        4
#define FXTM_FLIGHTSTATUS_RECOVERY      5
#define FXTM_FLIGHTSTATUS_BALISTIC      6
#define FXTM_FLIGHTSTATUS_TOUCHDOWN     7

#define FXTM_FLIGHTSTATUS_STRING(x) (\
    (x&FXTM_FLIGHTSTATUS_MASK) == FXTM_FLIGHTSTATUS_LAUNCHPAD?"LAUNCHPAD":\
    (x&FXTM_FLIGHTSTATUS_MASK) == FXTM_FLIGHTSTATUS_LIFTOFF?"LIFTOFF":\
    (x&FXTM_FLIGHTSTATUS_MASK) == FXTM_FLIGHTSTATUS_BURNOUT?"BURNOUT":\
    (x&FXTM_FLIGHTSTATUS_MASK) == FXTM_FLIGHTSTATUS_SEPARATION?"SEPARATION":\
    (x&FXTM_FLIGHTSTATUS_MASK) == FXTM_FLIGHTSTATUS_APOGEE?"APOGEE":\
    (x&FXTM_FLIGHTSTATUS_MASK) == FXTM_FLIGHTSTATUS_RECOVERY?"RECOVERY":\
    (x&FXTM_FLIGHTSTATUS_MASK) == FXTM_FLIGHTSTATUS_BALISTIC?"BALTISTIC":\
    (x&FXTM_FLIGHTSTATUS_MASK) == FXTM_FLIGHTSTATUS_TOUCHDOWN?"TOUCHDOWN":\
    "ERROR")

//#define Gr     (10)
#define Gr     (9.8)
#define THRESHOLD_LIFTOFF_ACC   (1.5*Gr) //OK
#define THRESHOLD_PREBURNOUT_ACC (0.5*Gr) //OK
#define THRESHOLD_BURNOUT_ACC   (2*Gr) //OK
#define THRESHOLD_APOGEE_ACC    (1*Gr) //OK
#define THRESHOLD_BALISTIC_ACC  (1*Gr) //??
#define THRESHOLD_RECOVERY_ACC  (0.2*Gr)//??
#define THRESHOLD_TOUCHDOWN_ACC (1*Gr)//??

#define MAX_ACC ((uint16_t)-1)
static uint16_t max_acc = 0;
static uint16_t min_acc = MAX_ACC;
static float    ts = 0.0;
static float    acc = 0.0;

uint8_t gFlightStatus = FXTM_FLIGHTSTATUS_LAUNCHPAD;

void fxtm_setflightstatus (uint8_t flightStatus)
{
    gFlightStatus &= (FXTM_ERROR_MASK<<FXTM_ERROR_REG);
    gFlightStatus |= flightStatus;
    printf("At ts: %f ", ts);
    printf("acc: %f ", acc);
    printf("flightstatus:%s(%3u)\r\n",
	   FXTM_FLIGHTSTATUS_STRING(gFlightStatus), FXTM_FLIGHTSTATUS(gFlightStatus));
}

void fxtm_getflightstatus (uint8_t* pFlightStatus)
{
    *pFlightStatus = gFlightStatus;
}

static bool preburnout = false;

void fxstatus_setacc2 (float new_acc)
{
    uint8_t status = 0;if (new_acc < min_acc) {
        min_acc = new_acc;
    }

    if (new_acc > max_acc) {
        max_acc = new_acc;
    }

    fxtm_getflightstatus(&status);
    switch (status) {
        case FXTM_FLIGHTSTATUS_LAUNCHPAD:
            if (new_acc > THRESHOLD_LIFTOFF_ACC) {
                fxtm_setflightstatus(FXTM_FLIGHTSTATUS_LIFTOFF);
            }
            break;
            //OK

        case FXTM_FLIGHTSTATUS_LIFTOFF:
            if (new_acc < THRESHOLD_PREBURNOUT_ACC) {
                preburnout = true;
            }
            if (preburnout && (new_acc > THRESHOLD_BURNOUT_ACC)) {
                fxtm_setflightstatus(FXTM_FLIGHTSTATUS_BURNOUT);
            }
            break;
            //OK

        case FXTM_FLIGHTSTATUS_BURNOUT:
            if (new_acc < THRESHOLD_APOGEE_ACC) {
                fxtm_setflightstatus(FXTM_FLIGHTSTATUS_APOGEE);
            }
            break;
            //OK

        case FXTM_FLIGHTSTATUS_APOGEE:
            if (status != FXTM_FLIGHTSTATUS_SEPARATION) {
                if (new_acc > THRESHOLD_BALISTIC_ACC) {
                    fxtm_setflightstatus(FXTM_FLIGHTSTATUS_BALISTIC);
                }
            }
            break;
            //OK

        case FXTM_FLIGHTSTATUS_SEPARATION:
            if (new_acc > THRESHOLD_RECOVERY_ACC) {
                fxtm_setflightstatus(FXTM_FLIGHTSTATUS_RECOVERY);
            }
            break;
            //OK

        case FXTM_FLIGHTSTATUS_BALISTIC:
            //fall-trough
        case FXTM_FLIGHTSTATUS_RECOVERY:
            if (new_acc > THRESHOLD_TOUCHDOWN_ACC) {
                fxtm_setflightstatus(FXTM_FLIGHTSTATUS_TOUCHDOWN);
            }
            break;
            //OK
    }
}

const char* getfield(char* line, int num)
{
    const char* tok;
    for (tok = strtok(line, ";");
            tok && *tok;
            tok = strtok(NULL, ";\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}

int main()
{
    int i = 0;
    fxtm_setflightstatus(FXTM_FLIGHTSTATUS_LAUNCHPAD);

    FILE* stream = fopen("input", "r");

    char line[1024];
    while (fgets(line, 1024, stream))
    {
        char* tmp = strdup(line);
        char* tmp2 = strdup(line);
        const char* tts = getfield(tmp, 1);
        sscanf(tts, "%f", &ts);
        const char* as = getfield(tmp2, 3);
        sscanf(as, "%f", &acc);
#if 0
        printf("Field 1 would be %s ", tts);
        printf("At ts:%f\n", ts);
        printf("Field 3 would be %s ", as);
        printf("acc:%f\n", acc);
#endif
        fxstatus_setacc2(acc);
        free(tmp);
        free(tmp2);
        i++;
        if (i == 2122) {
            fxtm_setflightstatus(FXTM_FLIGHTSTATUS_SEPARATION);
        }
    }

    printf("FINAL: min_acc:%d max_acc:%d\n",min_acc, max_acc);
    printf("FINAL: flightstatus:%s(%3u)\r\n",
	   FXTM_FLIGHTSTATUS_STRING(gFlightStatus), FXTM_FLIGHTSTATUS(gFlightStatus));

    return 0;
}

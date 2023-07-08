#include <stdint.h>
#include <stddef.h>
#include "trame.h"
#include "math.h"

#define MAX_ACC ((uint16_t)-1)

static float max_acc = 0;
static float min_acc = MAX_ACC;
static float acc = 0;

#define THRESHOLD_LIFTOFF_ACC   1
#define THRESHOLD_BURNOUT_ACC   1
#define THRESHOLD_APOGEE_ACC    1
#define THRESHOLD_BALISTIC_ACC  1
#define THRESHOLD_RECOVERY_ACC  1
#define THRESHOLD_TOUCHDOWN_ACC 1

void fxstatus_setacc(imuraw_t a[])
{
    uint8_t status = 0;
    float abs_acc = 0;
    float new_acc = 0;

    abs_acc  = a[0]*a[0];
    abs_acc += a[1]*a[1];
    abs_acc += a[2]*a[2];

    abs_acc = sqrt(abs_acc);
    new_acc = (0.9*abs_acc) + (0.1*acc);

    if (new_acc < min_acc) {
        min_acc = new_acc;
    }

    if (new_acc > max_acc) {
        max_acc = new_acc;
    }

    fxtm_getflightstatus(NULL, &status);
    switch (status) {
        case FXTM_FLIGHTSTATUS_LAUNCHPAD:
            if (new_acc > THRESHOLD_LIFTOFF_ACC) {
                fxtm_setflightstatus(FXTM_FLIGHTSTATUS_LIFTOFF);
            }
            break;
            //OK
            
        case FXTM_FLIGHTSTATUS_LIFTOFF:
            if (new_acc < THRESHOLD_BURNOUT_ACC) {
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
            if (new_acc < THRESHOLD_TOUCHDOWN_ACC) {
                fxtm_setflightstatus(FXTM_FLIGHTSTATUS_TOUCHDOWN);
            }
            break;
            //OK
    }
    
    acc = new_acc;
}

void dumxFxstatus (bool isConsole)
{

}

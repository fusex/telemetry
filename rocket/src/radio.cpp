#define TAG "RADIO"

#include <fusexconfig.h>
#include <fusexutil.h>
#include <BGC_Lora.h> 
#include <trame.h>

#include "init.h"

#if 0
static BGC_Lora lora(10, true);  //arduino mega board with lora shield
#else
static BGC_Lora lora(BGC_LORA_SS, true);
#endif

void setupRadio ()
{
    module_add(TAG);
    if (!lora.init()) {
        TTRACE("init failed ! Fatal !!!\n\r");
        Init_SetFatal();
        return;
    } else {
        TTRACE("init Done\n\r");
        module_setup(TAG, FXTM_SUCCESS);
    }
}

void loopRadio ()
{
    lora.send((uint8_t *)fxtm_getdata(), fxtm_getdatasize());
}

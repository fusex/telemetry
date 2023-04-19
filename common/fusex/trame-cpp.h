#ifndef _TRAME_CPP_H
#define _TRAME_CPP_H

extern "C"
{

void fxtm_reset(uint32_t ts);
void fxtm_gendata(void);
void fxtm_setsoundlvl(unsigned int level);
void fxtm_setimu(float a[], float m[], float g[]);
void fxtm_setimu2(float a[]);
void fxtm_settemperature(int8_t temperature);
void fxtm_setpressure(uint16_t pressure);
void fxtm_setdiffpressure(uint16_t diffpressure);
void fxtm_sethumidity(float humidity);
void fxtm_setflightstatus(uint8_t flightStatus);
void fxtm_setgps(float latitude, float longitude);

fxtm_data_t*  fxtm_getdata();
fxtm_block_t* fxtm_getblock();

size_t   fxtm_getblocksize();
size_t   fxtm_getdatasize();
size_t   fxtm_dumpdata(fxtm_data_t* tm, char* buf, size_t bufsize);
uint16_t fxtm_check(fxtm_data_t* tm);
size_t   fxtm_tojson(fxtm_data_t* tm, char* buf, size_t bufsize);

void fxtm_getimu(fxtm_data_t* tm, float* imu);
void fxtm_getgps(fxtm_data_t* tm, float* gps);
void fxtm_getpressure(fxtm_data_t* tm, uint32_t* ppressure);
void fxtm_getts(fxtm_data_t* tm, uint32_t* pts);
void fxtm_getid(fxtm_data_t* tm, uint16_t* pid);
void fxtm_getsoundlvl(fxtm_data_t* tm, uint8_t* psndlvl);
void fxtm_gettemperature(fxtm_data_t* tm, int8_t* ptemp);
void fxtm_gethumidity(fxtm_data_t* tm, uint8_t* phumidity);
}

static_assert(sizeof(fxtm_block_t) == 512, "fxtm_block_t must be 512 bytes");
#endif //define _TRAME_H

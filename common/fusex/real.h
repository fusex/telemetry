typedef float imureal_t;
typedef struct {
    imureal_t x;
    imureal_t y;
    imureal_t z;
} imureal_sensor_t;

typedef float gpsreal_t;
typedef struct {
    gpsreal_t lat;
    gpsreal_t lon;
} gps_position_t;

typedef struct {
    float    speedx;
    float    speedy;
} speed_t;

typedef struct {
    float    qx;
    float    qy;
    float    qz;
    float    qw;
} quaternion_t;

typedef struct {
    float    qx;
    float    qy;
    float    qz;
} position_t;

typedef struct {
    int16_t  rssi;
    int16_t  snr;
    int16_t  frequencyError;
    uint32_t frequency;
} radio_t;

typedef struct {
    int8_t            temperature;
    uint8_t           humidity;
    uint16_t          pressure;
} atmos_t;

typedef struct {
    uint32_t          id;
    uint32_t          timestamp;
                     
    uint16_t          diffpressure;
    atmos_t           atmos;

#if 0                
    uint8_t           soundLevel;
#endif               

    uint8_t           battLevel;
    uint8_t           flightStatus;

    uint32_t          altimeter;
    gps_position_t    gps;

    imureal_sensor_t  accel;
    imureal_sensor_t  gyro;
    imureal_sensor_t  magn;
    imureal_sensor_t  accel2;
    speed_t           groundspeed;
    quaternion_t      orientation;
    position_t        pos;

    radio_t           radio; 
} fxreel_data_t;

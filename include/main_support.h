#ifndef MAIN_SUPPORT_H
#define MAIN_SUPPORT_H

#include <String>
#include <accel_data.h>
//#include <sd_data.h>
#include <FlexCAN_T4.h>
#include <MegaCAN.h>
#include <arduino.h>
#include <RingBuf.h>
#include <SdFat.h>
#include <InternalTemperature.h>
#include <pc_communication.h>
#include <SPI.h>
#include <analog_sensors.h>
//#include <imxrt.h>
// Use Teensy SDIO
#define SD_CONFIG  SdioConfig(FIFO_SDIO)

// Interval between points for 1 ksps.
#define LOG_INTERVAL_USEC 1000

// 30 data points 2 bytes per data and 1 byte for comma => 90 bytes per line
// Size to log 90 byte lines at 1 kHz for more than ten minutes.
const uint32_t MAX_SAMPLE_COUNT = (20000000);
const uint32_t LOG_FILE_SIZE = (303 + (107 * (MAX_SAMPLE_COUNT)));  // 

// Space to hold more than 800 ms of data for 90 byte lines at 1 ksps.
#define LOG_FILENAME ("car_data.csv")
// This defines the header for the SD file
const String DATA_HEADER = "time_ms,A_0,A_1,A_2,A_3,A_4,A_5,A_6,A_7,accel_X,accel_Y,accel_Z,accel_temperature,msq_seconds,msq_pw1,msq_pw2,msq_rpm,msq_adv_deg,msq_squirt,msq_engine,msq_afrtgt1,msq_afrtgt2,msq_baro,msq_map,msq_mat,msq_clt,msq_tps,msq_batt,msq_TPSdot,msq_MAPdot,msq_RPMdot,msq_MAF,msq_dwell,msq_syncnt,msq_syncreason";

constexpr uint32_t RED_LED_PIN = 33;
constexpr uint32_t BLUE_LED_PIN = 34;
constexpr uint32_t GREEN_LED_PIN = 35;
constexpr uint32_t PHOTO_PIN = 25; 
constexpr uint32_t analog_sensor_pins[1] = { PHOTO_PIN };
constexpr uint32_t ACCEL_INT1_PIN = 9;
constexpr uint32_t ACCEL_CS_PIN = 0;
constexpr uint32_t NUM_DATA_SAMPLES_STORED = 512;
constexpr uint32_t NUM_RAW_SAMPLES_STORED = NUM_DATA_SAMPLES_STORED >> 4;
uint32_t NUM_SHIFTS_RAW_SAMPLES_TO_AVG = (uint32_t)log2(NUM_RAW_SAMPLES_STORED);

typedef enum DATA_TYPE{
    A_0,
    A_1,
    A_2,
    A_3,
    A_4,
    A_5,
    A_6,
    A_7,
    teensy_temp,
    accel_X,
    accel_Y,
    accel_Z,
    accel_temperature,
    msq_seconds,        // Seconds since startup
    msq_pw1,
    msq_pw2,
    msq_rpm,            // Engine RPM 
    msq_adv_deg,
    msq_squirt,
    msq_engine,
    msq_afrtgt1,
    msq_afrtgt2,
    msq_baro,           // Barometric Pressure
    msq_map,            // Manifold Air Pressure (kPa)
    msq_mat,
    msq_clt,            // Coolant temperature (degrees F)
    msq_tps,            // Throttle Position (%)
    msq_batt,
    msq_TPSdot,
    msq_MAPdot,
    msq_RPMdot,
    msq_MAF,
    msq_dwell,
    msq_syncnt,
    msq_syncreason,
    NUM_DATA_TYPES
} data_type;


typedef enum CAR_STATE{
    OFF,
    NEUTRAL,
    PC_CONNECTED,
    RUNNING,
    LOGGING_DATA,
    TEST,
    ACCEL_INT1,
    ACCEL_INT2,
    NUM_STATES // Simple counter of the states we have, don't add anything beyond this
} car_state;


#endif
#ifndef ADXL362_H
#define ADXL362_H

constexpr char SOFT_RESET_VAL = 'R';
constexpr uint8_t DEVICE_ID = 0;

constexpr auto ACCEL_RANGE_2G = (0b00) << 6;
constexpr auto ACCEL_RANGE_4G = (0b01) << 6;
constexpr auto ACCEL_RANGE_8G = (0b10) << 6;
constexpr auto HALF_BW = 0b0 << 4;
constexpr auto QUARTER_BW = 0b1 << 4;
constexpr auto DATA_RATE_12_5Hz = 0b000;
constexpr auto DATA_RATE_25Hz = 0b001;
constexpr auto DATA_RATE_50Hz = 0b010;
constexpr auto DATA_RATE_100Hz = 0b011; // On reset accel will default to this speed
constexpr auto DATA_RATE_200Hz = 0b100;
constexpr auto DATA_RATE_400Hz = 0b111;


typedef enum control_registers
{
    DEVID_AD = 0x00, // Device ID
    DEVID_MST = 0x01,
    PARTID = 0x02,
    REVID = 0x03,
    STATUS = 0x0B,
    FIFO_ENTRIES_L = 0x0C,
    FIFO_ENTRIES_H = 0x0D,
    SOFT_RESET = 0x1F, // Send ascii value for 'R' (0x52) to this reg then delay 500uS for soft reset
    THRESH_ACT_L = 0x20,
    THRESH_ACT_H = 0x21,
    TIME_ACT = 0x22,
    THRESH_INACT_L = 0x23,
    THRESH_INACT_H = 0x24,
    TIME_INACT_L = 0x25,
    TIME_INACT_H = 0x26,
    ACT_INACT_CTL = 0x27,
    FIFO_CONTROL = 0x28,
    INTMAP1 = 0x2A,
    INTMAP2 = 0x2B,
    FILTER_CTL = 0x2C,
    POWER_CTL = 0x2D,
    SELF_TEST = 0x2E,
} control_registers;

typedef enum data_registers
{
    XDATA = 0x08,
    YDATA = 0x09,
    ZDATA = 0x0A,
    XDATA_L = 0x0E,
    XDATA_H = 0x0F,
    YDATA_L = 0x10,
    YDATA_H = 0x11,
    ZDATA_L = 0x12,
    ZDATA_H = 0x13,
    TEMP_L = 0x14,
    TEMP_H = 0x15,
    FIFO_SAMPLES = 0x29,
} data_registers;


#endif
#ifndef ACCEL_DATA_H
#define ACCEL_DATA_H

#include <SPI.h>
#include <ADXL362.h>

constexpr uint32_t SPI_CLK_SPEED =  8000000; // can be anywhere rom 1MHz to 8MHz for the ADXL362
constexpr uint8_t WRITE_CMD =       0x0A;
constexpr uint8_t READ_CMD =        0x0B;
constexpr uint8_t READ_FIFO_CMD =   0x0D;
constexpr auto ACCEL1_DEVICE_ID = 0xAD;

class accel_data
{
public: 
    void begin_accel(const uint32_t CS_PIN);
    bool detect_accel(void);
    void set_activity_threshold(int16_t threshold_val);
    // Description sets the time to activate for the accel time (in seconds) = time_act / (data collection rate); output data rate is set in filter_ctl reg
    // NOTE: To avoid false positives set this to at least greater than 1
    void set_time_to_active(uint8_t time_act_val);
    // Sets the threshold for when device will consider motion to be detected
    void set_inactivity_threshold(int16_t inactivity_val);
    // Sets the time for if activity is not detected
    void set_inactive_time(int16_t inactivity_val);
    // Disable FIFOs for now
    void disable_FIFO(void);
    void disable_act_inact(void);
    void enable_INT1_data_ready(void);
    void begin_2g_res_measure(void);
    int16_t get_x_axis_measurement(void);
    int16_t get_y_axis_measurement(void);
    int16_t get_z_axis_measurement(void);
    int16_t get_temperature_measurement(void);

    uint32_t m_CS_pin;
   
    void write_reg(const uint8_t write_address, const uint8_t data);
    uint8_t read_reg(const uint8_t read_address);
    void double_write(const uint8_t start_reg_addr, const uint8_t data_L, const uint8_t data_H);
    int16_t double_read(const uint8_t start_reg_addr);
    void burst_read_data(int16_t accel_data_array[5]);
    byte get_status_register(void);
    byte get_int1_status_reg(void);

    inline void pin_toggle(const uint32_t PIN) {
        digitalWriteFast(PIN, digitalReadFast(PIN) ^ 1);
    }

private:

};


#endif
#ifndef ANALOG_SENSORS_H
#define ANALOG_SENSORS_H

#include <stdint.h>
#include <arduino.h>
#include <RingBuf.h>

class analog_sensors
{

public:
    void init_sensors(const int num_sensors, const uint32_t *pin_array);
    int16_t read_sensor(const int sensor_pin);
    void read_all_sensors(const int num_sensors, int16_t *temp_buf, const uint32_t *pin_array);
    void calibrate_max_photoVal(void);
    int16_t get_max_photo(void) const;
private:
    /* data */
    int16_t max_photo_val;

};



#endif
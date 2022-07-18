#include <analog_sensors.h>
#define MAX(a,b) (a > b) ? (a) : (b)

void analog_sensors::init_sensors(const int num_sensors, const uint32_t *pin_array)
{
    for(int i = 0; i < num_sensors; i++)
    {
        pinMode(pin_array[i], INPUT);
    }
}

int16_t analog_sensors::read_sensor(const int sensor_pin)
{
    return (int16_t)analogRead(sensor_pin);
}

void analog_sensors::read_all_sensors(const int num_sensors, int16_t *temp_buf, const uint32_t *pin_array)
{
    for(int i = 0; i < num_sensors; i++)
    {
        *(temp_buf + i) = (int16_t)analogRead(*(pin_array + i));
    }
}

void analog_sensors::calibrate_max_photoVal(void)
{
    int count = 0;
    while(count++ > 10000)
    {
        max_photo_val = MAX(read_sensor(25), max_photo_val);
        delayMicroseconds(100);
    }
}

int16_t analog_sensors::get_max_photo(void) const
{
    return max_photo_val;
}

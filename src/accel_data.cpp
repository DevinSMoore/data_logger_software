#include <accel_data.h>
#ifndef _TEST_
//#define _TEST_
#endif
void accel_data::begin_accel(const uint32_t CS_PIN)
{
    m_CS_pin = CS_PIN;
    pinMode(m_CS_pin, OUTPUT);
    digitalWrite(m_CS_pin, HIGH);
    SPI1.begin();
    write_reg(POWER_CTL, 0x0A);
}

bool accel_data::detect_accel(void)
{
    byte accel_id = read_reg(DEVICE_ID);
    //Serial.printf("Device ID returned: 0x%X\n", accel_id);
    if(accel_id == ACCEL1_DEVICE_ID)
    {
        return true;
    }

    return false;
}


void accel_data::set_activity_threshold(int16_t threshold_val)
{

    byte thresh_H = (threshold_val & 0xFF00) >> 8;
    byte thresh_L = (threshold_val & 0x00FF);

    #ifdef _TEST_
        Serial.printf("threshold_val: %d\n", threshold_val);
        Serial.printf("Thresh_(H/L): 0x%X | 0x%X\n", thresh_H, thresh_L);
        double_write(THRESH_ACT_L, thresh_L, thresh_H);
        Serial.printf("Value of threshold register: %d\n", double_read(THRESH_ACT_L));
    #else
        double_write(THRESH_ACT_L, thresh_L, thresh_H);
    #endif
}

void accel_data::set_time_to_active(uint8_t time_act_val)
{
    write_reg(TIME_ACT, time_act_val);
    #ifdef _TEST_
        Serial.printf("TIME_ACT register value: 0x%X\n", read_reg(TIME_ACT));
    #endif
}

void accel_data::set_inactivity_threshold(int16_t inactivity_val)
{
    byte thresh_H = (inactivity_val & 0xFF00) >> 8;
    byte thresh_L = (inactivity_val & 0x00FF);

    #ifdef _TEST_
        Serial.printf("threshold_val: %d\n", inactivity_val);
        Serial.printf("Thresh_(H/L): 0x%X | 0x%X\n", thresh_H, thresh_L);
        double_write(THRESH_INACT_L, thresh_L, thresh_H);
        Serial.printf("Value of threshold register: %d\n", double_read(THRESH_ACT_L));
    #else
        double_write(THRESH_INACT_L, thresh_L, thresh_H);
    #endif
}

void accel_data::set_inactive_time(int16_t inactivity_val)
{
    byte thresh_H = (inactivity_val & 0xFF00) >> 8;
    byte thresh_L = (inactivity_val & 0x00FF);

    #ifdef _TEST_
        Serial.printf("threshold_val: %d\n", inactivity_val);
        Serial.printf("Thresh_(H/L): 0x%X | 0x%X\n", thresh_H, thresh_L);
        double_write(TIME_INACT_L, thresh_L, thresh_H);
        Serial.printf("Value of threshold register: %d\n", double_read(THRESH_ACT_L));
    #else
        double_write(TIME_INACT_L, thresh_L, thresh_H);
    #endif
}

// Disable FIFO
// Write 0 to reg FIFO_CONTROL
void accel_data::disable_FIFO(void)
{
    write_reg(FIFO_CONTROL, 0x00);
  
    #ifdef _TEST_
        if(read_reg(FIFO_CONTROL) == 0)
        {
            Serial.println("FIFO DISABLED...");
        }
    #endif
}

void accel_data::disable_act_inact(void)
{
    write_reg(ACT_INACT_CTL, 0x00);
   
    #ifdef _TEST_
    if(read_reg(ACT_INACT_CTL) == 0)
    {
        Serial.println("Act/Inactivity thresholds disabled...");
    }
    #endif
}

void accel_data::enable_INT1_data_ready(void)
{
    // write 0000 | 0001 -> data ready enable is bit[0]
    write_reg(INTMAP1, 0x01);
    
    #ifdef _TEST_
    if(read_reg(INTMAP1) == 0x01)
    {
        Serial.println("Data ready interrupt enabled...");
    }
    #endif
}

void accel_data::begin_2g_res_measure(void)
{
    write_reg(FILTER_CTL, (ACCEL_RANGE_2G | QUARTER_BW | DATA_RATE_200Hz)); // To enable 2g we want 00 in bit[7:6] and for a 12.5 Hz data rate set bit[2:0] to 000; so write 0x00 to enable 2g
    write_reg(POWER_CTL, 0x02); // Write 10 to bit[1:0] to enable measurement mode
    #ifdef _TEST_
        if((read_reg(FILTER_CTL) == 0x00) && (read_reg(POWER_CTL) == 0x02))
        {
            Serial.println("2g measurement normal operation enabled...");
        }
    #endif
}

int16_t accel_data::get_x_axis_measurement(void)
{
    return (read_reg(XDATA_H) << 8) | (read_reg(XDATA_L));
}

int16_t accel_data::get_y_axis_measurement(void)
{
    return (read_reg(YDATA_H) << 8) | (read_reg(YDATA_L));
}

int16_t accel_data::get_z_axis_measurement(void)
{
    return (read_reg(ZDATA_H) << 8) | (read_reg(ZDATA_L));
}

int16_t accel_data::get_temperature_measurement(void)
{
    return (read_reg(TEMP_H) << 8) | (read_reg(TEMP_L));
}

uint16_t read_x_data()
{
    return 0;
}

void accel_data::write_reg(const uint8_t write_address, const uint8_t data)
{
    SPI1.beginTransaction(SPISettings(SPI_CLK_SPEED, MSBFIRST, SPI_MODE0));
    digitalToggle(m_CS_pin);
    SPI1.transfer(WRITE_CMD);
    SPI1.transfer(write_address);
    SPI1.transfer(data);
    digitalToggle(m_CS_pin);
    SPI1.endTransaction();
}

uint8_t accel_data::read_reg(const uint8_t read_address)
{
    SPI1.beginTransaction(SPISettings(SPI_CLK_SPEED, MSBFIRST, SPI_MODE0));

    digitalToggle(m_CS_pin);
    
    SPI1.transfer(READ_CMD);
    SPI1.transfer(read_address);
    uint8_t return_data = SPI1.transfer(0x00); // Send dummy value to push data into the FIFO
    
    digitalToggle(m_CS_pin);
    SPI1.endTransaction();

    return return_data;
}

void accel_data::double_write(const uint8_t start_reg_addr, const uint8_t data_L, const uint8_t data_H)
{
    SPI1.beginTransaction(SPISettings(SPI_CLK_SPEED, MSBFIRST, SPI_MODE0));

    digitalToggle(m_CS_pin);
    
    SPI1.transfer(WRITE_CMD);
    SPI1.transfer(start_reg_addr);
    SPI1.transfer(data_L);
    SPI1.transfer(data_H);

    digitalToggle(m_CS_pin);
    SPI1.endTransaction();

}

int16_t accel_data::double_read(const uint8_t start_reg_addr)
{
    byte temp_byte = 0;
    
    SPI1.beginTransaction(SPISettings(SPI_CLK_SPEED, MSBFIRST, SPI_MODE0));
    digitalToggle(m_CS_pin);
    SPI1.transfer(READ_CMD);
    SPI1.transfer(start_reg_addr);
   
    temp_byte = SPI1.transfer(0x00);
    int16_t return_data = temp_byte;
   
    temp_byte = SPI1.transfer(0x00);
    return_data |= temp_byte << 8;

    digitalToggle(m_CS_pin); 
    SPI1.endTransaction();
    return return_data;
}

void accel_data::burst_read_data(int16_t accel_data_array[4]){
    byte temp[8] = {0};
    SPI1.beginTransaction(SPISettings(SPI_CLK_SPEED, MSBFIRST, SPI_MODE0));
    digitalToggle(m_CS_pin);
    SPI1.transfer(READ_CMD);
    SPI1.transfer(XDATA_L);
    SPI1.transfer(temp, temp, 8);
    accel_data_array[0] = (temp[1] << 8) | temp[0];
    accel_data_array[1] = (temp[3] << 8) | temp[2];
    accel_data_array[2] = (temp[5] << 8) | temp[4];
    accel_data_array[3] = (temp[7] << 8) | temp[6];
    digitalToggle(m_CS_pin);
    SPI1.endTransaction();
}

byte accel_data::get_status_register(void)
{
    SPI1.beginTransaction(SPISettings(SPI_CLK_SPEED, MSBFIRST, SPI_MODE0));
    digitalToggle(m_CS_pin);
    SPI1.transfer(READ_CMD);
    SPI1.transfer(STATUS);
    byte temp = SPI1.transfer(0x00);
    digitalToggle(m_CS_pin);
    SPI1.endTransaction();
    return temp;
}

byte accel_data::get_int1_status_reg(void) 
{
    SPI1.beginTransaction(SPISettings(SPI_CLK_SPEED, MSBFIRST, SPI_MODE0));
    digitalToggle(m_CS_pin);
    SPI1.transfer(READ_CMD);
    SPI1.transfer(INTMAP1);
    byte temp = SPI1.transfer(0x00);
    digitalToggle(m_CS_pin);
    SPI1.endTransaction();
    return temp;
}


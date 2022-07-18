#include <main_support.h>
#include <SD.h>

#ifndef _TEST_
//#define _TEST_
#endif

FILE test_file;
volatile uint32_t ACCEL_INITIALIZED = 0;

static RingBuf<int16_t, NUM_DATA_SAMPLES_STORED> data[NUM_DATA_TYPES];
static RingBuf<int16_t, NUM_RAW_SAMPLES_STORED> raw_data[NUM_DATA_TYPES];
static RingBuf<uint32_t, NUM_DATA_SAMPLES_STORED> time_data;

volatile car_state current_state = NEUTRAL;

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_64> can1;
CAN_message_t msg_in;
CAN_message_t msg_out;
MegaCAN megasquirt_can;
const extern uint32_t baseID = 1512;
constexpr uint32_t FINAL_MSQ_GROUP = 63;
volatile auto FINAL_ID_FLAG = 0;
constexpr auto FINAL_MSQ_ID = baseID + FINAL_MSQ_GROUP; // 63 is the last group number (indexing from 0) and is also the last CAN id from the MSQ
constexpr auto MSQ_BAUD_RATE = 500000; // 500kHz which is defined in the documentation
int16_t temp_array[4] = {0};
byte test_byte = 0;
MegaCAN_broadcast_message_t msq_broadcast_msg; // megasquirt [msq]

pc_communication pc;
accel_data acceleromter;

analog_sensors sensors_analog;

IntervalTimer interval1;
IntervalTimer interval2;
IntervalTimer interval3;

volatile uint32_t GLOBAL_100MS_FLAG = 0;
volatile uint32_t GLOBAL_1MS_FLAG = 0;
volatile uint32_t GLOBAL_2500uS_FLAG = 0;
volatile uint32_t BUFFER_FULL_FLAG = 0;

volatile uint32_t ACCEL_INT1_FLAG = 0;

car_state set_current_state(void);
void initialize_SD(void);
void ACCEL_INT1_ISR(void)
{
  ACCEL_INT1_FLAG = 1;
  //Serial.println("HIT ACCEL_INT1 interrupt handler!...");
}


void CAN1_ISR(const CAN_message_t &in_msg)
{
  //unpack megasquirt broadcast data into mega_bCast_msg:

  // The ECU broadcasts at a set rate, and the idea is for the can bus to be interrupted everytime we get one of these broadcasts.
  // the following function does all of the magic internally
  megasquirt_can.getBCastData(in_msg.id, in_msg.buf, msq_broadcast_msg); // This will automatically do the math on the incoming values.

  if(in_msg.id >= (FINAL_MSQ_ID - 1)) {
    FINAL_ID_FLAG = 1;
  }

}

void interval1_100MS(void)
{
  GLOBAL_100MS_FLAG = 1;
}

volatile uint32_t time_ms = 0;

void interval2_1MS(void)
{
  time_ms++;
}

void init_accel(void);

void interval3_2500_us(void) {
  GLOBAL_2500uS_FLAG = 1;
}

int counter = 0;
uint32_t timer_start = 0;
int16_t temp_16int = 0;
int32_t temp_32int = 0;
uint32_t temp_uint32 = 0;
uint32_t write_counter = 0;
double x_temp = 0;
double y_temp = 0;
int main(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600); delay(100);
  attachInterrupt(digitalPinToInterrupt(ACCEL_INT1_PIN), ACCEL_INT1_ISR, RISING);
  initialize_SD();

  can1.begin();
  can1.setBaudRate(500000);
  can1.enableFIFO();
  can1.enableFIFOInterrupt();
  can1.onReceive(FIFO, CAN1_ISR);

  SD.begin(BUILTIN_SDCARD);
  test_file = SD.open("test_data.csv", FILE_WRITE_BEGIN);

  interval1.begin(interval1_100MS, 100000);
  interval2.begin(interval2_1MS, 1000);
  init_accel();
  time_ms = 0;
  Serial.print("X-axis"); Serial.print('\t');
  Serial.print("Y-axis"); Serial.print('\t');
  Serial.println("Z-axis");




  while(1){
    can1.events();

    //Serial.printf("Millis count: %u\n", millis());
    if(GLOBAL_100MS_FLAG)
    {
      GLOBAL_100MS_FLAG = 0;
      digitalToggle(LED_BUILTIN);
    } 
    
    current_state = set_current_state();


    switch(current_state)
    {
      case TEST:
        
        break;
      case PC_CONNECTED:
        //Serial.println("PC CONNECTED...");
        break;

      case NEUTRAL:
        //Serial.println("PC NOT CONNECTED...");
        
        break;
      case ACCEL_INT1:
        ACCEL_INT1_FLAG = 0;
        //Serial.println("HIT accel_int1 CASE....");

        acceleromter.burst_read_data(temp_array);
        temp_16int = InternalTemperature.readTemperatureF();
        raw_data[accel_X].push(temp_array[0]);
        raw_data[accel_Y].push(temp_array[1]);
        raw_data[accel_Z].push(temp_array[2]);
        raw_data[accel_temperature].push(temp_array[3]);
        raw_data[teensy_temp].push(temp_16int);


        // if((counter % NUM_RAW_SAMPLES_STORED) == 0)
        // {
        //   for(int i = 0; i < 4; i++)
        //   {
        //     temp_16int &= ~temp_16int;
        //     temp_32int &= ~temp_32int;
        //     for(unsigned int j = 0; j < NUM_RAW_SAMPLES_STORED; j++)
        //     {
        //         raw_data[accel_X + i].pop(temp_16int);
        //         temp_32int += temp_16int;
        //     }
        //     temp_16int = temp_32int >> NUM_SHIFTS_RAW_SAMPLES_TO_AVG;
        //     data[accel_X + i].push(temp_16int);
        //     if(i == 0)
        //     {
        //       //Serial.print("AVG x-axis: ");
        //       x_temp = (temp_16int / (double)1000);
        //     } else if (i == 1)
        //     {
        //       y_temp = (temp_16int / (double)1000);
        //     } else if (i == 2)
        //     {
        //       Serial.print(x_temp); Serial.print('\t');
        //       Serial.print(y_temp); Serial.print('\t');
        //       Serial.println(temp_16int / (double)1000);
              
        //     }
        //   }
        //   //Serial.printf("time_ms: %u\n", time_ms);
        //   time_data.push(time_ms);

        // }




        if(data[accel_X].isFull())
        {
          //Serial.println("Data Arrays full...");
          BUFFER_FULL_FLAG = 1;
        }
        ++counter;
        //delayMicroseconds(10);
        break;
      case RUNNING:

   
        if (FINAL_ID_FLAG)
        {
          // This is a subset of all available data.
          FINAL_ID_FLAG = 0;
          data[msq_rpm].push(msq_broadcast_msg.rpm);
          data[msq_clt].push(msq_broadcast_msg.clt);
          data[msq_engine].push(msq_broadcast_msg.engine);
          data[msq_baro].push(msq_broadcast_msg.baro);
          data[msq_mat].push(msq_broadcast_msg.mat);
          data[msq_batt].push(msq_broadcast_msg.batt);
          data[msq_syncnt].push(msq_broadcast_msg.synccnt);
          data[msq_syncreason].push(msq_broadcast_msg.syncreason);
        }   
        break;
      case LOGGING_DATA:
        Serial.println("Writing to SD...");
        timer_start = millis();
        
        
        
        test_file.open(LOG_FILENAME, O_WRITE | O_APPEND);
        for(uint32_t j = 0; j < NUM_DATA_SAMPLES_STORED; j++) 
        {
          time_data.pop(temp_uint32);
          test_file.print(temp_uint32);
          test_file.print(',');
          for(int i = 0; i < accel_temperature; i++)
          {
            if(data[i].isEmpty())
            {
              temp_16int = -1;
            } else
            {
              data[i].pop(temp_16int);
            }

            file.print(temp_16int); file.print(',');
          }
          test_file.println();
        }
        test_file.close();
        Serial.printf("Groups of %d Written: %d\n", NUM_DATA_SAMPLES_STORED, ++write_counter);

        {
          uint32_t max_time = millis() - timer_start;
          Serial.printf("Time to write %d lines of %d * 3 bytes: %f\n", NUM_RAW_SAMPLES_STORED, NUM_DATA_TYPES, ((float)max_time / (float)1000));
        }
        break;
      default:

        break;
    }
  } // end main control loop

} // End main()



// Begin Function Definitions

car_state set_current_state(void)
{
  #ifdef _TEST_

    return TEST;
  #endif

  if(pc.is_connected() && current_state == (NEUTRAL || RUNNING))
  {
    // if(pc.receive_handshake())
    // {
    //   pc.acknowledge_handshake();
    //   return PC_CONNECTED;
    // } 
  }
  else if (ACCEL_INT1_FLAG)
  {
    ACCEL_INT1_FLAG = 0;
    return ACCEL_INT1;
  } 
  else if (BUFFER_FULL_FLAG)
  {
    BUFFER_FULL_FLAG = 0;
    return LOGGING_DATA;
  }
  else if (!pc.is_connected() && current_state != RUNNING)
  {
    return RUNNING; 
  } 
  
  return RUNNING;
}

void initialize_SD(void)
{
     if (!sd.begin(SD_CONFIG)) {
      sd.initErrorHalt(&Serial);
   }
   Serial.println("after sd.begin() ..."); delay(10);
   // Open or create file - truncate existing file.
   if (!file.open(LOG_FILENAME, O_RDWR | O_CREAT | O_TRUNC)) {
      Serial.println("open failed\n");
      return;
   }
   Serial.println("after file.open() ..."); delay(10);

   // File must be pre-allocated to avoid huge
   // delays searching for free clusters.
   if (!file.preAllocate(LOG_FILE_SIZE)) {
      Serial.println("preAllocate failed\n");
      file.close();
      return;
   }

   file.println(DATA_HEADER);
   file.close();
}

void init_accel(void)
{
  acceleromter.begin_accel(ACCEL_CS_PIN);
  if(acceleromter.detect_accel() == true)
  {
    //Serial.println("Device detected now initializing...");


    acceleromter.set_activity_threshold(321);
    acceleromter.set_time_to_active(5);
    acceleromter.disable_FIFO();
    acceleromter.disable_act_inact();
    acceleromter.enable_INT1_data_ready();
    acceleromter.begin_2g_res_measure();
    acceleromter.burst_read_data(temp_array);
    ACCEL_INITIALIZED = 1;

  } else {
    //Serial.println("Accel not initialized...");
  }

}
// Barometers/LPS25HB.h
#ifndef LPS25HB_h
#define LPS25HB_h

#include <Arduino.h>
#include <Wire.h>

#include "Memory.h"
#include "Data.hpp"
#include "Sensor.hpp"

#define LPS25HB_ADDR 0x5D

class LPS25HB : Sensor {

  private:
    const int RES_CONF_REG = 0x10;
    const int CTRL_REG1    = 0x20;
    const int CTRL_REG2    = 0x21;
    
    /* Data Registers */
    const int PRESS_OUT_XL_REG = 0x28;
    const int PRESS_OUT_L_REG  = 0x29;
    const int PRESS_OUT_H_REG  = 0x2A;

    const int TEMP_OUT_L_REG = 0x2B;
    const int TEMP_OUT_H_REG = 0x2C;

    const int WHO_AM_I_REG = 0x0F;

    /* Data */
    float pressure, temperature, altitude;

    const int EMA_SMOOTHING_FACTOR = 0.2;

    void update_sensor() override;

    void read(int registerAddress, byte* data, int bytes=1);

  public:
    void get_data(Data *data) override;
    bool begin() override;

    LPS25HB(int address=LPS25HB_ADDR, TwoWire *wire=&Wire);
};

/** Notes
 * RES_CONF (10h)
 *  - Number of times temp, and pressure are averaged default is 11 (64 times, and 512 times respectively)
 *    - I think we could try 01 (16 times, and 32 times respectively)
 *    - 00000101 or 0x05
 * 
 * CTRL_REG1 (20h)
 *  - Control register 1
 *    -  PD:        leave default 0
 *    -  ODR:       Sample rate we want 1 shot so 000
 *    -  DIFF_EN:   Differential pressure enable idk what that is so default of 0
 *    -  BDU:       leave at default of 0?
 *    -  RESET_AZ:  leave default 0
 *    -  SIM:       leave default 0
 *  - 00000000 or 0x00
 * 
 * CTRL_REG2 (21h)
 *  - Control register 2
 *    - BOOT:           leave default 0
 *    - FIFO_EN:        leave default 0
 *    - STOP_ON_FTH:    leave default 0
 *    - FIFO_MEAN_DEC:  leave default 0
 *    - I2C_DIS:        leave default 0
 *    - SWRESET:        leave default 0
 *    - AUTO_ZERO:      leave default 0
 *    - ONE_SHOT:       set to 1 when I want to take a measurement
 * 
 * Pressure Register (LSB: 28h, CSB: 29h, MSB: 2Ah)
 * Temperature Register (LSB: 2Bh, MSB: 2Ch)
*/

#endif
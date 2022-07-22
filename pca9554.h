/*==============================================================================================================*
 
    @file     PCA9554.h
    @author   Nadav Matalon
    @license  MIT (c) 2016 Nadav Matalon
    PCA9554 Driver (4-Channel GPIO I2C Expander)
    Ver. 1.0.0 - First release (24.10.16)
 *===============================================================================================================*
    INTRODUCTION
 *===============================================================================================================*
 
    The PCA9554 is an 8-Channel GPIO Expander with a hardware I2C interface.
 
    The PCA9554's 8 channels (or IO pins) can be controlled as a single unit or individually in terms of their
    Mode (INPUT /OUTPUT) and Polarity (NON-INVERTED INPUT / INVERTED INPUT). The pins' states (LOW / HIGH) can 
    be read (in INPUT mode) or written (in OUTPUT mode).
    
    This library contains a complete driver for the PCA9554 exposing all the above functionality.
 *===============================================================================================================*
    I2C ADDRESS
 *===============================================================================================================*
    The PCA9536 has 8 addresses, selectable using three address pins. The lower address is 32. In practice, the
    address is shifted left 1 position and a 0 (write) or 1 (read) indicator is added, so to write to address 32
    the shifted address is 64 etc.
 
        PART               DEVICE I2C ADDRESS          PART
       NUMBER          (BIN)      (HEX)     (DEC)     MARKING
      PCA9554D        1000001     0x32       65      PCA9554
*===============================================================================================================*
    REGISTER POINTERS
*===============================================================================================================*
    REG_INPUT           0x00        // Input Port Register           (R)    B00000000 (Default)
    REG_OUTPUT          0x01        // Output Port Register          (R/W)  B00000001
    REG_POLARITY        0x02        // Polarity Inversion Register   (R/W)  B00000010
    REG_CONFIG          0x03        // Configuration Register        (R/W)  B00000011
*===============================================================================================================*
    REGISTER 0: INPUT REGISTER - READ ONLY (0 = LOW / 1 = HIGH)
*===============================================================================================================*
    TODO: CHECK: DEFAULT (WITH NO EXTERNAL INPUT SIGNAL CONNECTED): 'HIGH' (ALL IO PINS HAVE WEAK PULL-UP RESISTORS)
 
                                    DEFAULT
    PIN_IO0             BIT 0          1
    PIN_IO1             BIT 1          1
    PIN_IO2             BIT 2          1
    PIN_IO3             BIT 3          1
    PIN_IO4             BIT 4          1
    PIN_IO5             BIT 5          1
    PIN_IO6             BIT 6          1
    PIN_IO7             BIT 7          1

*===============================================================================================================*
    REGISTER 1: OUTPUT REGISTER - READ / WRITE (0 = LOW / 1 = HIGH)
*===============================================================================================================*
                                    DEFAULT
    IO0                 BIT 0          1
    IO1                 BIT 1          1
    IO2                 BIT 2          1
    IO3                 BIT 3          1
    IO4                 BIT 4          1
    IO5                 BIT 5          1
    IO6                 BIT 6          1
    IO7                 BIT 7          1

*===============================================================================================================*
    REGISTER 2: POLARITY REGISTER - READ / WRITE (0 = NON-INVERTED / 1 = INVERTED)
*===============================================================================================================*
                                    DEFAULT
     PIN_IO0             BIT 0         0
     PIN_IO1             BIT 1         0
     PIN_IO2             BIT 2         0
     PIN_IO3             BIT 3         0
     PIN_IO4             BIT 4         0
     PIN_IO5             BIT 5         0
     PIN_IO6             BIT 6         0
     PIN_IO7             BIT 7         0

*===============================================================================================================*
    REGISTER 3: CONFIGURATION - READ / WRITE (0 = OUTPUT / 1 = INPUT)
*===============================================================================================================*
    POWER-UP DEFAULT: ALL PINS ARE SET AS 'INPUT' (1)
 
                                    DEFAULT
     PIN_IO0             BIT 0         1
     PIN_IO1             BIT 1         1
     PIN_IO2             BIT 2         1
     PIN_IO3             BIT 3         1
     PIN_IO4             BIT 4         1
     PIN_IO5             BIT 5         1
     PIN_IO6             BIT 6         1
     PIN_IO7             BIT 7         1

*===============================================================================================================*
    LICENSE
*===============================================================================================================*
 
    The MIT License (MIT)
    Copyright (c) 2016 Nadav Matalon
    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
    documentation files (the "Software"), to deal in the Software without restriction, including without
    limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
    the Software, and to permit persons to whom the Software is furnished to do so, subject to the following
    conditions:
    The above copyright notice and this permission notice shall be included in all copies or substantial
    portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
    LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*==============================================================================================================*/

#if 1
__asm volatile ("nop");
#endif

#ifndef PCA9554_h
#define PCA9554_h

#include <Arduino.h>
#include "Wire.h"
//#include "utility/PCA9554_PString.h"

namespace Pca9554 {

    const byte DEV_ADDR         = 0x20;
    const byte NUM_BYTES        = 0x01;
    const byte ALL_INPUT        = 0xFF;
    const byte ALL_OUTPUT       = 0x00;
    const byte ALL_LOW          = 0x00;
    const byte ALL_HIGH         = 0xFF;
    const byte ALL_NON_INVERTED = 0x00;
    const byte ALL_INVERTED     = 0xFF;
    const byte COM_SUCCESS      = 0x00;

    typedef enum:byte {
        REG_INPUT    = 0,      // default
        REG_OUTPUT   = 1,
        REG_POLARITY = 2,
        REG_CONFIG   = 3
    } reg_ptr_t;

    typedef enum:byte {
        IO0 = 0,
        IO1 = 1,
        IO2 = 2,
        IO3 = 3,
        IO4 = 4,
        IO5 = 5,
        IO6 = 6,
        IO7 = 7
    } pin_t;

    typedef enum:byte {
        IO_OUTPUT = 0,
        IO_INPUT  = 1
    } mode_t;

    typedef enum:byte {
        IO_LOW  = 0,
        IO_HIGH = 1
    } state_t;

    typedef enum:byte {
        IO_NON_INVERTED = 0,
        IO_INVERTED     = 1
    } polarity_t;

    class PCA9554 {
         public:
            PCA9554(byte hwAddress);
            ~PCA9554();
            byte ping();
            byte getMode(pin_t pin);
            byte getState(pin_t pin);
            byte getPolarity(pin_t pin);
            void setMode(pin_t pin, mode_t newMode);
            void setMode(mode_t newMode);
            void setState(pin_t pin, state_t newState);
            void setState(state_t newState);
            void toggleState(pin_t pin);
            void toggleState();
            void setPolarity(pin_t pin, polarity_t newPolarity);
            void setPolarity(polarity_t newPolarity);
            void reset();
            byte getComResult();
         private:
            byte _comBuffer;
            byte address;
            byte getReg(reg_ptr_t regPtr);
            byte getPin(pin_t pin, reg_ptr_t regPtr);
            void setReg(reg_ptr_t ptr, byte newSetting);
            void setPin(pin_t pin, reg_ptr_t regPtr, byte newSetting);
            void initCall(reg_ptr_t regPtr);
            void endCall();
            //friend PCA9554_PString PCA9554ComStr(const PCA9554&);
            //friend PCA9554_PString PCA9554InfoStr(const PCA9554&);
    };
}

using namespace Pca9554;

#endif

/*==============================================================================================================*
    @file     PCA9554.cpp
    @author   Nadav Matalon, Joakim Tysseng
    @license  MIT (c) 2016 Nadav Matalon
    PCA9554 Driver (8-Channel GPIO I2C Expander)
    Ver. 1.0.0 - First release (22.07.22)
 
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

#include "PCA9554.h"

/*==============================================================================================================*
    CONSTRUCTOR
 *==============================================================================================================*/

PCA9554::PCA9554( TwoWire &selecedWire, byte hwAddress) {
    _comBuffer = COM_SUCCESS;
    address = hwAddress;
    wire = &selecedWire;
}

/*==============================================================================================================*
    DESTRUCTOR
 *==============================================================================================================*/

PCA9554::~PCA9554() {}

/*==============================================================================================================*
    PING (0 = SUCCESS / 1, 2... = ERROR CODE)
 *==============================================================================================================*/

// For meaning of I2C Error Codes see README

byte PCA9554::ping() {
    wire->beginTransmission(DEV_ADDR + address);
    return wire->endTransmission();
}

/*==============================================================================================================*
    GET MODE (0 = OUTPUT / 1 = INPUT)
 *==============================================================================================================*/

byte PCA9554::getMode(pin_t pin) {
    return getPin(pin, REG_CONFIG);
}

byte PCA9554::getState(pin_t pin) {
    return getPin(pin, getMode(pin) ? REG_INPUT : REG_OUTPUT);
}

/*==============================================================================================================*
    GET POLARITY: INPUT PINS ONLY (0 = NON-INVERTED / 1 = INVERTED)
 *==============================================================================================================*/

byte PCA9554::getPolarity(pin_t pin) {
    return getPin(pin, REG_POLARITY);
}

/*==============================================================================================================*
    SET MODE
 *==============================================================================================================*/

void PCA9554::setMode(pin_t pin, mode_t newMode) {                           // PARAMS: IO0 / IO1 / IO2 / IO3
    setPin(pin, REG_CONFIG, newMode);                                        //         IO_INPUT / IO_OUTPUT
}

/*==============================================================================================================*
    SET MODE : ALL PINS
 *==============================================================================================================*/

void PCA9554::setMode(mode_t newMode) {                                      // PARAMS: IO_INPUT / IO_OUTPUT
    setReg(REG_CONFIG, newMode ? ALL_INPUT : ALL_OUTPUT);
}

void PCA9554::setState(pin_t pin, state_t newState) {                        // PARAMS: IO0 / IO1 / IO2 / IO3
    setPin(pin, REG_OUTPUT, newState);                                       //         IO_LOW / IO_HIGH
}

void PCA9554::setState(state_t newState) {                                   // PARAMS: IO_LOW / IO_HIGH
    setReg(REG_OUTPUT, newState ? ALL_HIGH : ALL_LOW);
}

void PCA9554::toggleState(pin_t pin) {
    setReg(REG_OUTPUT, getReg(REG_OUTPUT) ^ (1 << pin));
}

void PCA9554::toggleState() {
    setReg(REG_OUTPUT, ~getReg(REG_OUTPUT));
}

/*==============================================================================================================*
    SET POLARITY (INPUT PINS ONLY)
 *==============================================================================================================*/

void PCA9554::setPolarity(pin_t pin, polarity_t newPolarity) {          // PARAMS: IO0 / IO1 / IO2 / IO3
    setPin(pin, REG_POLARITY, newPolarity);                             //         IO_NON_INVERTED / IO_INVERTED
}

/*==============================================================================================================*
    SET POLARITY : ALL PINS (INPUT PINS ONLY)
 *==============================================================================================================*/

void PCA9554::setPolarity(polarity_t newPolarity) {                     // PARAMS: IO_NON_INVERTED / IO_INVERTED
    byte polarityVals, polarityMask, polarityNew;
    polarityVals = getReg(REG_POLARITY);
    polarityMask = getReg(REG_CONFIG);
    polarityNew  = newPolarity ? ALL_INVERTED : ALL_NON_INVERTED;
    setReg(REG_POLARITY, (polarityVals & ~polarityMask) | (polarityNew & polarityMask));
}

/*==============================================================================================================*
    RESET
 *==============================================================================================================*/

void PCA9554::reset() {
    setMode(IO_INPUT);
    setState(IO_HIGH);
    setPolarity(IO_NON_INVERTED);
    initCall(REG_INPUT);
    endCall();
}

/*==============================================================================================================*
    GET REGISTER DATA
 *==============================================================================================================*/

byte PCA9554::getReg(reg_ptr_t regPtr) {
    byte regData = 0;
    initCall(regPtr);
    endCall();
    if (_comBuffer == COM_SUCCESS) {
        wire->requestFrom((byte)(DEV_ADDR + address), NUM_BYTES);
        if (wire->available() == NUM_BYTES) regData = wire->read();
        else {
            while (wire->available()) wire->read();
            _comBuffer = ping();
        }
    }
    return regData;
}

/*==============================================================================================================*
    GET PIN DATA
 *==============================================================================================================*/

byte PCA9554::getPin(pin_t pin, reg_ptr_t regPtr) {
    return bitRead(getReg(regPtr), pin);
}

/*==============================================================================================================*
    SET REGISTER DATA
 *==============================================================================================================*/

void PCA9554::setReg(reg_ptr_t regPtr, byte newSetting) {
    if (regPtr > 0) {
        initCall(regPtr);
        wire->write(newSetting);
        endCall();
    }
}

/*==============================================================================================================*
    SET PIN DATA
 *==============================================================================================================*/

void PCA9554::setPin(pin_t pin, reg_ptr_t regPtr, byte newSetting) {
    byte newReg = getReg(regPtr);
    bitWrite(newReg, pin, newSetting);
    setReg(regPtr, newReg);
}

/*==============================================================================================================*
    INITIATE I2C COMMUNICATION
 *==============================================================================================================*/

void PCA9554::initCall(reg_ptr_t regPtr) {
    wire->beginTransmission(DEV_ADDR + address);
    wire->write(regPtr);
}

/*==============================================================================================================*
    END I2C COMMUNICATION
 *==============================================================================================================*/

void PCA9554::endCall() {
    _comBuffer = wire->endTransmission();
}

/*==============================================================================================================*
    GET COMMUNICATION RESULT
 *==============================================================================================================*/

byte PCA9554::getComResult() {
    return _comBuffer;
}

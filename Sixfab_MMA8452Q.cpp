#include "Sixfab_MMA8452Q.h"
#include <Arduino.h> 
#include <Wire.h>

MMA8452Q::MMA8452Q(byte addr)
{
    address = addr;
}

byte MMA8452Q::init(MMA8452Q_Scale fsr, MMA8452Q_ODR odr) {
  scale = fsr; // Haul fsr into our class variable, scale

  Wire.begin(); // Initialize I2C

  byte c = readRegister(WHO_AM_I); // Read WHO_AM_I register

  if (c != 0x2A) // WHO_AM_I should always be 0x2A
  {
    return 0;
  }

  standby(); // Must be in standby to change registers
  setScale(scale); // Set up accelerometer scale
  setODR(odr); // Set up output data rate
  setupPL(); // Set up portrait/landscape detection
  // Multiply parameter by 0.0625g to calculate threshold.
  setupTap(0x80, 0x80, 0x08); // Disable x, y, set z to 0.5g

  active(); // Set to active to start reading

  return 1;
}

void MMA8452Q::read() {
  byte rawData[6]; // x/y/z accel register data stored here

  readRegisters(OUT_X_MSB, rawData, 6); // Read the six raw data registers into data array

  x = (rawData[0] << 8 | rawData[1]) >> 4;
  y = (rawData[2] << 8 | rawData[3]) >> 4;
  z = (rawData[4] << 8 | rawData[5]) >> 4;
  cx = (float) x / (float)(1 << 11) * (float)(scale);
  cy = (float) y / (float)(1 << 11) * (float)(scale);
  cz = (float) z / (float)(1 << 11) * (float)(scale);
}

byte MMA8452Q::available() {
  return (readRegister(STATUS) & 0x08) >> 3;
}

void MMA8452Q::setScale(MMA8452Q_Scale fsr) {
  byte cfg = readRegister(XYZ_DATA_CFG);
  cfg &= 0xFC;
  cfg |= (fsr >> 2);
  writeRegister(XYZ_DATA_CFG, cfg);
}

void MMA8452Q::setODR(MMA8452Q_ODR odr) {
  byte ctrl = readRegister(CTRL_REG1);
  ctrl &= 0xCF;
  ctrl |= (odr << 3);
  writeRegister(CTRL_REG1, ctrl);
}

void MMA8452Q::setupTap(byte xThs, byte yThs, byte zThs) {
  byte temp = 0;
  if (!(xThs & 0x80)) {
    temp |= 0x3; // Enable taps on x
    writeRegister(PULSE_THSX, xThs); // x thresh
  }
  if (!(yThs & 0x80)) {
    temp |= 0xC; // Enable taps on y
    writeRegister(PULSE_THSY, yThs); // y thresh
  }
  if (!(zThs & 0x80)) {
    temp |= 0x30; // Enable taps on z
    writeRegister(PULSE_THSZ, zThs); // z thresh
  }

  writeRegister(PULSE_CFG, temp | 0x40);
  writeRegister(PULSE_TMLT, 0x30);
  writeRegister(PULSE_LTCY, 0xA0);
  writeRegister(PULSE_WIND, 0xFF);
}

byte MMA8452Q::readTap() {
  byte tapStat = readRegister(PULSE_SRC);
  if (tapStat & 0x80) // Read EA bit to check if a interrupt was generated
  {
    return tapStat & 0x7F;
  } else
    return 0;
}

void MMA8452Q::setupPL() {

  writeRegister(PL_CFG, readRegister(PL_CFG) | 0x40);
  writeRegister(PL_COUNT, 0x50);
}

byte MMA8452Q::readPL() {
  byte plStat = readRegister(PL_STATUS);

  if (plStat & 0x40)
    return LOCKOUT;
  else
    return (plStat & 0x6) >> 1;
}

void MMA8452Q::standby() {
  byte c = readRegister(CTRL_REG1);
  writeRegister(CTRL_REG1, c & ~(0x01)); //Clear the active bit to go into standby
}

// SET ACTIVE MODE
//	Sets the MMA8452 to active mode. Needs to be in this mode to output data
void MMA8452Q::active() {
  byte c = readRegister(CTRL_REG1);
  writeRegister(CTRL_REG1, c | 0x01); //Set the active bit to begin detection
}

// WRITE A SINGLE REGISTER
// 	Write a single byte of data to a register in the MMA8452Q.
void MMA8452Q::writeRegister(MMA8452Q_Register reg, byte data) {
  writeRegisters(reg, & data, 1);
}

void MMA8452Q::writeRegisters(MMA8452Q_Register reg, byte * buffer, byte len) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  for (int x = 0; x < len; x++)
    Wire.write(buffer[x]);
  Wire.endTransmission();
}

byte MMA8452Q::readRegister(MMA8452Q_Register reg) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(address, (byte) 1);
  while (!Wire.available());
  return Wire.read();
}

void MMA8452Q::readRegisters(MMA8452Q_Register reg, byte * buffer, byte len) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(address, len);
  while (Wire.available() < len);
  for (int x = 0; x < len; x++)
    buffer[x] = Wire.read();
}

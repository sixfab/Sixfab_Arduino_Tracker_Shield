
#include <Wire.h>
#include "Sixfab_HDC1080.h"

Sixfab_HDC1080::Sixfab_HDC1080(){

}

void Sixfab_HDC1080::begin(uint8_t address){
	_address = address;
	Wire.begin();

	setResolution(SIXFAB_HDC1080_RESOLUTION_14BIT, SIXFAB_HDC1080_RESOLUTION_14BIT);
}

void Sixfab_HDC1080::setResolution(HDC1080_MeasurementResolution humidity, HDC1080_MeasurementResolution temperature) {
	HDC1080_Registers reg;
	reg.HumidityMeasurementResolution = 0;
	reg.TemperatureMeasurementResolution = 0;

	if (temperature == SIXFAB_HDC1080_RESOLUTION_11BIT)
		reg.TemperatureMeasurementResolution = 0x01;

	switch (humidity)
	{
		case SIXFAB_HDC1080_RESOLUTION_8BIT:
			reg.HumidityMeasurementResolution = 0x02;
			break;
		case SIXFAB_HDC1080_RESOLUTION_11BIT:
			reg.HumidityMeasurementResolution = 0x01;
			break;
		default:
			break;
	}

	writeRegister(reg);
}

HDC1080_SerialNumber Sixfab_HDC1080::readSerialNumber() {
	HDC1080_SerialNumber sernum;
	sernum.serialFirst = readData(HDC1080_SERIAL_ID_FIRST);
	sernum.serialMid = readData(HDC1080_SERIAL_ID_MID);
	sernum.serialLast = readData(HDC1080_SERIAL_ID_LAST);
	return sernum;
}

HDC1080_Registers Sixfab_HDC1080::readRegister() {
	HDC1080_Registers reg;
	reg.rawData = (readData(HDC1080_CONFIGURATION) >> 8);
	return reg;
}

void Sixfab_HDC1080::writeRegister(HDC1080_Registers reg) {
	Wire.beginTransmission(_address);
	Wire.write(HDC1080_CONFIGURATION);
	Wire.write(reg.rawData);
	Wire.write(0x00);
	Wire.endTransmission();
	delay(10);
}

void Sixfab_HDC1080::heatUp(uint8_t seconds) {
	HDC1080_Registers reg = readRegister();
	reg.Heater = 1;
	reg.ModeOfAcquisition = 1;
	writeRegister(reg);

	uint8_t buf[4];
	for (int i = 1; i < (seconds*66); i++) {
		Wire.beginTransmission(_address);
		Wire.write(0x00);
		Wire.endTransmission();
		delay(20);
		Wire.requestFrom(_address, (uint8_t)4);
		Wire.readBytes(buf, (size_t)4);
	}
	reg.Heater = 0;
	reg.ModeOfAcquisition = 0;
	writeRegister(reg);
}



double Sixfab_HDC1080::readT() {
	return readTemperature();
}

double Sixfab_HDC1080::readTemperature() {
	uint16_t rawT = readData(HDC1080_TEMPERATURE);
	return (rawT / pow(2, 16)) * 165.0 - 40.0;
}

double Sixfab_HDC1080::readH() {
	return readHumidity();
}

double Sixfab_HDC1080::readHumidity() {
	uint16_t rawH = readData(HDC1080_HUMIDITY);
	return (rawH / pow(2, 16)) * 100.0;
}

uint16_t Sixfab_HDC1080::readManufacturerId() {
	return readData(HDC1080_MANUFACTURER_ID);
}

uint16_t Sixfab_HDC1080::readDeviceId() {
	return readData(HDC1080_DEVICE_ID);
}

uint16_t Sixfab_HDC1080::readData(uint8_t pointer) {
	Wire.beginTransmission(_address);
	Wire.write(pointer);
	Wire.endTransmission();
	
	delay(9);
	Wire.requestFrom(_address, (uint8_t)2);

	byte msb = Wire.read();
	byte lsb = Wire.read();

	return msb << 8 | lsb;
}
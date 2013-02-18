#pragma once

#include "cinder/Serial.h"
#include "Sensor.h"

namespace HeartRate
{

class SensorDevice : public Sensor
{
public:
	SensorDevice();

	bool init( const std::string &name, int baudRate );

	bool        hasMessage();
	void        getMessage( char &type, std::string &data );

	static void printDevices();
	static std::vector<std::string> getDeviceNames();

protected:
	ci::Serial  mSerial;
};


} //namespace HeartRate


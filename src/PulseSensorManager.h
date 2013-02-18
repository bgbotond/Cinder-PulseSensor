#pragma once

#include "mndlkit/params/PParams.h"
#include "PulseSensor.h"

namespace HeartRate
{

class PulseSensorManager
{
public:
	PulseSensorManager(){};

	void setup();
	void update();

	template<typename T>
	void addCallback( int device, PulseSensor::MessageType messageType, void (T::* callbackFunction)( int value ), T* callbackObject )
	{
		if( device < 0 && device >= PULSE_SENSOR_COUNT )
			return;

		mPulseSensors[device].addCallback<T>( messageType, callbackFunction, callbackObject );
	}

protected:
	void initParams();

	static const int BAUD_RATE          = 57600;
	static const int PULSE_SENSOR_COUNT = 2;

protected:
	PulseSensor mPulseSensors[PULSE_SENSOR_COUNT];

	// available devices
	mndl::kit::params::PInterfaceGl mParams;
	std::vector<std::string>        mDeviceNames;
	std::vector<std::string>        mFileNames;
	std::vector<std::string>        mAllNames;
	int                             mDevices[PULSE_SENSOR_COUNT];
	int                             mDevicesSave[PULSE_SENSOR_COUNT];
};


} //namespace HeartRate


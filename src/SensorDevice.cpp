
#include "cinder/app/AppBasic.h"
#include "SensorDevice.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace HeartRate
{

SensorDevice::SensorDevice()
: Sensor()
, mSerial()
{
}

bool SensorDevice::init( const string &deviceName, int baudRate )
{
	if( mInited )
		return false;

	try
	{
		Serial::Device dev = Serial::findDeviceByNameContains( deviceName );
		mSerial = Serial( dev, baudRate );
		mSerial.flush();
		mName = mSerial.getDevice().getName();
		mInited = true;
		return true;
	}
	catch( ... )
	{
		console() << "There was an error initializing the serial device: " << deviceName << endl;
	}

	return false;
}

bool SensorDevice::hasMessage()
{
	if( ! mInited )
		return false;

	if( mSerial.getNumBytesAvailable())
		return true;

	return false;
}

void SensorDevice::getMessage( char &type, std::string &data )
{
	if( ! mInited )
	{
		type = '\0';
		data.clear();
	}
	else
	{
		type = (char) mSerial.readByte();
		data = mSerial.readStringUntil( '\n', 0, 0.1 );
		mSerial.flush();
		assert( hasMessage() == false );
	}
}

void SensorDevice::printDevices()
{
	const vector<Serial::Device> &devices( Serial::getDevices());

	for( vector<Serial::Device>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt )
	{
		console() << "Device: " << deviceIt->getName() << endl;
	}
}

vector<string> SensorDevice::getDeviceNames()
{
	vector<string> deviceNames;
	const vector<Serial::Device> &devices( Serial::getDevices());
	for( vector<Serial::Device>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt )
	{
		deviceNames.push_back( deviceIt->getName());
	}

	return deviceNames;
}

} // namespace HeartRate

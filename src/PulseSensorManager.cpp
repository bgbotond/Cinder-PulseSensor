#include <sstream>

#include "PulseSensorManager.h"
#include "Util.h"
#include "SensorDevice.h"
#include "SensorFile.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace HeartRate
{

void PulseSensorManager::setup()
{
	initParams();

	Vec2i pos( 240, 20 );

	for( int i = 0; i < PULSE_SENSOR_COUNT; ++i )
	{
		std::stringstream ss;
		ss << "Sensor" << i;

		mPulseSensors[i].setup( ss.str(), pos + i * Vec2i( 0, 130 ) );
	}
}

void PulseSensorManager::initParams()
{
	mDeviceNames = SensorDevice::getDeviceNames();
	mFileNames   = SensorFile::getFileNames();

	mAllNames.push_back( PulseSensor::NO_DEVICE );
	for( vector<string>::iterator it = mFileNames.begin(); it != mFileNames.end(); ++it )
	{
		mAllNames.push_back( *it );
	}
	for( vector<string>::iterator it = mDeviceNames.begin(); it != mDeviceNames.end(); ++it )
	{
		mAllNames.push_back( *it );
	}

	mParams = mndl::kit::params::PInterfaceGl( "PulseSensorManager", Vec2i( 200, 120 ), Vec2i( 20, 120 ) );
	mParams.addPersistentSizeAndPosition();

	mParams.addText( "Available devices" );
	for( int i = 0; i < PULSE_SENSOR_COUNT; ++i )
	{
		std::stringstream ss;
		ss << "Sensor" << i;

		mParams.addPersistentParam( ss.str(), mAllNames, &mDevices[i], 0 );

		mDevices[i] = 0;
		mDevicesSave[i] = 0;
	}
}

void PulseSensorManager::update()
{
	for( int i = 0; i < PULSE_SENSOR_COUNT; ++i )
	{
		if( mDevices[i] != mDevicesSave[i] )
		{
			if( mDevices[i] == 0 )
			{
				mPulseSensors[i].deinit();
			}
			else
			{
				string name;
				if( mDevices[i] - 1 < (int)mFileNames.size() )
				{
					name = mFileNames[ mDevices[i] -1 ];
					mPulseSensors[i].init( name );
				}
				else
				{
					name = mDeviceNames[ mDevices[i] - mFileNames.size() - 1 ];
					mPulseSensors[i].init( name, BAUD_RATE );
				}
			}
			mDevicesSave[i] = mDevices[i];
		}

		mPulseSensors[i].update();
	}
}

void PulseSensorManager::draw()
{
	for( int i = 0; i < PULSE_SENSOR_COUNT; ++i )
	{
		mPulseSensors[i].draw();
	}
}

bool PulseSensorManager::mouseDown( MouseEvent event )
{
	for( int i = 0; i < PULSE_SENSOR_COUNT; ++i )
	{
		if( mPulseSensors[i].mouseDown( event ) )
			return true;
	}

	return false;
}

bool PulseSensorManager::mouseDrag( MouseEvent event )
{
	for( int i = 0; i < PULSE_SENSOR_COUNT; ++i )
	{
		if( mPulseSensors[i].mouseDrag( event ) )
			return true;
	}

	return false;
}

} // namespace HeartRate

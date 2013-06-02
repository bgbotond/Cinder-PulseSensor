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
	for( int i = 0; i < PULSE_SENSOR_COUNT; ++i )
	{
		stringstream ss;
		ss << "Sensor" << i;

		mPulseSensors[i].setup( ss.str() );
	}

	initParams();
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

	mParams = mndl::params::PInterfaceGl( "PulseSensorManager", Vec2i( 300, 180 ), Vec2i( 326, 150 ) );
	mParams.addPersistentSizeAndPosition();

	mParams.addText( "Available devices" );
	for( int i = 0; i < PULSE_SENSOR_COUNT; ++i )
	{
		stringstream ss;
		ss << "Sensor" << i;

		mDevices[i] = 0;
		mDevicesSave[i] = 0;

		mParams.addPersistentParam( ss.str(), mAllNames, &mDevices[i], 0 );

		mDeviceRecordings[i]     = false;
		mDeviceRecordingsSave[i] = false;

		stringstream ss2;
		ss2 << ss.str() << " Recording";
		mParams.addParam( ss2.str(), &mDeviceRecordings[i], "", false );

		stringstream ss3;
		ss3 << ss.str() << " Capture file name";
		mDeviceRecordingCaptureFileNames[i] = ss.str() + "capture-out.xml";
		mParams.addParam( ss3.str(), &mDeviceRecordingCaptureFileNames[i], "", false );
	}

	mParams.addPersistentParam( "Min data height", &mMinDataHeight, Cardioid::FBO_HEIGHT / 2, "min=0 max=128"            );
	mParams.addPersistentParam( "Smooth data"    , &mSmoothData   , 0.3f                    , "min=0.0 max=1.0 step=.01" );
}

void PulseSensorManager::update()
{
	for( int i = 0; i < PULSE_SENSOR_COUNT; ++i )
	{
		mPulseSensors[i].setMinDataHeight( mMinDataHeight );
		mPulseSensors[i].setSmoothData( mSmoothData );

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

		if( mDeviceRecordings[i] != mDeviceRecordingsSave[i] )
		{
			if( mDeviceRecordings[i] )
				mPulseSensors[i].startRecording( mDeviceRecordingCaptureFileNames[i] );
			else
				mPulseSensors[i].stopRecording();
		}

		mDeviceRecordings[i] = mPulseSensors[i].isRecording();
		mDeviceRecordingsSave[i] = mDeviceRecordings[i];

		mPulseSensors[i].update();
	}
}

void PulseSensorManager::draw()
{
	mParams.draw();

// 	for( int i = 0; i < PULSE_SENSOR_COUNT; ++i )
// 	{
// 		drawFboTest( i );
// 	}
}

void PulseSensorManager::drawFboTest( int sensor )
{
	ci::gl::Fbo* fbo = mPulseSensors[sensor].getFbo();

	Area area = Area( Vec2i( 100, 100 + ( sensor * ( Cardioid::FBO_HEIGHT + 10 ) ) ), Vec2i( 100 + Cardioid::FBO_WIDTH, 100 + Cardioid::FBO_HEIGHT + ( sensor * ( Cardioid::FBO_HEIGHT + 10 ) ) ) );

	glEnable( GL_TEXTURE_2D );
	fbo->bindTexture();
	gl::draw( fbo->getTexture(0), area );
	fbo->unbindTexture();
}

} // namespace HeartRate

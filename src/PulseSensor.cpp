
#include <string>
#include "cinder/app/AppBasic.h"

#include "PulseSensor.h"
#include "SensorDevice.h"
#include "SensorFile.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace HeartRate
{
const string PulseSensor::NO_DEVICE = "no device";

PulseSensor::PulseSensor()
: mSensor( 0 )
, mRecorder()
, mBeatPerMinute( 0 )
, mSensorData( 0 )
, mBeatPauseTime( 0 )
, mSensorName()
, mDeviceName( NO_DEVICE )
{
}

void PulseSensor::setup( const std::string &sensorName )
{
	mSensorName = sensorName;

	mCardioid.setup();
	addCallback< Cardioid >( MT_SensorData, &Cardioid::addData, &mCardioid );
}

const std::string& PulseSensor::getSensorName() const
{
	return mSensorName;
}

const std::string& PulseSensor::getDeviceName() const
{
	return mDeviceName;
}

bool PulseSensor::init( const string &deviceName, int baudRate )
{
	stopRecording();
	mCardioid.clear();

	SensorDevice *sensorDevice = new SensorDevice();

	if( sensorDevice->init( deviceName, baudRate ))
	{
		if( mSensor )
			delete mSensor;

		mSensor = sensorDevice;
		mDeviceName = mSensor->getName();
		return true;
	}
	else
	{
		delete sensorDevice;
	}

	return false;
}

bool PulseSensor::init( const string &fileName )
{
	stopRecording();
	mCardioid.clear();

	SensorFile *sensorFile = new SensorFile();

	if( sensorFile->init( fileName ))
	{
		if( mSensor )
			delete mSensor;

		mSensor = sensorFile;
		mDeviceName = mSensor->getName();
		return true;
	}
	else
	{
		delete sensorFile;
	}

	return false;
}

bool PulseSensor::deinit()
{
	if( ! mSensor )
		return false;

	stopRecording();

	mDeviceName = NO_DEVICE;
	mCardioid.clear();
	delete mSensor;
	mSensor = 0;

	return true;
}

void PulseSensor::update()
{
	if( ! mSensor || ! mSensor->isInited())
		return;

	if( mSensor->hasMessage())
	{
		try
		{
			char   messageId;
			string message;

			mSensor->getMessage( messageId, message );

			processMessage( messageId, message );
		}
		catch( ... )
		{
			console() << "Timeout while reading input from serial device!" << endl;
		}
	}
}

void PulseSensor::setMinDataHeight( int minDataHeight )
{
	mCardioid.setMinDataHeight( minDataHeight );
}

int PulseSensor::getMinDataHeight() const
{
	return mCardioid.getMinDataHeight();
}

void PulseSensor::setSmoothData( float smoothData )
{
	mCardioid.setSmoothData( smoothData );
}

float PulseSensor::getSmoothData() const
{
	return mCardioid.getSmoothData();
}

ci::gl::Fbo *PulseSensor::getFbo()
{
	return mCardioid.getFbo();
}

ci::gl::Texture PulseSensor::getPulseTexture()
{
	return mCardioid.getFbo()->getTexture();
}

int PulseSensor::getBeatPerMinute()
{
	return mBeatPerMinute;
}

int PulseSensor::getSensorData()
{
	return mSensorData;
}

int PulseSensor::getBeatPauseTime()
{
	return mBeatPauseTime;
}

void PulseSensor::startRecording( const std::string &fileName )
{
	if( mSensor == 0 )
		return;

	if( isRecording())
		return;

	mRecorder.startRecording( fileName, mSensor->getName());
}

void PulseSensor::stopRecording()
{
	if( mSensor == 0 )
		return;

	if( ! isRecording())
		return;

	mRecorder.stopRecording();
}

bool PulseSensor::isRecording()
{
	if( mSensor == 0 )
		return false;

	return mRecorder.isRecording();
}

void PulseSensor::processMessage( const char messageId, const std::string message )
{
	MessageType messageType  = convertCharToMessageType( messageId );
	int         messageValue = atoi( message.c_str());

	switch( messageType )
	{
	case MT_BeatPerMinute : mBeatPerMinute = messageValue; break;
	case MT_SensorData    : mSensorData    = messageValue; break;
	case MT_BeatPauseTime : mBeatPauseTime = messageValue; break;
	default               : console() << "unknown message: " << message << endl; return;
	}

	mListener.callCallback( (int)messageType, messageValue );

	if( mRecorder.isRecording())
		mRecorder.dataRecording( getElapsedSeconds(), messageId, message );
}

PulseSensor::MessageType PulseSensor::convertCharToMessageType( const char messageId )
{
	MessageType messageType = MT_Unknow;

	switch( messageId )
	{
	case 'Q': messageType = MT_BeatPerMinute; break; // 'Q' - current heart rate (beat per minute)
	case 'S': messageType = MT_SensorData;    break; // 'S' - raw sensor data
	case 'B': messageType = MT_BeatPauseTime; break; // 'B' - time between beats in milliseconds (ms time between beats)
	case '?': messageType = MT_Unknow;        break; // '?' - Unknown message type
	default : messageType = MT_Unknow;        break; // '?' - Unknown message type
	}

	return messageType;
}

char PulseSensor::convertMessageTypeToChar( const MessageType messageType )
{
	char messageId = '?';

	switch( messageType )
	{
	case MT_BeatPerMinute : messageId = 'Q'; break; // 'Q' - current heart rate (beat per minute)
	case MT_SensorData    : messageId = 'S'; break; // 'S' - raw sensor data
	case MT_BeatPauseTime : messageId = 'B'; break; // 'B' - time between beats in milliseconds (ms time between beats)
	case MT_Unknow        : messageId = '?'; break; // '?' - Unknown message type
	default               : messageId = '?'; break; // '?' - Unknown message type
	}

	return messageId;
}

} // namespace HeartRate

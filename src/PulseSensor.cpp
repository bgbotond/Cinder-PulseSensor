
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
, mRecording( false )
, mBeatPerMinute( 0 )
, mSensorData( 0 )
, mBeatPauseTime( 0 )
, mSensorName()
, mDeviceName( NO_DEVICE )
, mMonitor( 0 )
, mCardioid()
, mVisible( false )
, mFactorX( 0.0f )
, mFactorY( 0.0f )
, mOffset( 0 )
{
}

void PulseSensor::setup( const std::string &sensorName, const Vec2i &pos )
{
	mSensorName = sensorName;

	initParam( pos );
}

void PulseSensor::initParam( const Vec2i &pos )
{
	mOutputFileName = "pulsesensor-capture-out" + mSensorName + ".xml";
	mRecording      = false;

	mCardioid.setup();

	mMonitor = new Monitor( mDeviceName, pos + Vec2i( 300, 0 ), Vec2i( 350, 300 ));
	mMonitor->AddSection( "Pulse sensor data"                    );
	mMonitor->AddSection( "BeatPerMinute"    , &mBeatPerMinute   );
	mMonitor->AddSection( "SensorData"       , &mSensorData      );
	mMonitor->AddSection( "BeatPauseTimey"   , &mBeatPauseTime   );
	mMonitor->AddSection( "Cardioid"                             );
	mMonitor->AddSection( "Data"             , mCardioid.getFbo());
	addCallback<Cardioid>( MT_SensorData, &Cardioid::addData, &mCardioid );

	mParams = mndl::kit::params::PInterfaceGl( mSensorName, Vec2i( 230, 120 ), pos );
	mParams.addPersistentSizeAndPosition();

	// debug
	mParams.addText( "Device" );
	mParams.addParam( "DeviceName"     , &mDeviceName, "", true );
	mParams.addText( "Recorder" );
	mParams.addParam( "OutputFileName", &mOutputFileName, "", false );
	mParams.addParam( "Recording"     , &mRecording     , "", false );
	mParams.addText( "Cardioid" );
	mParams.addPersistentParam( "Visible", &mVisible, true );
	mParams.addPersistentParam( "FactorX", &mFactorX, mCardioid.getFactorX(), "min=0.01 max=1.0 step=0.05", false );
	mParams.addPersistentParam( "FactorY", &mFactorY, mCardioid.getFactorY(), "min=0.01 max=1.0 step=0.05", false );
	mParams.addPersistentParam( "Offset" , &mOffset , mCardioid.getOffset() , "min=0    max=900 step=10  ", false );
}

void PulseSensor::updateParam()
{
	if( isRecording() != mRecording )
	{
		if( mRecording )
			startRecording( mOutputFileName );
		else
			stopRecording();
	}

	if( mCardioid.getFactorX() != mFactorX )
		mCardioid.setFactorX( mFactorX );

	if( mCardioid.getFactorY() != mFactorY )
		mCardioid.setFactorY( mFactorY );

	if( mCardioid.getOffset() != mOffset )
		mCardioid.setOffset( mOffset );
}

bool PulseSensor::init( const string &deviceName, int baudRate )
{
	SensorDevice *sensorDevice = new SensorDevice();

	if( sensorDevice->init( deviceName, baudRate ))
	{
		if( mSensor )
			delete mSensor;

		mSensor = sensorDevice;
		mDeviceName = mSensor->getName();
		if( mMonitor )
			mMonitor->setName( mDeviceName );
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
	SensorFile *sensorFile = new SensorFile();

	if( sensorFile->init( fileName ))
	{
		if( mSensor )
			delete mSensor;

		mSensor = sensorFile;
		mDeviceName = mSensor->getName();
		if( mMonitor )
			mMonitor->setName( mDeviceName );
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

	mDeviceName = NO_DEVICE;
	if( mMonitor )
		mMonitor->setName( mDeviceName );
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

	updateParam();
}

void PulseSensor::draw()
{
	if( ! mSensor || ! mSensor->isInited())
		return;

	if( mVisible && mMonitor && mParams.isVisible())
		mMonitor->draw();
}

bool PulseSensor::mouseDown( MouseEvent event )
{
	if( mMonitor )
		return mMonitor->mouseDown( event );

	return false;
}

bool PulseSensor::mouseDrag( MouseEvent event )
{
	if( mMonitor )
		return mMonitor->mouseDrag( event );

	return false;
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
	mRecorder.stopRecording();
}

bool PulseSensor::isRecording()
{
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

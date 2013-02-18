#pragma once

#include "cinder/Xml.h"

#include "mndlkit/params/PParams.h"
#include "Sensor.h"
#include "Recorder.h"
#include "Listener.h"

namespace HeartRate
{

class PulseSensor
{
public:
	enum MessageType
	{
		MT_BeatPerMinute,              // 'Q' - current heart rate (beat per minute)
		MT_SensorData,                 // 'S' - raw sensor data
		MT_BeatPauseTime,              // 'B' - time between beats in milliseconds (ms time between beats)
		MT_Unknow,                     //  ?  - unknown message type
	};

public:
	PulseSensor();

	bool init( const std::string &deviceName, int baudRate );
	bool init( const std::string &fileName                 );
	bool deinit();

	void setup( const std::string &sensorName, const ci::Vec2i &pos );
	void update();

	int  getBeatPerMinute();
	int  getSensorData();
	int  getBeatPauseTime();

	void startRecording( const std::string &fileName );
	void stopRecording();
	bool isRecording();

	template<typename T>
	void addCallback( MessageType messageType, void (T::* callbackFunction)( int value ), T* callbackObject )
	{
		mListener.addCallback<T>( (int)messageType, callbackFunction, callbackObject );
	}

	static const std::string NO_DEVICE;

protected:
	void         processMessage( const char messageId, const std::string message );

	MessageType  convertCharToMessageType( const char        messageId   );
	char         convertMessageTypeToChar( const MessageType messageType );

	void         initParam( const ci::Vec2i &pos );
	void         updateParam();

protected:
	std::string               mSensorName;
	Sensor                   *mSensor;
	Recorder                  mRecorder;
	int                       mBeatPerMinute;
	int                       mSensorData;
	int                       mBeatPauseTime;
	Listener                  mListener;

	// params
	mndl::kit::params::PInterfaceGl mParams;
	// recordnig
	std::string               mOutputFileName;
	bool                      mRecording;
	std::string               mDeviceName;
};


} //namespace HeartRate


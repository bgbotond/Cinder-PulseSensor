#pragma once

#include <string>
#include <vector>

namespace HeartRate
{

class Sensor
{
public:
	Sensor();
	virtual ~Sensor();

	const bool          isInited();
	const std::string   getName();

	virtual bool        hasMessage()   = 0;
	virtual void        getMessage( char &type, std::string &data ) = 0;

protected:
	bool        mInited;
	std::string mName;
};

} //namespace HeartRate

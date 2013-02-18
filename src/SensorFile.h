#pragma once

#include "cinder/Xml.h"
#include "Sensor.h"

namespace HeartRate
{

class SensorFile : public Sensor
{
public:
	SensorFile();

	bool init( const std::string &fileName );

	bool        hasMessage();
	void        getMessage( char &type, std::string &data );

	static std::vector<std::string> getFileNames();

protected:
	ci::XmlTree                      mDoc;
	ci::XmlTree::Container          *mListXmlData;
	ci::XmlTree::Container::iterator mXmlDataActIt;
	double                           mTime;
};

} //namespace HeartRate

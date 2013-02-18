
#include "cinder/app/AppBasic.h"
#include "SensorFile.h"
#include "Util.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace HeartRate
{

SensorFile::SensorFile()
: Sensor()
, mListXmlData( 0 )
, mXmlDataActIt()
, mTime( 0.0 )
{
}

bool SensorFile::init( const string &fileName )
{
	if( mInited )
		return false;

	fs::path fullPath( getAssetPath( string( "capture/" ) + fileName ));
	if( ! fs::exists( fullPath ))
	{
		console() << "Unable to load captured file: " << fileName << endl;
		return false;
	}

	mDoc = XmlTree( loadFile( fullPath ));

	if( mDoc.hasChild( "PulseSensor" ))
	{
		XmlTree &xmlPulseSensor = mDoc.getChild( "PulseSensor" );
		mName = xmlPulseSensor.getAttributeValue<string>( "Name"    , "" );

		if( xmlPulseSensor.hasChild( "Data" ))
		{
			mListXmlData   = &xmlPulseSensor.getChildren();
			mXmlDataActIt  =  mListXmlData->end();
			mInited        =  true;
			mTime          =  getElapsedSeconds();
			return true;
		}
	}

	console() << "Invalid captured file: " << fileName << endl;
	return false;
}

bool SensorFile::hasMessage()
{
	if( ! mInited )
		return false;

	if( ! mListXmlData || mListXmlData->size() == 0 )
		return false;

	ci::XmlTree::Container::iterator xmlDataActIt = mXmlDataActIt;

	if( xmlDataActIt == mListXmlData->end())
	{
		xmlDataActIt = mListXmlData->begin();
	}
	else
	{
		++xmlDataActIt;
		if( xmlDataActIt == mListXmlData->end())
			xmlDataActIt = mListXmlData->begin();
	}

	double time = (*xmlDataActIt)->getAttributeValue<double>( "Time", 0.0 );

	if( time + mTime <= getElapsedSeconds())
	{
		mXmlDataActIt = xmlDataActIt;
		if( mXmlDataActIt == mListXmlData->begin())
			mTime = getElapsedSeconds();
		return true;
	}

	return false;
}

void SensorFile::getMessage( char &type, std::string &data )
{
	if( ! mInited )
	{
		type = '\0';
		data.clear();
	}
	else
	{
		type = (*mXmlDataActIt)->getAttributeValue<char>( "Type", '\0' );
		data = (*mXmlDataActIt)->getAttributeValue<string>( "Data", "" );
	}
}

vector<string> SensorFile::getFileNames()
{
	vector<string> fileNames;

	fs::path capturePath = getPath( "capture" );
	fs::directory_iterator endIter;

	for( fs::directory_iterator it( capturePath ) ; it != endIter; ++it )
	{
		if( fs::is_regular_file( it->status() ) )
		{
			if( it->path().extension() == ".xml" )
				fileNames.push_back( it->path().filename().string());
// 			it->status()
// 			result_set.insert(result_set_t::value_type(fs::last_write_time(dir_iter->status()), *dir_iter);
		}
	}
// 	const vector<Serial::Device> &devices( Serial::getDevices());
// 	for( vector<Serial::Device>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt )
// 	{
// 		deviceNames.push_back( deviceIt->getName());
// 	}

	return fileNames;
}

} // namespace HeartRate

#pragma once

#include "cinder/Vector.h"
#include "cinder/gl/Fbo.h"

namespace HeartRate
{

class Cardioid
{
public:
	static const int FBO_WIDTH  = 256;
	static const int FBO_HEIGHT = 128;

public:
	Cardioid();

	void            addData( int data );
	void            clear();

	void setMinDataHeight( int minDataHeight );
	int  getMinDataHeight() const;
	void  setSmoothData( float smoothData );
	float getSmoothData() const;
	ci::gl::Fbo *getFbo();

	void setup();

protected:
	void smoothData();
	void updateFbo();

protected:
	ci::gl::Fbo       mFbo;
	std::deque< int > mData;
	int               mDataMaxLength;

	float             mFactor;     // factor to display Data on for y axis
	float             mOffset;     // offset to display Data on for y axis
	int               mMinDataHeight;
	float             mSmoothData;
};


} //namespace HeartRate


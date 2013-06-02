
#include "cinder/gl/gl.h"
#include "cinder/CinderMath.h"
#include "Cardioid.h"

using namespace ci;
using namespace std;

namespace HeartRate
{
/*
	ci::lerp interpolal egy megadott faktorral
	a kirajzolt adatok között merem a minimum es maximum erteket
	offset automatikusan modosul

	amplitudo es offset interpolacio min-max bol kiderul, hogy kb ugyanakkor lagyen mindig az amplitudo

	512*128
*/

Cardioid::Cardioid()
: mFbo()
, mData()
, mDataMaxLength( FBO_WIDTH )
, mFactor( 1.0 )
, mOffset( 0 )
, mMinDataHeight( FBO_HEIGHT / 2 )
, mSmoothData( 0.3f )
{
}

void Cardioid::addData( int data )
{
	mData.push_back( data );

	while( mData.size() > (unsigned int)mDataMaxLength )
	{
		mData.erase( mData.begin() );
	}

	smoothData();
	updateFbo();
}

void Cardioid::clear()
{
	mData.clear();

	smoothData();
	updateFbo();
}

void Cardioid::smoothData()
{
	if( mData.size() == 0 )
		return;

	auto minmax = std::minmax_element( mData.begin(), mData.end() );

	int minData = *minmax.first;
	int maxData = *minmax.second;

	int height = maxData - minData;

	float factorNew;
	float offsetNew;

	if( height == 0 )
		factorNew = 1.0f;
	else if( height > FBO_HEIGHT )
		factorNew = (float)FBO_HEIGHT / height;
	else if( height < mMinDataHeight )
		factorNew = (float)mMinDataHeight / height;
	else
		factorNew = 1.0f;

	offsetNew = -minData - (int)( height / 2 );

	if( mData.size() == 1 )
	{
		mOffset = offsetNew;
		mFactor = factorNew;
	}
	else
	{
		mOffset = ci::lerp<int>  ( mOffset, offsetNew, mSmoothData );
		mFactor = ci::lerp<float>( mFactor, factorNew, mSmoothData );
	}
}

void Cardioid::setMinDataHeight( int minDataHeight )
{
	mMinDataHeight = minDataHeight;
}

int Cardioid::getMinDataHeight() const
{
	return mMinDataHeight;
}

void Cardioid::setSmoothData( float smoothData )
{
	mSmoothData = smoothData;
}

float Cardioid::getSmoothData() const
{
	return mSmoothData;
}

ci::gl::Fbo *Cardioid::getFbo()
{
	return &mFbo;
}

void Cardioid::updateFbo()
{
	gl::pushMatrices();

	mFbo.bindFramebuffer();

	gl::setViewport( mFbo.getBounds());
	gl::setMatricesWindow( mFbo.getSize(), false );
	gl::clear( ColorA( 0.f, 0.f, 0.f, 0.f ) );
	gl::color( Color::white());

	unsigned int dataLength = mData.size();

	Vec2f posPrev = Vec2f( 0, (float)FBO_HEIGHT / 2 );
	Vec2f posAct;
	int pos = 0;
	if( dataLength < FBO_WIDTH )
	{
		pos = FBO_WIDTH - dataLength;
		posAct = Vec2f( (float)pos, (float)FBO_HEIGHT / 2 );
		gl::drawLine( posAct, posPrev );
	}
	posPrev = posAct;

	for( std::deque< int >::const_iterator it = mData.begin(); it != mData.end(); ++it )
	{
		int data = *it;

		posAct = Vec2f( (float)pos, ( data + mOffset ) * mFactor + FBO_HEIGHT / 2 );

		if( pos != 0 )
			gl::drawLine( posAct, posPrev );

		posPrev = posAct;
		++pos;
	}


	mFbo.unbindFramebuffer();

	gl::popMatrices();
}

void Cardioid::setup()
{
	gl::Fbo::Format format;
	format.enableDepthBuffer( false );

	mFbo = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format ); 

	clear();
}

} // namespace HeartRate

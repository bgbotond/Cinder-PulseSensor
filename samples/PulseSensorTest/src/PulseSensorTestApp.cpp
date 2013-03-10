#include "cinder/app/AppBasic.h"
#include "cinder/params/Params.h"

#include "PulseSensorManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace HeartRate;

class PulseSensorTestApp : public AppBasic
{
public:
	void prepareSettings( Settings *settings );
	void setup();

	void keyDown( KeyEvent event );

	void update();
	void draw();

private:
	void initParams();

	void beat( int data );

private:
	PulseSensorManager mPulseSensorManager;

	float mColor;

	params::InterfaceGl mParams;
	float mFps;
};

void PulseSensorTestApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 640, 480 );
}

void PulseSensorTestApp::setup()
{
	mColor = 0.0;
	initParams();

	gl::disableVerticalSync();

	mPulseSensorManager.setup();

	mPulseSensorManager.addCallback<PulseSensorTestApp>( 0, PulseSensor::MT_BeatPauseTime, &PulseSensorTestApp::beat, this );
}

void PulseSensorTestApp::initParams()
{
	// params
	mParams = params::InterfaceGl( "Debug", Vec2i( 170, 80 ) );
	mParams.addParam( "Fps", &mFps, "", false );
}

void PulseSensorTestApp::keyDown( KeyEvent event )
{
	switch( event.getCode() )
	{
	case KeyEvent::KEY_f:
		{
			if ( ! isFullScreen() )
			{
				setFullScreen( true );
				if ( mParams.isVisible() )
					showCursor();
				else
					hideCursor();
			}
			else
			{
				setFullScreen( false );
				showCursor();
			}
		}
		break;
	case KeyEvent::KEY_s:
		{
			mParams.show( !mParams.isVisible() );
			if ( isFullScreen() )
			{
				if ( mParams.isVisible() )
					showCursor();
				else
					hideCursor();
			}
		}
		break;
	case KeyEvent::KEY_ESCAPE:
		{
			quit();
		}
		break;
	}
}

void PulseSensorTestApp::update()
{
	mColor *= .98f;
	mFps = getAverageFps();
	mPulseSensorManager.update();
}

void PulseSensorTestApp::draw()
{
	gl::clear( Color::gray( mColor ) );
	gl::setMatricesWindow( getWindowSize() );

	params::InterfaceGl::draw();
}

void PulseSensorTestApp::beat( int data )
{
	mColor = 1.0f;
}

CINDER_APP_BASIC( PulseSensorTestApp, RendererGl( RendererGl::AA_NONE ) )


#include "cinder/app/AppBasic.h"

#include "mndlkit/params/PParams.h"
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
	void shutdown();

	void keyDown( KeyEvent event );

	void update();
	void draw();

private:
	void initParams();

	void beat( int data );

private:
	PulseSensorManager mPulseSensorManager;

	float mColor;

	mndl::kit::params::PInterfaceGl mParams;
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
	mndl::kit::params::PInterfaceGl::load( "params.xml" );

	mParams = mndl::kit::params::PInterfaceGl( "Debug", Vec2i( 170, 80 ), Vec2i( 20, 20 ) );
	mParams.addPersistentSizeAndPosition();

	mParams.addParam( "Fps", &mFps, "", false );
}

void PulseSensorTestApp::shutdown()
{
	mndl::kit::params::PInterfaceGl::save();
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
			mndl::kit::params::PInterfaceGl::showAllParams( !mParams.isVisible() );
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
	gl::setMatricesWindow( getWindowSize());

	mndl::kit::params::PInterfaceGl::draw();
}

void PulseSensorTestApp::beat( int data )
{
	mColor = 1.0f;
}

CINDER_APP_BASIC( PulseSensorTestApp, RendererGl( RendererGl::AA_NONE ) )


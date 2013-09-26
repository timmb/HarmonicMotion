#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class HarmonicMotionApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void HarmonicMotionApp::setup()
{
}

void HarmonicMotionApp::mouseDown( MouseEvent event )
{
}

void HarmonicMotionApp::update()
{
}

void HarmonicMotionApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( HarmonicMotionApp, RendererGl )

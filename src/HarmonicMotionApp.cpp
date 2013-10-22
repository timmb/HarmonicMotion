#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "HarmonicMotion.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace hm;

class HarmonicMotionApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	
private:
	Pipeline mPipeline;
};

void HarmonicMotionApp::setup()
{
	std::cout << "Hello world" << std::endl;
	auto gen = NodePtr(new NodeKinect);
	auto printer = NodePtr(new NodeAccumulator);
	gen->outlet(0)->connect(printer->inlet(0));
	mPipeline.addNode(gen);
	mPipeline.addNode(printer);
	mPipeline.start();
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

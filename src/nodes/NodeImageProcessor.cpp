#include "NodeImageProcessor.h"
#include "Inlet.h"
#include "Outlet.h"
#include "opencv2/imgproc/imgproc.hpp"

using namespace hm;
using namespace std;


NodeImageProcessor::NodeImageProcessor(Node::Params const& params, string const& className)
: NodeUnthreaded(params, className, "Image processor", "Process 2D images (e.g. blur, threshold, etc.)")
, mProcess(0)
, mBlurAmount(0.f)
, mThreshold(128)
, mTimestamp(-42)
{
	vector<string> processNames;
	processNames.push_back("None");
	processNames.push_back("Blur");
	processNames.push_back("Threshold");
	processNames.push_back("Invert");
	
	mProcessParameter = addParameter("Process", &mProcess);
	mProcessParameter->setEnumerationLabels(processNames);
	mProcessParameter->addNewExternalValueCallback([this]() {
		processChangedCallback();
	});
	
	mParametersByProcess[NONE];
	
	ParameterPtr blurAmountParameter = addParameter("Blur amount", &mBlurAmount);
	blurAmountParameter->setBounds(0, 99999, 0, 99);
	mParametersByProcess[BLUR].push_back(blurAmountParameter);
	
	ParameterPtr thresholdParameter = addParameter("Threshold", &mThreshold);
	thresholdParameter->setBounds(-999999, 999999, 0, 255);
	mParametersByProcess[THRESHOLD].push_back(thresholdParameter);
	
	mInlet = createInlet(IMAGE2D, "Image to be processed", "2D images received at this input will be transformed using the selected process and outputted.");
	mOutlet = createOutlet(IMAGE2D, "Processed image", "Processed images will be outputted here.");
	
	processChangedCallback();
}

NodePtr NodeImageProcessor::create(Node::Params params) const
{
	return NodePtr(new NodeImageProcessor(params));
}

void NodeImageProcessor::step()
{
	Data data = mInlet->dataIfNewerThan(mTimestamp);
	if (!data.isNull())
	{
		mTimestamp = data.timestamp();
		if (data.isImage2d())
		{
			cv::Mat input = data.asImage2d().value;
			cv::Mat output;
			switch (mProcess)
			{
				case BLUR:
					output = cv::Mat(input.size(), input.type());
					cv::GaussianBlur(input, output, cv::Size(mBlurAmount*2+1, mBlurAmount*2+1), 0);
					break;
				case THRESHOLD:
					output = input;
					cv::threshold(input, output, mThreshold, 255, cv::THRESH_BINARY);
					break;
				case INVERT:
					output = input;
					cv::subtract(cv::Scalar::all(255), input, output);
					break;
				default:
					assert(false);
					// fallthrough
				case NONE:
					output = input;
					break;
			}
			Data out(Image2d(output, mTimestamp, data.id(), data.sceneMeta()));
			mOutlet->outputNewData(out);
		}
	}
}

void NodeImageProcessor::processChangedCallback()
{
	for (auto const& pair: mParametersByProcess)
	{
		for (ParameterPtr p: pair.second)
		{
			p->setVisible(mProcess == pair.first);
		}
	}
}





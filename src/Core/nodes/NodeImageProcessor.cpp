#include "nodes/NodeImageProcessor.h"
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
, mWidth(320)
, mHeight(240)
, mLockAspectRatio(false)
, mPrevAspectRatio(mWidth / (float)mHeight)
, mInterpolationMethod(0)
, mHeightWasAdjusted(false)
, mWidthWasAdjusted(false)
//, mDenoiseAmount(3)
//, mTemplateWindowDiameter(3)
//, mSearchWindowDiameter(10)
, mEnableClampMin(true)
, mClampMin(0)
, mEnableClampMax(true)
, mClampMax(65535)
, mMedianBlurAmount(1)
{
	vector<string> processNames;
	processNames.push_back("None");
	processNames.push_back("Blur");
	processNames.push_back("Median blur");
	processNames.push_back("Threshold");
	processNames.push_back("Invert");
	processNames.push_back("Resize");
	//processNames.push_back("Denoise");
	processNames.push_back("Clamp");

	mProcessParameter = addParameter("Process", &mProcess);
	mProcessParameter->setEnumerationLabels(processNames);
	mProcessParameter->addNewExternalValueCallback([this]() {
		processChangedCallback();
	});

	mParametersByProcess[NONE];

	ParameterPtr blurAmountParameter = addParameter("Blur amount", &mBlurAmount);
	blurAmountParameter->setBounds(0, 99999, 0, 99);
	mParametersByProcess[BLUR].push_back(blurAmountParameter);

	ParameterPtr medianBlurAmountParameter = addParameter("Median blur amount", &mMedianBlurAmount);
	medianBlurAmountParameter->setBounds(0, 2, 0, 2);
	mParametersByProcess[MEDIAN_BLUR].push_back(medianBlurAmountParameter);

	ParameterPtr thresholdParameter = addParameter("Threshold", &mThreshold);
	thresholdParameter->setBounds(-999999, 999999, 0, 255);
	mParametersByProcess[THRESHOLD].push_back(thresholdParameter);

	ParameterPtr widthParameter = addParameter("Width", &mWidth);
	widthParameter->setBounds(1, 100000, 1, 2000);
	widthParameter->addNewExternalValueCallback([this]() {
		mWidthWasAdjusted = !mHeightWasAdjusted;
	});
	// avoid recurrent updates
	widthParameter->addNewInternalValueCallback([this]() {
		//mWidthWasAdjusted = false;
	});
	mParametersByProcess[RESIZE].push_back(widthParameter);

	ParameterPtr heightParameter = addParameter("Height", &mHeight);
	heightParameter->setBounds(1, 100000, 1, 2000);
	heightParameter->addNewExternalValueCallback([this]() {
		mHeightWasAdjusted = !mWidthWasAdjusted;
	});
	heightParameter->addNewInternalValueCallback([this]() {
		//mHeightWasAdjusted = false;
	});
	mParametersByProcess[RESIZE].push_back(heightParameter);

	//ParameterPtr lockAspectParameter = addParameter("Lock aspect ratio", &mLockAspectRatio, "Adjust the width and height parameters when you edit them to keep the aspect ratio constant.");
	//mParametersByProcess[RESIZE].push_back(lockAspectParameter);

	vector<string> interpLabels =
	{
		"Nearest neighbour",
		"Linear",
		"Bicubic",
		"Area based",
		"Lanczos"
	};
	static_assert(cv::INTER_NEAREST == 0, "CV interpolation labels are wrong");
	static_assert(cv::INTER_LINEAR == 1, "CV interpolation labels are wrong");
	static_assert(cv::INTER_CUBIC == 2, "CV interpolation labels are wrong");
	static_assert(cv::INTER_AREA == 3, "CV interpolation labels are wrong");
	static_assert(cv::INTER_LANCZOS4 == 4, "CV interpolation labels are wrong");
	ParameterPtr interpParam = addParameter("Interpolation", &mInterpolationMethod, "The algorithm used to resize.");
	interpParam->setEnumerationLabels(interpLabels);
	mParametersByProcess[RESIZE].push_back(interpParam);

	//ParameterPtr denoiseAmountParameter = addParameter("Amount", &mDenoiseAmount, "How aggressively to remove noise.");
	//denoiseAmountParameter->setBounds(0, 999999, 0, 20);
	//mParametersByProcess[DENOISE].push_back(denoiseAmountParameter);

	//ParameterPtr templateWindowParameter = addParameter("Template window diameter", &mTemplateWindowDiameter, "How many pixels in each direction to search for a template patch to calculate weights. Recommended value: 3.");
	//templateWindowParameter->setBounds(0, 999999, 0, 10);
	//mParametersByProcess[DENOISE].push_back(templateWindowParameter);

	//ParameterPtr searchWindowParameter = addParameter("Search window diameter", &mSearchWindowDiameter, "How many pixels in each direction to consider when calculating a new value for each pixels. Larger values will linearly increase the calculation time. Recommended value: 10.");
	//searchWindowParameter->setBounds(0, 999999, 0, 50);
	//mParametersByProcess[DENOISE].push_back(searchWindowParameter);

	ParameterPtrT<bool> enableMinParam = addParameter("Min clamp", &mEnableClampMin, "Enables clamping lower values based on the min value.");
	mParametersByProcess[CLAMP].push_back(enableMinParam);

	mClampMinParameter = addParameter("Min value", &mClampMin, "Pixels with values less than this will be clamped to this value.");
	mClampMinParameter->setBounds(-std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), 0, 65535);
	mParametersByProcess[CLAMP].push_back(mClampMinParameter);
	enableMinParam->addNewExternalValueCallback([&]() {
		mClampMinParameter->setVisible(mEnableClampMin);
	});

	ParameterPtrT<bool> enableMaxParam = addParameter("Max clamp", &mEnableClampMax, "Enables clamping upper values based on the max value.");
	mParametersByProcess[CLAMP].push_back(enableMaxParam);

	mClampMaxParameter = addParameter("Max value", &mClampMax, "Pixels with values greater than this will be clamped to this value.");
	mClampMaxParameter->setBounds(-std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), 0, 65535);
	mParametersByProcess[CLAMP].push_back(mClampMaxParameter);
	enableMaxParam->addNewExternalValueCallback([&]() {
		mClampMaxParameter->setVisible(mEnableClampMax);
	});

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
	if (mWidthWasAdjusted)
	{
		if (mLockAspectRatio)
		{
			mHeight = mPrevAspectRatio * mWidth;
		}
		else
			mPrevAspectRatio = mWidth / (float)mHeight;
	}
	else if (mHeightWasAdjusted)
	{
		if (mLockAspectRatio)
			mWidth = mHeight / mPrevAspectRatio;
		else
			mPrevAspectRatio = mWidth / (float)mHeight;
	}
	mWidthWasAdjusted = false;
	mHeightWasAdjusted = false;

	if (mProcess < 0 || mProcess >= NUM_PROCESSES)
	{
		mProcess = NONE;
	}

	Data data = mInlet->dataIfNewerThan(mTimestamp);
	if (!data.isNull())
	{
		mTimestamp = data.timestamp();
		if (data.isImage2d())
		{
			cv::Mat input = data.asImage2d().value;
			if (input.rows > 0 && input.cols > 0)
			{
				cv::Mat output;
				SceneMetaPtr sceneMeta = data.sceneMeta();
				switch (mProcess)
				{
				case BLUR:
					output = cv::Mat(input.size(), input.type());
					cv::GaussianBlur(input, output, cv::Size(mBlurAmount * 2 + 1, mBlurAmount * 2 + 1), 0);
					break;
				case MEDIAN_BLUR:
					if (mMedianBlurAmount > 0)
					{
						output = cv::Mat(input.size(), input.type());
						cv::medianBlur(input, output, 1 + 2 * mMedianBlurAmount);
					}
					else
					{
						input.copyTo(output);
					}
					break;
				case THRESHOLD:
					output = input;
					cv::threshold(input, output, mThreshold, 255, cv::THRESH_BINARY);
					break;
				case INVERT:
					output = input;
					cv::subtract(cv::Scalar::all(255), input, output);
					break;
				case RESIZE:
					//output = cv::Mat(cv::Size(mWidth, mHeight), input.type());
					mWidth = max(1, mWidth);
					mHeight = max(1, mHeight);
					cv::resize(input, output, cv::Size(mWidth, mHeight), 0.0, 0.0, mInterpolationMethod);
					{
						SceneMeta* s = new SceneMeta(*sceneMeta);
						s->viewportSize = ci::Vec2i(mWidth, mHeight);
						sceneMeta = SceneMetaPtr(s);
					}
					break;
				//case DENOISE:
				//{
				//				cv::Mat tmp = input;
				//				input = cv::Mat();
				//				tmp.convertTo(input, CV_8UC1);
				//				try{
				//					int inputDepth = input.depth();
				//					assert(inputDepth == CV_8U);
				//					assert(input.rows > 0 && input.rows > 0);
				//					cv::fastNlMeansDenoising(input, output, mDenoiseAmount, mTemplateWindowDiameter * 2 + 1, mSearchWindowDiameter * 2 + 1);
				//				}
				//				catch (cv::Exception const& e)
				//				{
				//					std::cout << e.what() << endl;
				//					throw e;
				//				}
				//}
				//	break;
				case CLAMP:
					output = input;
					if (mEnableClampMax)
					{
						cv::min(output, mClampMax, output);
					}
					if (mEnableClampMin)
					{
						cv::max(output, mClampMin, output);
					}
					break;
				default:
					assert(false);
					// fallthrough
				case NONE:
					output = input;
					break;
				}
				Data out(Image2d(output, mTimestamp, data.id(), sceneMeta, data.asImage2d().role));
				mOutlet->outputNewData(out);
			}
		}
	}
}

void NodeImageProcessor::processChangedCallback()
{
	mProcess = max<int>(NONE, min<int>(NUM_PROCESSES, mProcess));
	mClampMaxParameter->setVisible(mEnableClampMax);
	mClampMinParameter->setVisible(mEnableClampMin);
	for (auto const& pair : mParametersByProcess)
	{
		for (ParameterPtr p : pair.second)
		{
			p->setVisible(mProcess == pair.first);
		}
	}
}





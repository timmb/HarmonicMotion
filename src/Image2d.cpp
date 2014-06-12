#include "Image2d.h"
#include "CinderOpenCV.h"
#include "Value.h"

using namespace hm;
using namespace std;

Image2d::Image2d(double timestamp, SceneMetaPtr sceneMeta)
: BaseData(timestamp, sceneMeta)
, value(480, 640, CV_8UC3)
{
	
}

Image2d::Image2d(int width, int height, double timestamp, SceneMetaPtr sceneMeta)
: BaseData(timestamp, sceneMeta)
, value(height, width, CV_8UC3)
{
	
}

Image2d::Image2d(ci::Surface8u surface, double timestamp, SceneMetaPtr sceneMeta)
: BaseData(timestamp, sceneMeta)
{
	if (surface)
	{
		value = ci::toOcv(surface);
	}
	checkValueHasDimensions();
}

Image2d::Image2d(cv::Mat matrix, double timestamp, SceneMetaPtr sceneMeta)
: BaseData(timestamp, sceneMeta)
, value(matrix)
{
	checkValueHasDimensions();
}

void Image2d::checkValueHasDimensions()
{
	int width = value.cols == 0? 640 : value.cols;
	int height = value.rows == 0? 480 : value.rows;
	if (value.cols != width || value.rows != height)
	{
		value = cv::Mat(height, width, CV_8UC3);
	}
	assert(invariant());
}


bool Image2d::invariant() const
{
	return value.rows > 0 && value.cols > 0;
}


ci::Surface8u Image2d::toSurface() const
{
	cv::Mat m(value);
	return ci::Surface8u(ci::fromOcv(m));
}






ostream& Image2d::printTo(ostream& out) const
{
	return out << "Image2d(w:"<<value.cols<<", "<<"h:"<<value.rows<<')';
}

Image2d Image2d::operator*(Value const& rhs) const
{
	assert(invariant());
	return Image2d(value * rhs.value, max(timestamp, rhs.timestamp), choose(sceneMeta, rhs.sceneMeta));
}

Image2d Image2d::operator/(Value const& rhs) const
{
	assert(invariant());
	return Image2d(value / rhs.value, max(timestamp, rhs.timestamp), choose(sceneMeta, rhs.sceneMeta));
}

Image2d Image2d::operator+(Value const& rhs) const
{
	assert(invariant());
	return Image2d(value + rhs.value, max(timestamp, rhs.timestamp), choose(sceneMeta, rhs.sceneMeta));
}

Image2d Image2d::operator-(Value const& rhs) const
{
	assert(invariant());
	return Image2d(value - rhs.value, max(timestamp, rhs.timestamp), choose(sceneMeta, rhs.sceneMeta));
}

Image2d& Image2d::operator*=(Value const& rhs)
{
	assert(invariant());
	value *= rhs.value;
	timestamp = max(timestamp, rhs.timestamp);
	return *this;
}

Image2d& Image2d::operator/=(Value const& rhs)
{
	assert(invariant());
	value /= rhs.value;
	timestamp = max(timestamp, rhs.timestamp);
	return *this;
}

Image2d& Image2d::operator+=(Value const& rhs)
{
	assert(invariant());
	value += rhs.value;
	timestamp = max(timestamp, rhs.timestamp);
	return *this;
}

Image2d& Image2d::operator-=(Value const& rhs)
{
	assert(invariant());
	value -= rhs.value;
	timestamp = max(timestamp, rhs.timestamp);
	return *this;
}


Image2d Image2d::operator*(Image2d const& rhs) const
{
	assert(invariant() && rhs.invariant());
	cv::Range outRows(0, min(value.rows, rhs.value.rows));
	cv::Range outCols(0, min(value.cols, rhs.value.cols));
	return Image2d(value(outRows, outCols)*rhs.value(outRows, outCols), max(timestamp, rhs.timestamp), choose(sceneMeta, rhs.sceneMeta));
}

Image2d Image2d::operator/(Image2d const& rhs) const
{
	assert(invariant() && rhs.invariant());
	cv::Range outRows(0, min(value.rows, rhs.value.rows));
	cv::Range outCols(0, min(value.cols, rhs.value.cols));
	return Image2d(value(outRows, outCols)/rhs.value(outRows, outCols), max(timestamp, rhs.timestamp), choose(sceneMeta, rhs.sceneMeta));
}

Image2d Image2d::operator+(Image2d const& rhs) const
{
	assert(invariant() && rhs.invariant());
	cv::Range outRows(0, min(value.rows, rhs.value.rows));
	cv::Range outCols(0, min(value.cols, rhs.value.cols));
	return Image2d(value(outRows, outCols)+rhs.value(outRows, outCols), max(timestamp, rhs.timestamp), choose(sceneMeta, rhs.sceneMeta));
}

Image2d Image2d::operator-(Image2d const& rhs) const
{
	assert(invariant() && rhs.invariant());
	cv::Range outRows(0, min(value.rows, rhs.value.rows));
	cv::Range outCols(0, min(value.cols, rhs.value.cols));
	return Image2d(value(outRows, outCols)-rhs.value(outRows, outCols), max(timestamp, rhs.timestamp), choose(sceneMeta, rhs.sceneMeta));
}


Image2d& Image2d::operator*=(Image2d const& rhs)
{
	assert(invariant() && rhs.invariant());
	cv::Range outRows(0, min(value.rows, rhs.value.rows));
	cv::Range outCols(0, min(value.cols, rhs.value.cols));
	value = value(outRows, outCols)*rhs.value(outRows, outCols);
	timestamp =max(timestamp, rhs.timestamp);
	return *this;
}

Image2d& Image2d::operator/=(Image2d const& rhs)
{
	assert(invariant() && rhs.invariant());
	cv::Range outRows(0, min(value.rows, rhs.value.rows));
	cv::Range outCols(0, min(value.cols, rhs.value.cols));
	value = value(outRows, outCols)/rhs.value(outRows, outCols);
	timestamp =max(timestamp, rhs.timestamp);
	return *this;
}

Image2d& Image2d::operator+=(Image2d const& rhs)
{
	assert(invariant() && rhs.invariant());
	cv::Range outRows(0, min(value.rows, rhs.value.rows));
	cv::Range outCols(0, min(value.cols, rhs.value.cols));
	value = value(outRows, outCols)+rhs.value(outRows, outCols);
	timestamp =max(timestamp, rhs.timestamp);
	return *this;
}

Image2d& Image2d::operator-=(Image2d const& rhs)
{
	assert(invariant() && rhs.invariant());
	cv::Range outRows(0, min(value.rows, rhs.value.rows));
	cv::Range outCols(0, min(value.cols, rhs.value.cols));
	value = value(outRows, outCols)-rhs.value(outRows, outCols);
	timestamp =max(timestamp, rhs.timestamp);
	return *this;
}

Image2d Image2d::operator+() const
{
	assert(invariant());
	return Image2d(value.clone(), timestamp, sceneMeta);
}

Image2d Image2d::operator-() const
{
	assert(invariant());
	return Image2d(-value, timestamp, sceneMeta);
}








//
//  WidgetParameter.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 03/02/2014.
//
//

#include "WidgetParameter.h"
#include <QFormLayout>
#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include "Utilities.h"

using namespace hm;


// The Qt5 function pointer syntax for connect() can get ugly with overloaded
// types. This provides a neater method of selection
template<typename... Args> struct SELECT {
    template<typename C, typename R>
    static constexpr auto OVERLOAD_OF( R (C::*pmf)(Args...) ) -> decltype(pmf) {
        return pmf;
    }
};
// see http://stackoverflow.com/a/16795664/794283
// usage: SELECT<int>::OVERLOAD_OF(&QSpinBox::valueChanged)


// ----------------------------
WidgetBaseParameter::WidgetBaseParameter()
{
	setContentsMargins(0, 0, 0, 0);
	setObjectName("WidgetParameter");
}

void WidgetBaseParameter::paintEvent(QPaintEvent *)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

WidgetBaseParameter* WidgetBaseParameter::create(ParameterPtr parameter)
{
	assert(parameter != nullptr);
	switch (parameter->type())
	{
		case BaseParameter::DOUBLE:
		{
			auto p = std::dynamic_pointer_cast<Parameter<double>>(parameter);
			assert(p);
			return createDelegate(p);
		}
		case BaseParameter::INT:
		{
			auto p = std::dynamic_pointer_cast<Parameter<int>>(parameter);
			assert(p);
			return createDelegate(p);
		}
		case BaseParameter::STRING:
		{
			auto p = std::dynamic_pointer_cast<Parameter<std::string>>(parameter);
			assert(p);
			return createDelegate(p);
		}
		case BaseParameter::NUM_TYPES:
		default:
			assert(false && "Unrecognised parameter type when creating widget.");
			return new WidgetBaseParameter();
	}
}

WidgetBaseParameter* WidgetBaseParameter::createDelegate(std::shared_ptr<Parameter<int>> parameter)
{
	return new WidgetParameterInt(parameter);
}

WidgetBaseParameter* WidgetBaseParameter::createDelegate(std::shared_ptr<Parameter<std::string>> parameter)
{
	return new WidgetParameterString(parameter);
}

WidgetBaseParameter* WidgetBaseParameter::createDelegate(std::shared_ptr<Parameter<double>> parameter)
{
	return new WidgetParameterDouble(parameter);
}



// ----------------------------

WidgetParameterDouble::WidgetParameterDouble(std::shared_ptr<Parameter<double>> parameter)
: WidgetParameter<double>(parameter)
, mSpinBox(new QDoubleSpinBox(this))
{
	mSpinBox->setMaximumWidth(100);
	mSpinBox->setRange(parameter->hardMin, parameter->hardMax);
	mSpinBox->move(0,0);
	
	// param -> widget
	mParameter->addNewInternalValueCallback([this](double value)
											{
												mSpinBox->blockSignals(true);
												Q_EMIT newInternalValue(value);
												mSpinBox->blockSignals(false);
											});
	bool success = connect(this, SIGNAL(newInternalValue(double)), mSpinBox, SLOT(setValue(double)));
	assert(success);
	
	// widget -> param
	success = connect(mSpinBox, SELECT<double>::OVERLOAD_OF(&QDoubleSpinBox::valueChanged), [this](double value)
					  {
						  mParameter->set(value);
					  });
	assert(success);
	
//	auto layout = new QHBoxLayout;
//	layout->setAlignment(Qt::AlignLeft);
//	setLayout(layout);
//	
//	layout->addWidget(mSpinBox);
}


// ----------------------------

WidgetParameterInt::WidgetParameterInt(std::shared_ptr<Parameter<int>> parameter)
: WidgetParameter<int>(parameter)
, mSpinBox(new QSpinBox(this))
{
	assert(parameter != nullptr);
	ParameterValueContainer valueContainer = parameter->toContainer();
	int* value = nullptr;
	value = boost::get<int>(&valueContainer);
	assert(value);
	if (value)
	{
		mSpinBox->setValue(*value);
	}
	mSpinBox->setRange(parameter->softMin, parameter->softMax);
	mSpinBox->move(0, 0);
	
	// param -> widget
	mParameter->addNewInternalValueCallback([this](int value)
	{
		mSpinBox->blockSignals(true);
		Q_EMIT newInternalValue(value);
		mSpinBox->blockSignals(false);
	});
	bool success = connect(this, SIGNAL(newInternalValue(int)), mSpinBox, SLOT(setValue(int)));
	assert(success);

	// widget -> param
	success = connect(mSpinBox, SELECT<int>::OVERLOAD_OF(&QSpinBox::valueChanged), [this](int value)
	{
		mParameter->set(value);
	});
	assert(success);
		
//	auto t = new QTimer(this);
//	t->setInterval(1000);
//	t->start();
//	connect(t, &QTimer::timeout, [this]() {
//		qDebug() << "pos" << mSpinBox->pos() << "geom"<<mSpinBox->geometry();
//		
//	});
}


// ----------------------------

WidgetParameterString::WidgetParameterString(std::shared_ptr<Parameter<std::string>> parameter)
: WidgetParameter<std::string>(parameter)
, mWidget(new QLineEdit(this))
{
	mWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	mWidget->move(0,0);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	
	// param -> widget
	mParameter->addNewInternalValueCallback([this](std::string value)
	{
		mWidget->blockSignals(true);
		Q_EMIT newInternalValue(str(value));
		mWidget->blockSignals(false);
	});
	bool success = connect(this, SIGNAL(newInternalValue(QString const&)), mWidget, SLOT(setText(QString const&)));
	assert(success);
	
	success = connect(mWidget, &QLineEdit::textChanged, [this](QString const& value)
	{
		mParameter->set(std::string(value.toUtf8()));
	});
	assert(success);
		
}






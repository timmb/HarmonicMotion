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
#include <QComboBox>

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
	mSpinBox->setRange(parameter->hardMin(), parameter->hardMax());
	mSpinBox->move(0,0);
	
	ParameterValueContainer valueContainer = parameter->toContainer();
	double* value = nullptr;
	value = boost::get<double>(&valueContainer);
	assert(value);
	if (value)
	{
		mSpinBox->setValue(*value);
	}
	
	// param -> widget
	mParameter->addNewInternalValueCallback([this](double value) {
		mSpinBox->blockSignals(true);
		Q_EMIT newInternalValue(value);
		mSpinBox->blockSignals(false);
	});
	bool success = connect(this, SIGNAL(newInternalValue(double)), mSpinBox, SLOT(setValue(double)));
	assert(success);
	
	// widget -> param
	success = connect(mSpinBox, SELECT<double>::OVERLOAD_OF(&QDoubleSpinBox::valueChanged), [this](double value) {
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
, mWidget(nullptr)
{
	assert(parameter != nullptr);
	
	if (parameter->hasEnumerationLabels())
	{
		QComboBox* w = new QComboBox(this);
		w->setEditable(false);
		std::vector<std::string> labels = parameter->enumerationLabels();
		for (int i=0; i<labels.size(); i++)
		{
			w->insertItem(i, str(labels[i]));
		}
		// param -> widget
		mInternalValueCallbackHandle = mParameter->addNewInternalValueCallback([w, this](int value) {
			w->blockSignals(true);
			Q_EMIT newInternalValue(value);
			w->blockSignals(false);
		});
		bool success = connect(this, SIGNAL(newInternalValue(int)), w, SLOT(setCurrentIndex(int)));
		assert(success);
		// widget -> param
		success = connect(w, SELECT<int>::OVERLOAD_OF(&QComboBox::currentIndexChanged), [this](int value) {
			mParameter->set(value);
		});
		assert(success);
		mWidget = w;
	}
	else
	{
		QSpinBox* spinBox = new QSpinBox(this);
		spinBox->setRange(parameter->softMin(), parameter->softMax());
		ParameterValueContainer valueContainer = parameter->toContainer();
		int* value = nullptr;
		value = boost::get<int>(&valueContainer);
		assert(value);
		if (value)
		{
			spinBox->setValue(*value);
		}
		spinBox->move(0, 0);
		
		// param -> widget
		mInternalValueCallbackHandle = mParameter->addNewInternalValueCallback([spinBox, this](int value)
		{
			spinBox->blockSignals(true);
			Q_EMIT newInternalValue(value);
			spinBox->blockSignals(false);
		});
		bool success = connect(this, SIGNAL(newInternalValue(int)), spinBox, SLOT(setValue(int)));
		assert(success);
		
		// widget -> param
		success = connect(spinBox, SELECT<int>::OVERLOAD_OF(&QSpinBox::valueChanged), [this](int value) {
			mParameter->set(value);
		});
		assert(success);
		mWidget = spinBox;
	}
}

WidgetParameterInt::~WidgetParameterInt()
{
	mParameter->removeNewInternalValueCallback(mInternalValueCallbackHandle);
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






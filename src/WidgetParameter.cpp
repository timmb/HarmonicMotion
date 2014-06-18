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
#include <QCheckBox>

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
WidgetBaseParameter::WidgetBaseParameter(ParameterPtr parameter)
: mParameter(parameter)
, mCallbackHandle(-42)
, mWasVisible(parameter->isVisible())
{
	setContentsMargins(0, 0, 0, 0);
	setObjectName("WidgetParameter");
	mCallbackHandle = mParameter->addChangeOfCharacteristicsCallback(std::bind(&WidgetBaseParameter::characteristicsCallback_, this));
}

WidgetBaseParameter::~WidgetBaseParameter()
{
	BOOST_VERIFY(mParameter->removeChangeOfCharacteristicsCallback(mCallbackHandle));
}

void WidgetBaseParameter::characteristicsCallback_()
{
	bool vis = mParameter->isVisible();
	if (mWasVisible != vis)
	{
		mWasVisible = vis;
		Q_EMIT parameterVisibilityChanged(this, vis);
	}
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
		case BaseParameter::BOOL:
		{
			auto p = std::dynamic_pointer_cast<Parameter<bool>>(parameter);
			assert(p);
			return new WidgetParameterBool(p);
		}
		case BaseParameter::FLOAT:
		{
			auto p = std::dynamic_pointer_cast<Parameter<float>>(parameter);
			assert(p);
			return new WidgetParameterFloat(p);
		}
		case BaseParameter::DOUBLE:
		{
			auto p = std::dynamic_pointer_cast<Parameter<double>>(parameter);
			assert(p);
			return new WidgetParameterDouble(p);
		}
		case BaseParameter::INT:
		{
			auto p = std::dynamic_pointer_cast<Parameter<int>>(parameter);
			assert(p);
			return new WidgetParameterInt(p);
		}
		case BaseParameter::STRING:
		{
			auto p = std::dynamic_pointer_cast<Parameter<std::string>>(parameter);
			assert(p);
			return new WidgetParameterString(p);
		}
		case BaseParameter::NUM_TYPES:
		default:
			assert(false && "Unrecognised parameter type when creating widget.");
			return new WidgetBaseParameter(parameter);
	}
}

// ----------------------------

WidgetParameterBool::WidgetParameterBool(ParameterPtrT<bool> parameter)
: WidgetParameter<bool>(parameter)
{
	QCheckBox* checkBox = new QCheckBox(this);
	checkBox->setChecked(parameter->lastValue());
	checkBox->move(0,0);
	
	parameter->addNewInternalValueCallback([checkBox, this](bool value){
		checkBox->blockSignals(true);
		checkBox->setChecked(value);
		checkBox->blockSignals(false);
	});
	BOOST_VERIFY(connect(checkBox, SELECT<bool>::OVERLOAD_OF(&QCheckBox::clicked), [this](bool b) {
		mParameter->set(b);
	}));
	
	mWidget = checkBox;
}


// ----------------------------

WidgetParameterFloat::WidgetParameterFloat(std::shared_ptr<Parameter<float>> parameter)
: WidgetParameter<float>(parameter)
{
	QDoubleSpinBox* spinBox = new QDoubleSpinBox(this);
	spinBox->setMaximumWidth(100);
	spinBox->setRange(parameter->hardMin(), parameter->hardMax());
	spinBox->move(0,0);
	
	ParameterValueContainer valueContainer = parameter->toContainer();
	float* value = nullptr;
	value = boost::get<float>(&valueContainer);
	assert(value);
	if (value)
	{
		spinBox->setValue(*value);
	}
	
	// param -> widget
	mParameter->addNewInternalValueCallback([spinBox, this](float value) {
		spinBox->blockSignals(true);
		Q_EMIT newInternalValue(value);
		spinBox->blockSignals(false);
	});
	bool success = connect(this, SIGNAL(newInternalValue(double)), spinBox, SLOT(setValue(double)));
	assert(success);
	
	// widget -> param
	success = connect(spinBox, SELECT<double>::OVERLOAD_OF(&QDoubleSpinBox::valueChanged), [this](double value) {
		mParameter->set(float(value));
	});
	assert(success);
	
	mWidget = spinBox;
}



// ----------------------------

WidgetParameterDouble::WidgetParameterDouble(std::shared_ptr<Parameter<double>> parameter)
: WidgetParameter<double>(parameter)
{
	QDoubleSpinBox* spinBox = new QDoubleSpinBox(this);
	spinBox->setMaximumWidth(100);
	spinBox->setRange(parameter->hardMin(), parameter->hardMax());
	spinBox->move(0,0);
	
	ParameterValueContainer valueContainer = parameter->toContainer();
	double* value = nullptr;
	value = boost::get<double>(&valueContainer);
	assert(value);
	if (value)
	{
		spinBox->setValue(*value);
	}
	
	// param -> widget
	mParameter->addNewInternalValueCallback([this, spinBox](double value) {
		spinBox->blockSignals(true);
		Q_EMIT newInternalValue(value);
		spinBox->blockSignals(false);
	});
	bool success = connect(this, SIGNAL(newInternalValue(double)), spinBox, SLOT(setValue(double)));
	assert(success);
	
	// widget -> param
	success = connect(spinBox, SELECT<double>::OVERLOAD_OF(&QDoubleSpinBox::valueChanged), [this](double value) {
		mParameter->set(value);
	});
	assert(success);
	
	mWidget = spinBox;
}


// ----------------------------

WidgetParameterInt::WidgetParameterInt(std::shared_ptr<Parameter<int>> parameter)
: WidgetParameter<int>(parameter)
{
	assert(parameter != nullptr);
	int* value = nullptr;
	ParameterValueContainer valueContainer = parameter->toContainer();
	value = boost::get<int>(&valueContainer);
	assert(value);
	
	if (parameter->hasEnumerationLabels())
	{
		QComboBox* w = new QComboBox(this);
		w->setEditable(false);
		std::vector<std::string> labels = parameter->enumerationLabels();
		for (int i=0; i<labels.size(); i++)
		{
			w->insertItem(i, str(labels[i]));
		}
		if (value)
		{
			w->setCurrentIndex(*value);
		}
		// param -> widget
		// note it's not necessary to remove this callback on destruction
		// as the shared_ptr to the parameter will ensure its lifetime
		// lasts that of this widget.
		mParameter->addNewInternalValueCallback([w, this](int value) {
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
		if (value)
		{
			spinBox->setValue(*value);
		}
		spinBox->move(0, 0);
		
		// param -> widget
		mParameter->addNewInternalValueCallback([spinBox, this](int value)
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



// ----------------------------

WidgetParameterString::WidgetParameterString(std::shared_ptr<Parameter<std::string>> parameter)
: WidgetParameter<std::string>(parameter)
{
	QLineEdit* widget = new QLineEdit(this);
	widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	widget->move(0,0);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	
	// param -> widget
	mParameter->addNewInternalValueCallback([this, widget](std::string value)
											{
												widget->blockSignals(true);
												Q_EMIT newInternalValue(str(value));
												widget->blockSignals(false);
											});
	bool success = connect(this, SIGNAL(newInternalValue(QString const&)), widget, SLOT(setText(QString const&)));
	assert(success);
	
	success = connect(widget, &QLineEdit::textChanged, [this](QString const& value)
					  {
						  mParameter->set(std::string(value.toUtf8()));
					  });
	assert(success);
	
	mWidget = widget;
}






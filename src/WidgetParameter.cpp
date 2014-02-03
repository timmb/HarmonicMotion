//
//  WidgetParameter.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 03/02/2014.
//
//

#include "WidgetParameter.h"
#include <QFormLayout>

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
WidgetBaseParameter* WidgetBaseParameter::create(ParameterPtr parameter)
{
	switch (parameter->type())
	{
		case BaseParameter::INT:
			return create(std::dynamic_pointer_cast<Parameter<int>>(parameter));
		case BaseParameter::STRING:
			return create(std::dynamic_pointer_cast<Parameter<std::string>>(parameter));
	}
}

WidgetBaseParameter* WidgetBaseParameter::create(std::shared_ptr<Parameter<int>> parameter)
{
	return new WidgetParameterInt(parameter);
}

WidgetBaseParameter* WidgetBaseParameter::create(std::shared_ptr<Parameter<std::string>> parameter)
{
	return new WidgetParameterString(parameter);
}



// ----------------------------

WidgetParameterInt::WidgetParameterInt(std::shared_ptr<Parameter<int>> parameter)
: WidgetParameter<int>(parameter)
, mSpinBox(new QSpinBox)
{
	mSpinBox->setRange(parameter->hardMin, parameter->hardMax);
	
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
	
	auto layout = new QHBoxLayout;
	setLayout(layout);
	
	layout->addWidget(mSpinBox);
}


// ----------------------------

WidgetParameterString::WidgetParameterString(std::shared_ptr<Parameter<std::string>> parameter)
: WidgetParameter<std::string>(parameter)
, mWidget(new QLineEdit)
{
	// param -> widget
	mParameter->addNewInternalValueCallback([this](std::string value)
	{
		mWidget->blockSignals(true);
		Q_EMIT newInternalValue(QString::fromUtf8(value.c_str()));
		mWidget->blockSignals(false);
	});
	bool success = connect(this, SIGNAL(newInternalValue(QString const&)), mWidget, SLOT(setText(QString const&)));
	assert(success);
	
	success = connect(mWidget, &QLineEdit::textChanged, [this](QString const& value)
	{
		mParameter->set(std::string(value.toUtf8()));
	});
	assert(success);
	
	auto layout = new QHBoxLayout;
	setLayout(layout);
	
	layout->addWidget(mWidget);
	
}






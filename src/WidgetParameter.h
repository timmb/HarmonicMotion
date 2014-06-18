//
//  WidgetParameter.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 03/02/2014.
//
//

#pragma once

#include <QWidget>
#include <memory>
#include "Parameter.h"
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <string>
#include <QLineEdit>

namespace hm
{
	/// Common base class of all widget parameters
	class WidgetBaseParameter : public QWidget
	{
		Q_OBJECT
		
	public:
		virtual ~WidgetBaseParameter();
		
		/// Factory function to create a widget of the correct type for the
		/// parameter.
		static WidgetBaseParameter* create(ParameterPtr parameter);
		
		/// \return Whether the underlying ParameterPtr belives it should be
		/// visible in the interface at the moment
		bool isParameterVisible() const { return mParameter->isVisible(); }
		
	protected:
		WidgetBaseParameter(ParameterPtr parameter);
		
		/// Necessary to reimplement to make stylesheet work
		/// see http://stackoverflow.com/questions/7276330/qt-stylesheet-for-custom-widget
		void paintEvent(QPaintEvent*);
		
	Q_SIGNALS:
		void parameterVisibilityChanged(WidgetBaseParameter* owningWidget, bool isVisible);
		
	private:
		void characteristicsCallback_();
		ParameterPtr mParameter;
		int mCallbackHandle;
		bool mWasVisible;
	};
	
	
	
	/// Unfortunately Qt doesn't support templated classes with signals and slots
	/// so we have this extra base class along the way.
	template <typename T>
	class WidgetParameter : public WidgetBaseParameter
	{
	public:
		WidgetParameter(std::shared_ptr<Parameter<T>> parameter)
		: WidgetBaseParameter(parameter)
		, mParameter(parameter)
		, mWidget(nullptr)
		{}

	protected:
		QSize sizeHint() const override { return mWidget->sizeHint(); }

		std::shared_ptr<Parameter<T>> mParameter;
		/// Derived types should provide a pointer to the widget
		/// that is being used to control this parameter.
		QWidget* mWidget;
		
	private:
	};
	
	
	class WidgetParameterBool : public WidgetParameter<bool>
	{
		Q_OBJECT;
		
	public:
		WidgetParameterBool(ParameterPtrT<bool> parameter);
		
	Q_SIGNALS:
		void newInternalValue(bool value);
	};
	
	
	
	// ---------------------------------------
	class WidgetParameterFloat : public WidgetParameter<float>
	{
		Q_OBJECT;
		
	public:
		WidgetParameterFloat(std::shared_ptr<Parameter<float>> parameter);
		
	Q_SIGNALS:
		void newInternalValue(double value);
	};

	// ---------------------------------------
	class WidgetParameterDouble : public WidgetParameter<double>
	{
		Q_OBJECT;
		
	public:
		WidgetParameterDouble(std::shared_ptr<Parameter<double>> parameter);
		
	Q_SIGNALS:
		void newInternalValue(double value);
	};
	
	
	// ---------------------------------------
	class WidgetParameterInt : public WidgetParameter<int>
	{
		Q_OBJECT;

	public:
		WidgetParameterInt(std::shared_ptr<Parameter<int>> parameter);
		
	Q_SIGNALS:
		void newInternalValue(int value);
	};
	
	// ---------------------------------------
	class WidgetParameterString : public WidgetParameter<std::string>
	{
		Q_OBJECT;
		
	public:
		WidgetParameterString(std::shared_ptr<Parameter<std::string>> parameter);
		
	Q_SIGNALS:
		void newInternalValue(QString const& value);
	};
}





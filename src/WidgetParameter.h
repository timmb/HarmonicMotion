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
	public:
		WidgetBaseParameter();
		
		/// Factory function to create a widget of the correct type for the
		/// parameter. Delegates to createDelegate
		static WidgetBaseParameter* create(ParameterPtr parameter);
		static WidgetBaseParameter* createDelegate(std::shared_ptr<Parameter<int>> parameter);
		static WidgetBaseParameter* createDelegate(std::shared_ptr<Parameter<std::string>> parameter);
		static WidgetBaseParameter* createDelegate(std::shared_ptr<Parameter<double>> parameter);
		
	protected:
		/// Necessary to reimplement to make stylesheet work
		/// see http://stackoverflow.com/questions/7276330/qt-stylesheet-for-custom-widget
		void paintEvent(QPaintEvent*);
	};
	
	
	
	/// Unfortunately Qt doesn't support templated classes with signals and slots
	/// so we have this extra base class along the way.
	template <typename T>
	class WidgetParameter : public WidgetBaseParameter
	{
	public:
		WidgetParameter(std::shared_ptr<Parameter<T>> parameter)
		: mParameter(parameter)
		{}


		
	protected:
		std::shared_ptr<Parameter<T>> mParameter;
	};
	

	// ---------------------------------------
	class WidgetParameterDouble : public WidgetParameter<double>
	{
		Q_OBJECT;
		
	public:
		WidgetParameterDouble(std::shared_ptr<Parameter<double>> parameter);
		
	Q_SIGNALS:
		void newInternalValue(double value);
		
	protected:
		QSize sizeHint() const { return mSpinBox->sizeHint(); }
		
	private:
		QDoubleSpinBox* mSpinBox;
	};
	
	
	// ---------------------------------------
	class WidgetParameterInt : public WidgetParameter<int>
	{
		Q_OBJECT;

	public:
		WidgetParameterInt(std::shared_ptr<Parameter<int>> parameter);
		
	Q_SIGNALS:
		void newInternalValue(int value);
		
	protected:
		QSize sizeHint() const { return mSpinBox->sizeHint(); }
		
	private:
		QSpinBox* mSpinBox;
	};
	
	// ---------------------------------------
	class WidgetParameterString : public WidgetParameter<std::string>
	{
		Q_OBJECT;
		
	public:
		WidgetParameterString(std::shared_ptr<Parameter<std::string>> parameter);
		
	Q_SIGNALS:
		void newInternalValue(QString const& value);
		
	protected:
		QSize sizeHint() const { return mWidget->sizeHint(); }
		
	private:
		QLineEdit* mWidget;
	};
}





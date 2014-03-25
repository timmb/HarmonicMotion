//
//  Common.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 26/09/2013.
//
//

#pragma once
#include <memory>
#include <vector>
#include <ostream>
#include <iostream>
#include <sstream>
#include "Type.h"

namespace hm
{
	// forward declarations
	class Inlet;
	class Outlet;
	class Node;
	class BaseParameter;
	class PatchCord;
	class Pipeline;
	
	// forward declare data types
	class Value;
	class Point3d;
	class Skeleton3d;
	class Scene3d;
	
	// typedefs
	typedef std::shared_ptr<Inlet> InletPtr;
	typedef std::shared_ptr<Outlet> OutletPtr;
	typedef std::shared_ptr<Node> NodePtr;
	typedef std::shared_ptr<BaseParameter> ParameterPtr;
	typedef std::shared_ptr<const BaseParameter> ParameterConstPtr;
	typedef std::shared_ptr<PatchCord> PatchCordPtr;
	typedef std::shared_ptr<Pipeline> PipelinePtr;
	
	/// \return number of seconds since the program began
	double elapsedTime();
	
	std::vector<Type> const& listOfTypes();
	
	// Check if STL container contains an element
	template <typename Container, typename Value>
	bool contains(Container container, Value const& value)
	{
		return std::find(container.begin(), container.end(), value) != container.end();
	}
	
	
	
	/// Wrapper to add indent to output stream whenever
	/// '\n' or std::endl is passed.
	template <typename OutputStream>
	class OStreamIndenter 
	{
	public:
		OStreamIndenter(OutputStream & out, int indentSize_ = 2);
		OStreamIndenter(OStreamIndenter<OutputStream> const& rhs);
		
		OutputStream& out;
		const int indentSize;
		/// Print new line and indent
		void newLineIndent();
	};
	
	template <typename OutputStream>
	OStreamIndenter<OutputStream>::OStreamIndenter(OutputStream & out_, int indentSize_)
	: out(out_)
	, indentSize(indentSize_)
	{}
	
	template <typename OutputStream>
	OStreamIndenter<OutputStream>::OStreamIndenter(OStreamIndenter<OutputStream> const& rhs)
	: out(rhs.out)
	, indentSize(rhs.indentSize)
	{}
	
	template <typename OutputStream>
	void OStreamIndenter<OutputStream>::newLineIndent()
	{
		out << '\n';
		for (int i=0; i<indentSize; i++)
		{
			out << ' ';
		}
	}
	
	template <typename OutputStream, typename T>
	OStreamIndenter<OutputStream>& operator<<(OStreamIndenter<OutputStream> & out, T const& rhs)
	{
		std::stringstream ss;
		ss << rhs;
		return out << ss.str();
	}
	
	template <typename OutputStream, typename _CharT, typename _Traits>
	OStreamIndenter<OutputStream>& operator<<(OStreamIndenter<OutputStream> & out, std::basic_ostream<_CharT, _Traits> rhs)
	{
		if (rhs==std::endl)
		{
			out.newLineIndent();
		}
		else
		{
			out.out << rhs;
		}
		return out;
	}
	
	template <typename OutputStream>
	OStreamIndenter<OutputStream>& operator<<(OStreamIndenter<OutputStream> & out, std::string const& rhs)
	{
		auto lineStart = 0;
		auto lineEnd = rhs.find('\n');
		for (;;)
		{
			out.out << rhs.substr(lineStart, lineEnd);
			if (lineEnd==std::string::npos)
			{
				break;
			}
			lineStart = lineEnd + 1;
			lineEnd = rhs.find('\n', lineStart);
			out.newLineIndent();
		}
		return out;
	}
	
	template <typename OutputStream>
	OStreamIndenter<OutputStream>& operator<<(OStreamIndenter<OutputStream> & out, char const* rhs)
	{
		out.out << std::string(rhs);
		return out;
	}
	
	template <typename OutputStream>
	OStreamIndenter<OutputStream> indent(OutputStream & out, int indentSize=2)
	{
		return OStreamIndenter<OutputStream>(out, indentSize);
	}

	
	

	
	// For printing verbose debugging info
#if defined(DEBUG) || defined(_DEBUG)
	#ifndef HM_LOG_DEBUG
		#define HM_LOG_DEBUG
	#endif
#endif
	
#define HM__FILE_NO_DIR__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef HM_LOG_DEBUG
	#define hm_debug(message) std::cerr << HM__FILE_NO_DIR__ << ' ' <<__LINE__<<' '<<message<<std::endl;
#else
	#define hm_debug(message) ;;
#endif

#define hm_info(message) std::cerr << "* Info: "<<message<<std::endl;
#define hm_warning(message) std::cerr << "*Warning: "<<message<<std::endl;
	
#define hm_error(message) std::cerr << "* Error:" << HM__FILE_NO_DIR__ << ' ' << __LINE__<<'\n'<<message<<std::endl;
	

}
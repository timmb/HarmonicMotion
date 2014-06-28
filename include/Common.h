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
#include "Type.h"
//#include "TypeTraits.h"
#include "SceneMeta.h"
#include "BaseData.h"

namespace hm
{
	// forward declarations
	class Data;
	class Let;
	class Inlet;
	class Outlet;
	class Node;
	class BaseParameter;
	template <typename T>
	class Parameter;
	class PatchCord;
	class Pipeline;
	class Listener;
	class Renderer;
	
	
	// typedefs
	typedef std::shared_ptr<Let> LetPtr;
	typedef std::shared_ptr<Inlet> InletPtr;
	typedef std::shared_ptr<Outlet> OutletPtr;
	typedef std::shared_ptr<Node> NodePtr;
	typedef std::shared_ptr<BaseParameter> ParameterPtr;
	template <typename T>
	using ParameterPtrT = std::shared_ptr<Parameter<T>>;
	typedef std::shared_ptr<const BaseParameter> ParameterConstPtr;
	typedef std::shared_ptr<PatchCord> PatchCordPtr;
	typedef std::shared_ptr<Pipeline> PipelinePtr;
	typedef std::shared_ptr<Listener> ListenerPtr;
	typedef std::shared_ptr<Renderer> RendererPtr;
	
	/// \return number of seconds since the program began
	double elapsedTime();
	
	// Check if STL container contains an element
	template <typename Container, typename Value>
	bool contains(Container container, Value const& value)
	{
		return std::find(container.begin(), container.end(), value) != container.end();
	}
	
	/// Add an indent to newlines on a string
	std::string indent(std::string const& s, int indentAmount=2);
	
	
	
	/// Get the timestamp from either Data or a class derived from BaseData
	double getTimestamp(Data const& x);
	double getTimestamp(BaseData const& x);
	
	
	inline double chooseTimestamp(BaseData const& x, BaseData const& y)
	{
		return std::max(x.timestamp, y.timestamp);
	}

	inline double chooseTimestamp(BaseData const& x, BaseData const& y, BaseData const& z)
	{
		return std::max(chooseTimestamp(x, y), z.timestamp);
	}
	
	template <typename T> inline
	typename std::enable_if<!std::is_base_of<BaseData, T>::value, double>::type
	chooseTimestamp(T x, BaseData const& y)
	{
		return y.timestamp;
	}
	
	template <typename T> inline
	typename std::enable_if<!std::is_base_of<BaseData, T>::value, double>::type
	chooseTimestamp(BaseData const& x, T y)
	{
		return x.timestamp;
	}
	
	
	
	inline int chooseId(BaseData const& x, BaseData const& y)
	{
		return x.id==BaseData::sDefaultId? y.id : x.id;
	}
	
	inline int chooseId(BaseData const& x, BaseData const& y, BaseData const& z)
	{
		int const& id = chooseId(x, y);
		return id==BaseData::sDefaultId? z.id : id;
	}
	
	template <typename T> inline
	typename std::enable_if<!std::is_base_of<BaseData, T>::value, int>::type
	chooseId(T x, BaseData const& y)
	{
		return y.id;
	}
	
	template <typename T> inline
	typename std::enable_if<!std::is_base_of<BaseData, T>::value, int>::type
	chooseId(BaseData const& x, T y)
	{
		return x.id;
	}

		
	
	inline SceneMetaPtr chooseSceneMeta(BaseData const& x, BaseData const& y)
	{
		return x.sceneMeta==SceneMeta::defaultSceneMeta()? y.sceneMeta : x.sceneMeta;
	}
	
	inline SceneMetaPtr chooseSceneMeta(BaseData const& x, BaseData const& y, BaseData const& z)
	{
		SceneMetaPtr const& s = chooseSceneMeta(x, y);
		return s==SceneMeta::defaultSceneMeta()? z.sceneMeta : s;
	}
	
	template <typename T> inline
	typename std::enable_if<!std::is_base_of<BaseData, T>::value, SceneMetaPtr>::type
	chooseSceneMeta(T x, BaseData const& y)
	{
		return y.sceneMeta;
	}
	
	template <typename T> inline
	typename std::enable_if<!std::is_base_of<BaseData, T>::value, SceneMetaPtr>::type
	chooseSceneMeta(BaseData const& x, T y)
	{
		return x.sceneMeta;
	}


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

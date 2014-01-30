//
//  Parameter.h
//
//  Created by Tim Murray-Browne on 20/02/2013.
//
//

#pragma once

#include "json/json.h"
#include "cinder/Vector.h"
#include <string>

namespace hm {

	/// Base class of all parameters
	class BaseParameter
	{
	public:
		virtual ~BaseParameter() {}
		
		/// Given the root of a JSON tree, this will read/write
		/// a value based on \c mPath.
		virtual void writeJson(Json::Value& root) const;
		/// \copydoc writeJson
		virtual bool readJson(Json::Value const& root);
		
		std::string mPath;
		
		
	protected:
		/// \param path Slash separated with the final element being the
		/// user-visible name of this parameter.
		/// e.g. "/Accumulator1/smoothing value"
		/// or "/value"
		BaseParameter(std::string const& path="");

		virtual void toJson(Json::Value& node) const = 0;
		virtual bool fromJson(Json::Value const& node) = 0;

	private:
		/// \return the JSON element corresponding to the given path
		template <typename JsonOrConstJson>
		static JsonOrConstJson& getChild(JsonOrConstJson& root, std::string const& path);
	};
	typedef std::shared_ptr<BaseParameter> ParameterPtr;
	
	template <typename T>
	T& BaseParameter::getChild(T& root, std::string const& path)
	{
		auto slashPos = path.find_first_of('/');
		// Treat as a name if there are no slashes or final character is a slash
		if (slashPos==std::string::npos || slashPos==path.size()-1)
		{
			return root[path];
		}
		else
		{
			assert(slashPos < path.size());
			std::string head = path.substr(0, slashPos);
			std::string tail = path.substr(slashPos+1);
			return getChild(root[head], tail);
		}
	}

	
	// Main class to represent parameters
	template <typename T>
	class Parameter : public BaseParameter
	{
	public:
		Parameter(std::string const& path, T* value)
		: BaseParameter(path)
		, mValue(value)
		{
			assert(mValue != nullptr);
		}
		
	protected:
		virtual void toJson(Json::Value& child) const override
		{
			child << *mValue;
		}
		
		virtual bool fromJson(Json::Value const& child) override
		{
			T tempValue;
			if (child >> tempValue)
			{
				child >> *mValue;
				return true;
			}
			else
				return false;
		}
		
	private:
		T* mValue;
	};
	
	
	
} // namespace tmb

namespace Json
{
	// To avoid having to specialise the Parameter class, we overload
	// the streaming operator for all supported datatypes
	bool operator>>(Json::Value const& child, double& value);
	bool operator>>(Json::Value const& child, int& value);
	bool operator>>(Json::Value const& child, ci::Vec3f& value);
	bool operator>>(Json::Value const& child, std::string& value);
	
	Json::Value& operator<<(Json::Value& lhs, ci::Vec3f const& rhs);

	template <typename T>
	Json::Value& operator<<(Json::Value& lhs, T const& rhs)
	{
		lhs = rhs;
		return lhs;
	}
	
}

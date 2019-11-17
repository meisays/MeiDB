#pragma once

#include "BasicType.h"

#include <sstream>

namespace MeiDB {
	template <typename T>
	struct Convert {
		static String ToString(const T& value) { return String(); }
	};

	template<typename T>
	String ToString(const T& value) {
		wstringstream ss;
		ss << value;
		String result;
		ss >> result;
		return result;
	}

	template<typename T>
	T Parse(const String& value) {
		wstringstream ss;
		ss << value;
		T result;
		ss >> result;
		return result;
	}

	template<typename T>
	struct ToStringTrait {
		enum {Supported = false};
	};

	//这里的意思也就是说 ToStringTrait<String> 的实例我们已经提供了。  提供了一个特例，
	//string
	template <>
	struct ToStringTrait<String> {
		enum {Supported = true};
	};

	template <>
	struct Convert<String> 
	{
		static String ToString(const String& value) {
			return value;
		}
	};

	//Int 
	template<>
	struct ToStringTrait<Int32> 
	{
		enum {Supported = true};
	};

	template<>
	struct Convert<Int32> {
		static String ToString(const Int32& value) 
		{
			return MeiDB::ToString(value);
		}

		static Int32 Parse(const String& value) 
		{
			return MeiDB::Parse<Int32>(value);
		}
	};

	//UInt32
	template<>
	struct ToStringTrait<UInt32> {
		enum {Supported = true};
	};

	template<>
	struct Convert<UInt32> {
		static String ToString(const Int32& value) {
			return MeiDB::ToString<UInt32>(value);
		}
	};
}
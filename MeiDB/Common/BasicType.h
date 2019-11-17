//
//define primitive types
//

#pragma once

#include <string>
#include <memory>

using namespace std;

namespace MeiDB {
	//统一命名格式

	typedef char Byte;
	typedef __int16 Int16;
	typedef __int32 Int32;
	typedef __int64 Int64;
	typedef unsigned __int16 UInt16;
	typedef unsigned __int32 UInt32;
	typedef unsigned __int64 UInt64;

	typedef bool Bool;

	typedef wchar_t Char;
	typedef wstring String;
	typedef shared_ptr<String> PString;

	#define T(X) L##X
	#define STR(X) String(L ## X)

	template<typename T>
	struct IEnumerator {
		typedef shared_ptr<IEnumerator<T>> Ptr;

		virtual bool move_next();
		virtual const T current();
		virtual ~IEnumerator();
	};

	template<typename T>
	struct IEnumerator<T*> {
		typedef shared_ptr<IEnumerator<T*>> Ptr;

		virtual bool move_next() = 0;
		virtual T* current() = 0;
		virtual ~IEnumerator() {}
	};

	template<typename T>
	struct IEnumerator<shared_ptr<T>> {
		typedef shared_ptr<IEnumerator<shared_ptr<T>>> Ptr;
		virtual bool move_next() = 0;
		virtual shared_ptr<T> current() = 0;
		virtual ~IEnumerator() {}
	};

	class Exception {
	protected:
		String message;
	public:
		Exception(const String& message = String()) 
		:message(message){

		}

		const String& get_msg() const {
			return message;
		}
	};

	class OutOfRangeException : public Exception {
	public:
		OutOfRangeException(const String& msg = String())
			:Exception(msg) {}
	};

	class InvalidOperationException : public Exception {
	public:
		InvalidOperationException(const String& msg = String())
			:Exception(msg) {};
	};
}
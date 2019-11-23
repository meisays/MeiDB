#pragma once

/**
***
**/
#include "..\..\Common\BasicType.h"

#include <vector>
#include <memory>
using namespace std;



namespace MeiDB {
	struct DbValue {
		typedef shared_ptr<DbValue> Ptr;
		DbValue(bool is_null) : is_null(is_null) {}
		DbValue() : is_null(true) {}
		bool is_null;

		virtual Int32 size() = 0;
		virtual const Byte* data() = 0;

		virtual ~DbValue() {}
	};

	struct DbInt : public DbValue {
		typedef shared_ptr<DbInt> Ptr;

		DbInt() : DbValue(true), value(0) {}
		DbInt(const Int32& value) : DbValue(false), value(value) {}

		Int32 value;

		virtual Int32 size() override {
			return sizeof(Int32);
		}

		virtual const Byte* data() override {
			return reinterpret_cast<Byte*>(&value);
		}

	};

	struct DbFloat : public DbValue {
		typedef shared_ptr<DbFloat> Ptr;
		DbFloat() : DbValue(true), value(0.0) {}
		DbFloat(const float& value) : DbValue(false), value(value) {}

		virtual Int32 size() override {
			return sizeof(float);
		}

		virtual const Byte* data() override {
			return reinterpret_cast<Byte*>(&value);
		}

		float value;
	};

	struct DbString : public DbValue {
		typedef shared_ptr<DbString> Ptr;
		DbString() : DbValue(true) {};
		DbString(const String& str) : DbValue(false), value(str) {}

		virtual Int32 size() override {
			return value.length() * sizeof(Char);
		}

		virtual const Byte* data() override {
			//强制类型转换
			return (Byte*)this->value.data();
		}

		String value;
	};

	struct IDataRow {
		typedef shared_ptr<IDataRow> Ptr;
		virtual shared_ptr<DbValue> get_value(const String& columnName) = 0;
		virtual shared_ptr<DbValue> get_value(const Int32& columnIndex) = 0;

		template<typename T>
		shared_ptr<T> get_value(const String& column_name) {
			return dynamic_pointer_cast<T>(this->get_value(column_name));
		}

		template<typename T>
		shared_ptr<T> get_value(const Int32& column_index) {
			return dynamic_pointer_cast<T>(this->get_value(column_index));
		}

		virtual ~IDataRow() {}
	};

	typedef public IEnumerator<IDataRow::Ptr> IDataReader;
}
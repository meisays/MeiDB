#pragma once

#include <vector>
#include <map>
using namespace std;

#include "..\Common\BasicType.h"

namespace MeiDB {
	enum class SqlType {
		Int,
		Float, 
		String
	};

	//接口  类型
	struct IColumnDef {
		typedef shared_ptr<IColumnDef> Ptr;

		virtual String column_name() = 0;
		virtual SqlType column_type() = 0;
	};

	struct ColumnDef : public IColumnDef {
		typedef shared_ptr<ColumnDef> Ptr;

		ColumnDef(const String& name, const SqlType& type) 
			: _name(name), _type(type) {}

		virtual String column_name() override {
			return this->_name;
		}

		virtual SqlType column_type() override {
			return this->_type;
		}

		String _name;
		SqlType _type;
	};

	//
	struct ITupleDesc {
		typedef shared_ptr<ITupleDesc> Ptr;

		virtual int get_ordinal(const String& column_name) = 0;
		virtual IColumnDef::Ptr get_column_desc(const Int32& column_index) = 0; //

		bool contain_column(const String& column_name) {
			return this->get_ordinal(column_name) != -1;
		}
	};

	struct TupleDesc : public ITupleDesc {
		typedef shared_ptr<TupleDesc> Ptr;
		void add_column(const ColumnDef& column) {
			ColumnDef::Ptr column_ptr = make_shared<ColumnDef>(column);
			_column_list.push_back(column_ptr);  //column 名字重复怎么处理
			_column_map[column._name] = _column_list.size() - 1; // index begin from 0
			//_column_map.insert(column_ptr->column_name, _column_list.size() - 1>);
		}

		void add_column(const String& name, SqlType type) {
			this->add_column(ColumnDef(name, type));
		}

		virtual int get_ordinal(const String& column_name) override {
			auto it = this->_column_map.find(column_name);
			if (it != this->_column_map.end()) {
				return it->second;
			}
			else {
				return -1;
			}
		}

		virtual IColumnDef::Ptr get_column_desc(const Int32& column_index) override {
			return _column_list[column_index];
		}


	public:
		vector<ColumnDef::Ptr> _column_list; 
	private:
		map<String, int> _column_map; //index-name, snapshot for a aolumn name with index
	};
}


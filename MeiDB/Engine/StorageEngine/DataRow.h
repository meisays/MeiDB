#pragma once
#include "..\Client\Client.h"
#include "..\..\Common\BasicType.h"
#include "..\Schema.h"

#include <vector>
#include <memory>
#include <algorithm>
using namespace std;

namespace MeiDB {
	//we use variable size DataRow layout
	//for a row with n fields, The DataRow starts with an int16[n+1] array
	//in which ths [i] stores the start offset of the  i-th fields(starting from 0)
	//offset of -1 means that fields is null
	//Todo make the layout aligned

	class DataRow : public IDataRow {
	public:
		static const Int16 NullOffset = -1;

		static Int32 CalculateRowSize(const std::vector<DbValue::Ptr>& fields) {
			int total_size = 2;
			for_each(fields.begin(), fields.end(), [&](DbValue::Ptr v) {total_size += (v->size() + 2); });

			return total_size;
		}

		//this method assume pointer of destination has enough sapce to hold all the values
		static void CopyDataRow(Byte* destination, std::vector<DbValue::Ptr>& fields) {
			//Byte* current = destination;
			//for_each(fields.begin(), fields.end(), [&](DbValue::Ptr v) {
			//	auto offset_size = sizeof(Int16);
			//	//set each row's start offset
			//	memcpy(current, v->data(), v->size());
			//	current += v->size();
			//});
			auto fields_count = fields.size();
			Int16* offset_des = (Int16*)destination;
			Int16 offset_size = sizeof(Int16) * (fields_count + 1);
			Byte* current_data_des = destination + offset_size;

			for (int i = 0; i < fields_count; i++) {
				DbValue::Ptr column = fields[i];
				Int32 field_size = column->size();

				if (column->is_null) {
					offset_des[i] = NullOffset;
					//current_data_des += sizeof(Int16);
				}
				else {
					// fields is not null
					memcpy(current_data_des, column->data(), field_size);
					offset_des[i] = current_data_des - destination;

					current_data_des += field_size;
				}
			}

			offset_des[fields_count] = current_data_des - destination;
		}

	public:

		DataRow(ITupleDesc::Ptr schema, Byte* data):
			schema(schema), data(data) { }

		virtual shared_ptr<DbValue> get_value(const String& column_name) override {
			int column_index = schema->get_ordinal(column_name);
			return get_value(column_index);
		}

		virtual shared_ptr<DbValue> get_value(const Int32& column_index) override {
			//first: find out the offset array
			Int16* offset_ptr = (Int16*)data;
			Int16 offset = offset_ptr[column_index];
			
			//if (offset == NullOffset)
				//return nullptr;
			//second: find data pointer, locate data
			Byte* row_data = data + offset;

			IColumnDef::Ptr column = schema->get_column_desc(column_index);
			switch (column->column_type()){
			case SqlType::Float:
				if (offset == NullOffset) {
					return make_shared<DbFloat>();
				}
				else {
					return make_shared<DbFloat>(*((float*)row_data));
				}
				break;
			case SqlType::Int:
				if (offset == NullOffset) {
					return make_shared<DbInt>();
				}
				else {
					return make_shared<DbInt>(*((Int32*)row_data));
				}
				break;
			case SqlType::String:
				if (offset == NullOffset) {
					return make_shared<DbString>();
				}
				else {
					Int16 field_size = offset_ptr[column_index + 1] - offset_ptr[column_index];
					return make_shared<DbString>(String((Char*)row_data, field_size / sizeof(Char) ));
				}
				break;
			default:
				break;
			}
		}



		ITupleDesc::Ptr schema;
		Byte* data;
	};

}




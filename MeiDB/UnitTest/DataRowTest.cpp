#include "Framework\UnitTest.h"

#include "..\Engine\QueryProcessor\Tokenizer.h"
#include "..\Engine\StorageEngine\Page.h"
#include "..\Engine\StorageEngine\DataRow.h"

#include <iostream>
#include <vector>



namespace MeiDB {
	namespace DataRowTest {

		void case1() {

			std::cout << "case 1: test for insert data row" << std::endl;
			DataPage data_page(PageId{1});

			TupleDesc::Ptr schema = make_shared<TupleDesc>();
			schema->add_column(T("id"), SqlType::Int);
			schema->add_column(T("data"), SqlType::String);
			schema->add_column(T("Region"), SqlType::Int);

			vector<DbValue::Ptr> fields;
			fields.push_back(make_shared<DbInt>(1)); //size 4
			fields.push_back(make_shared<DbString>(STR("Test Data 1"))); //size 12
			fields.push_back(make_shared<DbInt>(10)); //size 4

			RowPtr row_ptr = data_page.insert_data_row(fields);

			IDataRow::Ptr r = make_shared<DataRow>(schema, row_ptr.data);

			DbInt::Ptr r1 = r->get_value<DbInt>(T("id"));
			DbString::Ptr r2 = r->get_value<DbString>(T("data"));
			DbInt::Ptr r3 = r->get_value<DbInt>(T("Region"));

			Assert::AreEqual(r1->value, 1);
			Assert::IsFalse(r1->is_null);

			Assert::AreEqual(r2->value, String(STR("Test Data 1")));
			Assert::IsFalse(r2->is_null);

			Assert::AreEqual(r3->value, 10);
			Assert::IsFalse(r3->is_null);

		}

		void case2() {
			std::cout << "case 2 : test DataRow get value with column index" << std::endl;
			DataPage data_page(PageId{ 1 });

			TupleDesc::Ptr schema = make_shared<TupleDesc>();
			schema->add_column(T("id"), SqlType::Int);
			schema->add_column(T("data"), SqlType::String);
			schema->add_column(T("Region"), SqlType::Int);

			vector<DbValue::Ptr> fields;
			fields.push_back(make_shared<DbInt>(1)); //size 4
			fields.push_back(make_shared<DbString>(STR("Test Data 1"))); //size 12
			fields.push_back(make_shared<DbInt>(10)); //size 4

			RowPtr row_ptr = data_page.insert_data_row(fields);

			IDataRow::Ptr r = make_shared<DataRow>(schema, row_ptr.data);

			DbInt::Ptr r1 = r->get_value<DbInt>(0);
			DbString::Ptr r2 = r->get_value<DbString>(1);
			DbInt::Ptr r3 = r->get_value<DbInt>(2);

			Assert::AreEqual(r1->value, 1);
			Assert::IsFalse(r1->is_null);

			Assert::AreEqual(r2->value, String(STR("Test Data 1")));
			Assert::IsFalse(r2->is_null);

			Assert::AreEqual(r3->value, 10);
			Assert::IsFalse(r3->is_null);
		}

		void case3() {
			std::cout << "case 3 : test for null value" << std::endl; 
			DataPage page(PageId{1});

			TupleDesc::Ptr schema = make_shared<TupleDesc>();
			schema->add_column(T("id"), SqlType::Int);
			schema->add_column(T("data"), SqlType::String);
			schema->add_column(T("region"), SqlType::Int);
			schema->add_column(T("desc"), SqlType::String);

			vector<DbValue::Ptr> fields;
			fields.push_back(make_shared<DbInt>(1));
			fields.push_back(make_shared<DbString>());
			fields.push_back(make_shared<DbInt>(3));
			fields.push_back(make_shared<DbString>(STR("i love c++ ;;; 2019-11-23")));

			RowPtr row_ptr = page.insert_data_row(fields);
			DataRow::Ptr data_row = make_shared<DataRow>(schema, row_ptr.data);

			DbInt::Ptr c1 = data_row->get_value<DbInt>(T("id"));
			DbString::Ptr c2 = data_row->get_value<DbString>(T("data"));
			DbInt::Ptr c3 = data_row->get_value<DbInt>(T("region"));
			DbString::Ptr c4 = data_row->get_value<DbString>(T("desc"));

			Assert::IsFalse(c1->is_null);
			Assert::AreEqual(c1->value, 1);

			Assert::IsTrue(c2->is_null);

			Assert::IsFalse(c3->is_null);
			Assert::AreEqual(c3->value, 3);

			Assert::IsFalse(c4->is_null);
			Assert::AreEqual(c4->value, STR("i love c++ ;;; 2019-11-23"));

		}

		void test_all_case() {
			std::cout << "data row test start: -->" << std::endl;
			case1();
			case2();
			case3();
		}
	}
}
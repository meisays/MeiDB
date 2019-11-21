
#include <cstdio>
#include "..\Engine\StorageEngine\PageFile.h"
#include <iostream>

#include "Framework\UnitTest.h"
#include "TestUtility.h"

namespace MeiDB {
	namespace PageFileTest {
		void case1() {
			remove("test.df");

			std::cout << "case1 : test for update file" << std::endl;
			
			PageFile file(STR("test.df"));
			file.open();
			
			DataPage page(0);
			Int16  row1 = 1;
			Int32 row2 = 2;

			RowId row_id1 = page.insert_row_value(row1);
			RowId row_id2 = page.insert_row_value(row2);

			file.allocate_page(&page);
			file.flush();

			DataPage* page_from_file = file.read_page_as<DataPage>(0);
			Assert::AreEqual(page, *page_from_file);

			Int64 row3 = 3;
			RowId row_id3 = page.insert_row_value(row3);

			file.write_page(&page);
			file.flush();

			page_from_file = file.read_page_as<DataPage>(0);
			Assert::AreEqual(page, *page_from_file);

			file.flush();
			file.close();

		}

		void case2() {
			remove("test.df");
			std::cout << "case2 : test for append file" << std::endl;

			PageFile file(STR("test.df"));

			file.open();

			DataPage page(0);
			Int16 row1 = 1;
			Int32 row2 = 2;
			Int64 row3 = 3;

			page.insert_row_value(row1);
			page.insert_row_value(row2);
			page.insert_row_value(row3);

			file.allocate_page(&page);
			file.flush();
			file.close();

			file.open();
			DataPage* page_from_file = file.read_page_as<DataPage>(0);
			Assert::AreEqual(page, *page_from_file);

			file.flush();
			file.close();
		}

		void case3() {
			remove("test.df");

			std::cout << "case3 : test file size" << std::endl;

			Int32 row1 = 1;
			Int64 row2 = 2;
			DataPage page1(0);
			DataPage page2(1);

			PageFile file(STR("test.df"));
			file.open();
			page1.insert_row_value(row1);
			file.allocate_page(&page1);
			file.flush(); file.close();

			file.open();
			Assert::AreEqual<UInt32>(sizeof(DataPage), file.size());

			page2.insert_row_value(row2);
			file.allocate_page(&page2);
			
			auto tt2 = file.size();
			file.flush(); 
			
			auto tt = file.size();

			file.close();

			file.open();

			auto size_page = sizeof(DataPage);

			Assert::AreEqual<UInt32>(2 * sizeof(DataPage), file.size());
			file.close();
		}

		void case4() {
			remove("test.df");

			std::cout << "case4 : test page id generation " << std::endl;

			PageFile file(STR("test.df"));
			file.open();

			DataPage page(0);
			file.allocate_page(&page);
			file.flush();

			DataPage* page_from_file = file.read_page_as<DataPage>(0);

			Assert::AreEqual(PageSize, file.size());
			file.flush();

			DataPage page1(1);
			file.allocate_page(&page1);
			file.flush(); file.close();

			file.open();
			Assert::AreEqual(2* PageSize, file.size());

			page_from_file = file.read_page_as<DataPage>(0);
			Assert::AreEqual(*page_from_file, page);

			page_from_file = file.read_page_as<DataPage>(1);
			Assert::AreEqual(*page_from_file, page1);
		}



		void test_all_case() {
			case1();
			case2();
			case3();
			case4();
		}
	}
}


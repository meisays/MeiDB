#include "Framework\UnitTest.h"
#include "TestUtility.h"

#include "..\Engine\StorageEngine\Heap.h"
#include "..\Engine\Client\Client.h"

#include <iostream>
using namespace std;

namespace MeiDB {
	namespace HeapTest {
		const string file_name = "test_heap.hdf";
		void case1() {
			std::cout << "\ttest basic heap operation " << std::endl;
			remove(file_name.c_str());

			Assert::AreEqual<int>(PageSize, sizeof(SimpleHeapHeaderPageData));

			BufferManager buffer;
			SimpleHeap heap(&buffer);
			
			Int32 row1 = 1;
			Int32 row2 = 2;
			Int32 row3 = 3;

			RowId row_id1 = heap.insert_row_value(row1);
			RowId row_id2 = heap.insert_row_value(row2);
			RowId row_id3 = heap.insert_row_value(row3);

			IEnumerator<RowPtr>::Ptr enumerator = heap.get_enumerator();
			Assert::IsTrue(enumerator->move_next());

			AssertRowPtr(row1, enumerator->current());

			Assert::IsTrue(enumerator->move_next());
			AssertRowPtr(row2, enumerator->current());

			Assert::IsTrue(enumerator->move_next());
			AssertRowPtr(row3, enumerator->current());

			Assert::IsFalse(enumerator->move_next());
			Assert::IsFalse(enumerator->move_next());
		}

		void case2() {
			std::cout << "\ttest persist heap" << std::endl;
			remove(file_name.c_str());

			Int32 row1 = 1;
			Int32 row2 = 2;
			Int32 row3 = 3;

			PageId header_page_id = InvalidPageId;

			{
				BufferManager buffer(T("test_heap.hdf"));
				SimpleHeap heap(&buffer);

				header_page_id = heap.get_header_page_id();
				heap.insert_row_value(row1);
				heap.insert_row_value(row2);
				heap.insert_row_value(row3);
			}

			BufferManager buffer(T("test_heap.hdf"));
			SimpleHeap heap(&buffer, header_page_id);

			IEnumerator<RowPtr>::Ptr enumerator = heap.get_enumerator();

			Assert::IsTrue(enumerator->move_next());

			AssertRowPtr(row1, enumerator->current());

			Assert::IsTrue(enumerator->move_next());
			AssertRowPtr(row2, enumerator->current(), L"The data is not correctly set for row2.");

			Assert::IsTrue(enumerator->move_next());
			AssertRowPtr(row3, enumerator->current(), L"The data is not correctly set for row3.");

			Assert::IsFalse(enumerator->move_next());
			Assert::IsFalse(enumerator->move_next());

			//BufferManager buffer_manager;
			//SimpleHeap 
		}

		void test_all_case() {
			std::cout << "test for heap. start ----->" << std::endl;
			case1();
			case2();
		}
	}
}
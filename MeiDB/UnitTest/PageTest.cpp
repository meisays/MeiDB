
#include "..\Engine\StorageEngine\Page.h"
#include "..\Engine\QueryProcessor\Tokenizer.h"

//#include ""
#include "Framework\UnitTest.h"
#include "TestUtility.h"
#include <iostream>


namespace MeiDB {
	namespace PageTest {
		void case1() {
			Assert::AreEqual<int>(PageSize, sizeof(DataPage), L"Page size is not equal to PageSize");
		}

		void case2() {
			Page test;
			DataPage data_page(PageId{ 50 });
			Page* page = (Page*)(&data_page);
			Assert::AreEqual({ 50 }, page->get_page_id(), L"pageid is not correctly return accessing DataPage as page");
		}

		void case3() {
			Assert::AreEqual<int>(PageSize, sizeof(DataPage), L"Datapage size is not equal to PagaSIze");

			//Test init
			DataPage page(PageId{1});
			Assert::AreEqual(0, page.get_row_count(), L"the row count of new data page is not 0");


		}

		void case4() {
			//test insert one row
			DataPage page(PageId{ 1 });
			Int32 row_data = 9;
			RowId rid = page.insert_row_value(row_data);
			Assert::AreEqual(1, page.get_row_count(), L"Row Count of thw page is not 1");

			Int32 return_data = 0;
			RowPtr row_ptr = page.get_row_ptr(rid);
			AssertRowPtr(row_data, row_ptr);

			Int32 max_data = 0xffffffff;
			rid = page.insert_row_value(max_data);
			Assert::AreEqual(2, page.get_row_count(), L"Row Count of thw page is not 1");

			row_ptr = page.get_row_ptr(rid);
			AssertRowPtr(max_data, row_ptr);

			//Int32 length = row_ptr.length;
			//Int32 value = *((Int32*)(row_ptr.data));
			//Assert::AreEqual<Int32>(sizeof(Int32), length);
			//Assert::AreEqual<Int32>(value, row_data);

		}


		void case5(){
			std::cout << "case5 test insert thress rows" << std::endl;
			
			DataPage page(1); // page_id =1
			Int16 row1 = 1;
			Int16 row2 = 2;
			Int16 row3 = 3;

			RowId row_id1 = page.insert_row_value(row1);
			RowId row_id2 = page.insert_row_value(row2);
			RowId row_id3 = page.insert_row_value(row3);

			Assert::AreEqual(3, page.get_row_count());
			RowPtr row_ptr1 = page.get_row_ptr(row_id1);
			RowPtr row_ptr2 = page.get_row_ptr(row_id2);
			RowPtr row_ptr3 = page.get_row_ptr(row_id3);

			//Assert::AreEqual(row1, );
			AssertRowPtr(row1, row_ptr1);
			AssertRowPtr(row2, row_ptr2);
			AssertRowPtr(row3, row_ptr3);

		}

		void case6() {
			std::cout << "case6: test page equal" << std::endl;

			DataPage page1(1);
			DataPage page2(1);
			
			Int16 row1 = 1;
			Int16 row2 = 2;
			Int16 row3 = 3;

			page1.insert_row_value(row1);
			page1.insert_row_value(row2);
			page1.insert_row_value(row3);

			page2.insert_row_value(row1);
			page2.insert_row_value(row2);
			page2.insert_row_value(row3);

			Assert::AreEqual(page1, page2);

		}

		void case7() {
			std::cout << "case7: test delete row" << std::endl;
			DataPage page(1);
			Int16 row1 = 1; //offset 0 (before delete), offset 0 (after delete)
			Int32 row2 = 2;// offset 2, 2
			Int16 row_tobe_del = 99; //offset 6, -
			Int64 row3 = 3; // offset 8, 6
			Int32 row4 = 4; //offset 16, 14

			Int16 offset1 = 0;
			Int16 offset2 = offset1 + sizeof(Int16) / sizeof(Byte);
			Int16 offset3 = offset2 + sizeof(Int32) / sizeof(Byte);
			Int16 offset4 = offset3 + sizeof(Int64) / sizeof(Byte);
			Int16 offset_free_space = offset4 + sizeof(Int32) / sizeof(Byte);

			RowId row_id1 = page.insert_row_value(row1);
			RowId row_id2 = page.insert_row_value(row2);
			RowId row_del_id = page.insert_row_value(row_tobe_del);
			RowId row_id3 = page.insert_row_value(row3);
			RowId row_id4 = page.insert_row_value(row4);

			page.delete_row(row_del_id);

			Assert::AreEqual({4}, page.get_row_count());
			Assert::AreEqual(offset3, page.get_row_offset(row_id3));
			Assert::AreEqual(offset_free_space, page.get_free_sapce_offset());

			//check the content of row3 and row4
			AssertRowPtr(row3, page.get_row_ptr(row_id3));
			AssertRowPtr(row4, page.get_row_ptr(row_id4));
		}

		void case8() {
			std::cout << "case 8: test insert after deletion" << std::endl;

			DataPage page(1);

			Int16 row1 = 1;
			Int32 row2 = 2;
			Int64 row3 = 3;

			RowId row_id1 = page.insert_row_value(row1);
			RowId row_id2 = page.insert_row_value(row2);
			RowId row_id3 = page.insert_row_value(row3);

			page.delete_row(row_id2);

			Int64 row4 = 4;
			RowId row_id4 = page.insert_row_value(row4);

			//小知识，删除插入不改变slot_id所对应的row
			RowPtr row_ptr3 = page.get_row_ptr(row_id3);
			AssertRowPtr(row3, row_ptr3);

			RowPtr row_ptr4 = page.get_row_ptr(row_id4);
			AssertRowPtr(row4, row_ptr4);

			//test for delete the last
			page.delete_row(row_id4);
			Int64 row5 = 5;
			RowId row_id5 = page.insert_row_value(row5);
			RowPtr row_ptr5 = page.get_row_ptr(row_id5);
			AssertRowPtr(row5, row_ptr5);
			//Assert::AreEqual(SlotInfo::EmptySlot, page.);

		}

		void case9() {
			std::cout << "case 9: test slot resue" << std::endl;
			DataPage page(1);
			Int16 row1 = 1;
			Int32 row2 = 2;
			Int64 row3 = 3;

			RowId row_id1 = page.insert_row_value(row1);
			RowId row_id2 = page.insert_row_value(row2);
			RowId row_id3 = page.insert_row_value(row3);

			page.delete_row(row_id2);

			Int64 row4 = 4;
			RowId row_id4 = page.insert_row_value(row4);
			RowPtr row_ptr4 = page.get_row_ptr(row_id4);
			AssertRowPtr(row4, row_ptr4);

			//test for slot 
			Assert::AreEqual(row_id2.slot_id, row_id4.slot_id);
		}

		void case10() {
			std::cout << "case 10: test available space" << std::endl;

			DataPage page(1);
			Int32 row1 = 1;
			RowId row_id1 = page.insert_row_value(row1);

			Assert::AreEqual<UInt16>(DataPageData::DataRegionSize - sizeof(row1) - sizeof(SlotInfo), 
				page.get_avaliable_space());

			Int64 row2 = 2;
			RowId row_id2 = page.insert_row_value(row2);
			Assert::AreEqual<UInt16>(DataPageData::DataRegionSize - sizeof(row1) - sizeof(row2) - sizeof(SlotInfo) * 2, 
				page.get_avaliable_space());
		}

		void case11() {
			std::cout << "cese11 : test insert with out data " << std::endl;
			DataPage page(1);
			Int32 row1 = 1;
			RowId row_id1 = page.insert_empty_row(sizeof(32));

			RowPtr row_ptr1 = page.get_row_ptr(row_id1);
			*((Int32*)row_ptr1.data) = row1;

			AssertRowPtr(row1, page.get_row_ptr(row_id1));

			//2019-11-21 舒服
		}

		void case12() {
			std::cout << "case 12 : test iterator " << std::endl;
			DataPage page(1);

			Int16 row1 = 1;
			Int32 row2 = 2;
			Int64 row3 = 3;
			Int32 row4 = 4;

			RowId row_id1 = page.insert_row_value(row1);
			RowId row_id2 = page.insert_row_value(row2);
			RowId row_id3 = page.insert_row_value(row3);
			RowId row_id4 = page.insert_row_value(row4);

			IEnumerator<RowPtr>::Ptr enumerator = page.get_enumerator();

			Assert::IsTrue(enumerator->move_next());
			AssertRowPtr(row1, enumerator->current());

			Assert::IsTrue(enumerator->move_next());
			AssertRowPtr(row2, enumerator->current());

			Assert::IsTrue(enumerator->move_next());
			AssertRowPtr(row3, enumerator->current());

			Assert::IsTrue(enumerator->move_next());
			AssertRowPtr(row4, enumerator->current());

			Assert::IsFalse(enumerator->move_next());
			Assert::IsFalse(enumerator->move_next());
		}

		void case13() {
			std::cout << "case 13: test empty enumerator" << std::endl;
			DataPage page(1);

			IEnumerator<RowPtr>::Ptr enumerator = page.get_enumerator();
			Assert::IsFalse(enumerator->move_next());
			Assert::IsFalse(enumerator->move_next());
		}

		void test_all_case(){
			case1();
			case2();
			case3();
			case4();
			case5();
			case6();
			case7();
			case8();
			case9();
			case10();
			case11();
			case12();
			case13();
		}


	}
}

#include "Framework\UnitTest.h"
#include "..\Engine\StorageEngine\BufferManager.h"


#include <iostream>
using namespace std;
namespace MeiDB {
	namespace BufferManagerTest {
		void case1() {
			std::cout << "\ttest for allocate page!" << std::endl;

			BufferManager buffer_manager;
			Page* page = buffer_manager.allocate_page();
			DataPage* data_page = buffer_manager.allocate_page<DataPage>();

			Assert::IsNotNull(page);
			Assert::IsNotNull(data_page);

			Assert::AreNotEqual(InvalidPageId, page->get_page_id());
			Assert::AreNotEqual(InvalidPageId, data_page->get_page_id());

			Assert::AreNotEqual(page->get_page_id(), data_page->get_page_id());

			Assert::AreEqual(page, buffer_manager.get_page(page->get_page_id()));
			Assert::AreEqual(data_page, buffer_manager.get_page_as<DataPage>(data_page->get_page_id()));
		}

		void case2()
		{
			std::cout << "\ttest for pin count!" << std::endl;

			BufferManager buffer_manager;
			Page* page = buffer_manager.allocate_page();
			DataPage* data_page = buffer_manager.allocate_page<DataPage>();

			buffer_manager.release_page(data_page->get_page_id());
			Assert::AreEqual(0, buffer_manager.get_frame(data_page->get_page_id()).pin_count);
			Assert::AreEqual(true, buffer_manager.get_frame(data_page->get_page_id()).is_dirty); 

			buffer_manager.get_frame(page->get_page_id()).is_dirty = false;
			buffer_manager.get_page(page->get_page_id());
			buffer_manager.get_page(page->get_page_id());

			Assert::AreEqual(3, buffer_manager.get_frame(page->get_page_id()).pin_count);
			buffer_manager.release_page(page->get_page_id());
			Assert::AreEqual(2, buffer_manager.get_frame(page->get_page_id()).pin_count);
			Assert::AreEqual(false, buffer_manager.get_frame(page->get_page_id()).is_dirty);
			buffer_manager.release_page(page->get_page_id(), true);
			Assert::AreEqual(1, buffer_manager.get_frame(page->get_page_id()).pin_count);
			Assert::AreEqual(true, buffer_manager.get_frame(page->get_page_id()).is_dirty);
			buffer_manager.release_page(page->get_page_id(), true);
			Assert::AreEqual(0, buffer_manager.get_frame(page->get_page_id()).pin_count);
			Assert::AreEqual(true, buffer_manager.get_frame(page->get_page_id()).is_dirty);

		}

		void case3() {

		}

		void test_all_case() {
			std::cout << "BufferManager test start ------->" << std::endl;
			case1();
			case2();
			case3();
		}
	}
}
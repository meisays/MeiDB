#pragma once

#include "..\..\Common\BasicType.h"
#include "PageFile.h"

#include <map>
#include <memory.h>
using namespace std;

namespace MeiDB {
	struct PageFrame {
		Page* page;
		int pin_count;
		bool is_dirty;
	};

	class BufferManager {
	public:
		typedef shared_ptr<BufferManager> Ptr;
		typedef std::map<PageId, PageFrame> BufferPool;

	public:
		BufferManager() 
		:next_page_id(0){
			this->page_file = nullptr;
		}

		BufferManager(const String& file_name) {
			this->page_file = make_shared<PageFile>(file_name);
			this->page_file->open();
		}

		~BufferManager() {
			if (this->page_file != nullptr) {
				this->page_file->flush();
				this->page_file->close();
			}
			
			for (BufferPool::iterator it = page_pool.begin(); it != page_pool.end(); it++) {
				delete it->second.page;
			}
			page_pool.clear();
		}

		template<typename TPage>
		TPage* allocate_page() {
			TPage* page = new TPage();
			if (page_file != nullptr) {
				// 针对某一个page_file 的buffer_pool
				page->set_page_id(page_file->get_next_page_id());
				page_file->allocate_page(page);
			}
			else {
				page->set_page_id(next_page_id);
			}

			//insert page into buffer pool
			page_pool.insert(BufferPool::value_type(page->get_page_id(), 
				PageFrame{(Page*)page, 1, true}));
			next_page_id++;
			return page;
		}

		Page* allocate_page() {
			return allocate_page<Page>();
		}

		template <typename TPage> //强制类型转换想要的类型
		inline TPage* get_page_as(const PageId& page_id) {
			return (TPage*)get_page(page_id);
		}

		Page* get_page(const PageId& page_id) {
			//return get_frame(page_id).page;
			Page* page = nullptr;

			if (page_pool.find(page_id) != page_pool.end()) {
				page = page_pool[page_id].page;
				//已经存在再 pool 之中，增加 pin
				page_pool[page_id].pin_count++;
			}
			else {
				//try to load from file
				if (this->page_file != nullptr && page_id < this->page_file->get_next_page_id()) {
					page = new Page();
					//page->set_page_id(page_id);
					page_file->read_page_to(page, page->get_page_id());
					//pin == 1 代表从磁盘之中读入初始状态
					page_pool.insert(BufferPool::value_type(page_id, {page, 1, false}));
					page = page_pool[page_id].page;
					page_pool[page_id].pin_count++;  //已经使用了一次了
				}
			}

			return page;
		}

		void release_page(const PageId& page_id, bool is_dirty = false) {
			//Page* page = get_page(page_id);
			if (page_pool.find(page_id) != page_pool.end()) {
				PageFrame& page_frame = page_pool[page_id];
				page_frame.pin_count--;
				if (is_dirty) {
					page_frame.is_dirty = true; // flush is_dirty bit to page_frame

					if (this->page_file != nullptr) {
						this->page_file->write_page(page_frame.page);
						page_frame.is_dirty = false;
					}
				}
			}
		}

		PageFrame& get_frame(const PageId& page_id) {
			//auto it = page_pool.find(page_id);
			//if (it != page_pool.end()) {
			//	return page_pool[page_id];
			//}
			//return 
			return page_pool[page_id];
		}

	private:
		PageId next_page_id;
		BufferPool page_pool;
		PageFile::Ptr page_file;
	};
}
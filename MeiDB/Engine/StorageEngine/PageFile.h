#pragma once
#include <fstream>
using namespace std;

#include <memory>

#include "Page.h"
#include "..\..\Common\BasicType.h"

namespace MeiDB {
	class PageFile {
	public:
		typedef shared_ptr<PageFile> Ptr;

		PageFile(const String& file_name)
			:file_name(file_name) {

		}

		void create_ifnot_exist() {
			FILE* f = _wfopen(file_name.c_str(), L"a");
			fclose(f);
		}

		void open() {
			//如果没有就新建一个文件
			FILE* f = _wfopen(file_name.c_str(), L"a");
			fclose(f);
			

			file = _wfopen(file_name.c_str(), L"r+b");
			next_page_id = this->size() / PageSize;
		}

		UInt32 size() {
			int file_no = _fileno(file);
			struct stat st;
			fstat(file_no, &st);
			return st.st_size;
		}

		UInt32 page_count() {
			return this->size() / PageSize;
		}

		void flush() {
			fflush(file);
		}

		void close() {
			fflush(file);
			fclose(file);
		}

		template<typename TPage>
		void allocate_page(TPage* page) {
			// 添加一个新的页面
			if (page->get_page_id() != next_page_id) {
				throw InvalidOperationException(T("Page id is not correctly"));
			}
			next_page_id++;
			fseek(file, 0, SEEK_END); //改变了内置的游标, set the position indicator to SEEK_END(file's end)
			fwrite((Byte*)page, sizeof(Byte), PageSize, file);
		}

		template<typename TPage>
		TPage* read_page_as(PageId page_id) {
			UInt32 page_size = sizeof(TPage);
			unique_ptr<Byte[]> buffer(new Byte[page_size]);

			fseek(file, page_id* page_size, SEEK_SET);
			fread(buffer.get(), sizeof(Byte), page_size, file);

			return (TPage*)buffer.release();
		}

		void read_page_to(Page* page, PageId page_id) {
			UInt32 page_size = sizeof(Page);
			fseek(file, page_id * page_size, SEEK_SET);

			fread(page, sizeof(Byte), page_size, file);
		}

		template<typename TPage>
		void write_page(TPage* page) {
			PageId page_id = page->get_page_id();

			if (page_id > this->next_page_id) {
				throw OutOfRangeException(L"too large page id");
			}

			UInt32 page_size = sizeof(TPage);
			fseek(file, page_id * page_size, SEEK_SET);
			fwrite((Byte*)page, sizeof(Byte), page_size, file);
		}

		PageId get_next_page_id() { return this->next_page_id; }

	private:
		String file_name;  //一个文件放入一个数据库，多个表，多个page
		FILE* file;
		PageId next_page_id;
	};
}
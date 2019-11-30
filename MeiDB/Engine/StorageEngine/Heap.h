#pragma once

/*
	实现替换策略
*/

#include "..\..\Common\BasicType.h"
#include "Page.h"
#include "BufferManager.h"
#include "..\Client\Client.h"
#include "DataRow.h"

#include <memory.h>
#include <vector>
using namespace std;

namespace MeiDB {
	class SimpleHeapHeaderPageData {
	public:
		static const UInt16 PageFixedDataSize = sizeof(SlotInfo) +
			sizeof(Int32) +
			sizeof(PageId) * 4 +
			sizeof(Int16) * 2 +
			sizeof(SlotId);

		static const UInt16 DataRegionSize = PageSize - PageFixedDataSize;

	public:
		SimpleHeapHeaderPageData()
			: page_id(InvalidPageId),
			slot_count(0),
			free_space_offset(0),
			max_slot_id(InvalidSoltId),
			next_page(InvalidPageId),
			first_data_page_id(InvalidPageId),
			last_data_page_id(InvalidPageId) {
			memset(this->data, 0, DataPageData::DataRegionSize);  // reset data region bits
		}

	public:
		//simple heap header page
		PageId first_data_page_id;
		PageId last_data_page_id;
		
		//DataPage layout, could read from or write to disk
		Byte data[DataRegionSize]; //data region
		SlotInfo slots[1]; // grows back to data
		Int32 slot_count; 
		Int16 free_space_offset; //the starting offset of free space 
		Int16 padding; // this is for memory alignment
		SlotId max_slot_id;
		PageId next_page;
		
		//page layout
		PageId page_id;
	};

	typedef DataPageImpl<SimpleHeapHeaderPageData> SimpleHeapHeaderPage; // heap header 不存入磁盘之中，只是在内存之中使用。

	class SimpleHeap {
	
	public:
		typedef shared_ptr<SimpleHeap> Ptr;

		SimpleHeap(BufferManager* buffer_manager) 
			: buffer(buffer_manager){
			SimpleHeapHeaderPage* header_page = buffer->allocate_page<SimpleHeapHeaderPage>();
			this->header_page_id = header_page->get_page_id();

			//first_data_page_id = last_data_page_id = current_data_page_id = new_data_page_id
			DataPage* new_data_page = allocate_new_data_page(header_page);  
			header_page->first_data_page_id = new_data_page->get_page_id();

			buffer->release_page(header_page->get_page_id(), true);
			buffer->release_page(new_data_page->get_page_id(), true);
		}

		SimpleHeap(BufferManager* buffer_manager, PageId headerPageId) 
			: buffer(buffer_manager){
			//SimpleHeapHeaderPage* header_page = buffer->allocate_page<SimpleHeapHeaderPage>();
			SimpleHeapHeaderPage* header_page = buffer->get_page_as<SimpleHeapHeaderPage>(header_page_id);
			this->header_page_id = header_page->get_page_id();
			this->current_page_id = header_page->first_data_page_id;

			buffer->release_page(header_page->get_page_id());
		}

		PageId get_header_page_id() { return this->header_page_id; }

		RowId insert_row_ptr(const RowPtr& row_ptr) {
			DataPage* current_page = buffer->get_page_as<DataPage>(current_page_id);

			if (current_page->get_avaliable_space() < row_ptr.length) {
				SimpleHeapHeaderPage* header_page = buffer->get_page_as<SimpleHeapHeaderPage>(current_page_id);
				buffer->release_page(current_page_id);

				DataPage* new_data_page = allocate_new_data_page(header_page);
				current_page = new_data_page;
				buffer->release_page(header_page->get_page_id(), true);
			}
			RowId result = current_page->insert_row_ptr(row_ptr);
			buffer->release_page(current_page->get_page_id(), true);

			return result;
		}

		RowId insert_data_row(const vector<DbValue::Ptr>& fields) {
			DataPage* current_page = buffer->get_page_as<DataPage>(current_page_id);
			auto fields_size = DataRow::CalculateRowSize(fields);
			if (fields_size > current_page->get_avaliable_space()) {
				SimpleHeapHeaderPage* header_page = 
					buffer->get_page_as<SimpleHeapHeaderPage>(header_page->get_page_id());
				buffer->release_page(current_page_id);

				current_page = allocate_new_data_page(header_page);
				buffer->release_page(header_page->get_page_id(), true);
			}
		}

		template<typename T>
		RowId insert_row_value(const T& value) {
			RowPtr row_ptr{(Byte*)(&value), sizeof(T)};
			return this->insert_row_ptr(row_ptr);
		}

		IEnumerator<RowPtr>::Ptr get_enumerator() {
			return make_shared<Enumerator>(this);
		}

	private:
		PageId header_page_id;
		PageId current_page_id;
		BufferManager* buffer;  //buffer 是heap 的实现域， 也就是为了实现buffer 我们从计算原理构建的人工数据结构
		
		DataPage* allocate_new_data_page(SimpleHeapHeaderPage* header_page) {
			DataPage* new_data_page = buffer->allocate_page<DataPage>();
			new_data_page->set_next_page_id(EndOfPage);
			header_page->last_data_page_id = new_data_page->get_page_id();
			
			this->current_page_id = new_data_page->get_page_id();

			return new_data_page;
		}

		class Enumerator : public IEnumerator<RowPtr>{
		public:
			typedef shared_ptr<Enumerator> Ptr;

			Enumerator(SimpleHeap* heap) : heap(heap){
				current_page_id = InvalidPageId;
			}

			~Enumerator() {
				if (this->current_page_id != InvalidPageId &&
					this->current_page_id != EndOfPage) {
					this->heap->buffer->release_page(this->current_page_id);
				}
			}

			virtual bool move_next() override {
				if (current_page_id == EndOfPage) {
					//已经迭代完所有页了
					return false;
				}
				if (current_page_id == InvalidPageId) {
					// 从第一页开始迭代
					SimpleHeapHeaderPage* header_page = this->heap->buffer->get_page_as<SimpleHeapHeaderPage>(this->heap->header_page_id);
					current_page_id = header_page->first_data_page_id;
					last_page_id = header_page->last_data_page_id;

					//we dont release the data page during the enumeration
					current_data_page = heap->buffer->get_page_as<DataPage>(current_page_id);
					this->current_page_enumerator = current_data_page->get_enumerator();

					this->heap->buffer->release_page(header_page->get_page_id());
				}

				bool has_next = this->current_page_enumerator->move_next();
				if (!has_next) {
					current_page_id = current_data_page->get_next_page_id();
					//about to move to next page, Release current page;
					heap->buffer->release_page(current_data_page->get_page_id());
					current_data_page = nullptr;
					current_page_enumerator = nullptr;

					if (current_page_id != EndOfPage) {
						current_data_page = heap->buffer->get_page_as<DataPage>(current_page_id);
						this->current_page_enumerator = current_data_page->get_enumerator();
						has_next = this->current_page_enumerator->move_next();
					}

				}
				return has_next;
			}

			virtual const RowPtr current() override {
				if (current_page_id == InvalidPageId) {
					throw InvalidOperationException(T("need to call move_next first"));
				}

				if(current_page_id == EndOfPage){
					throw OutOfRangeException(T("reached the end"));
				}

				return this->current_page_enumerator->current();
			}
		private:
			SimpleHeap* heap;
			IEnumerator<RowPtr>::Ptr current_page_enumerator;
			PageId current_page_id;
			DataPage* current_data_page;
			PageId last_page_id;
		};
	};
}
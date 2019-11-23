#pragma once  
//与磁盘交互的接口
#include <algorithm>
#include <memory.h>

using namespace std;

#include "..\..\Common\BasicType.h"

#include "..\Client\Client.h"

namespace MeiDB {
	typedef Int32 PageId;
	typedef Int32 SlotId;

	const UInt32 PageSize = 8192;
	const PageId InvalidPageId = -1;
	const PageId EndOfPage = -2;
	const SlotId InvalidSoltId = 1;  //sloted should be 0 or negative, 1 indicates invalid slot id
	const SlotId EndofSlot = 2;

	struct RowId {
		RowId(PageId page_id = InvalidPageId, SlotId slot_id = InvalidSoltId) :
			page_id(page_id), slot_id(slot_id) {}

		PageId page_id;
		SlotId slot_id;
	};

	struct SlotInfo {
		static const Int16 EmptySlotOffset = -1;
		static const SlotInfo EmptySlot;

		SlotInfo(int offset = SlotInfo::EmptySlot.offset, int length = SlotInfo::EmptySlot.length)
			: offset(offset), length(length) {}

		bool operator==(const SlotInfo& slot) const {
			return (this->offset == slot.offset && this->length == slot.length);
		}

		bool operator!=(const SlotInfo& slot) {
			return !this->operator==(slot);
		}

		Int16 offset; //offset within a page
		Int16 length; // length of a row
	};

	struct RowPtr {
		RowPtr(Byte* data = nullptr, Int32 length = 0)
			:data(data), length(length) {}

		Byte* data;
		Int32 length;
	};

	//layout requirement
	// - the data region starts from begining
	// - fields starts from the end
	// - The first field should be PageId
	class PageData {
	public:
		static const UInt16 PageFixedDataSize = sizeof(PageId);

		static const UInt16 DataRegionSize = PageSize - PageFixedDataSize;

		PageData()
			:page_id(InvalidPageId) {
			memset(this->data, 0, PageData::DataRegionSize);
		}

	protected:
		Byte data[DataRegionSize];
		PageId page_id;
	};

	//the layout of DataPage's data should be compatible with PageData so that
	// a DataPage can be cast to a Page

	class DataPageData {
	public:
		static const UInt16 PageFixedDataSize = sizeof(SlotInfo) +
			sizeof(Int32) +
			sizeof(PageId) * 2 +
			sizeof(Int16) * 2 +
			sizeof(SlotId);
		static const UInt16 DataRegionSize = PageSize - PageFixedDataSize;

		DataPageData()
			:page_id(InvalidPageId),
			slot_count(0),
			free_space_offset(0),
			max_slot_id(InvalidSoltId),
			next_page(InvalidPageId){
			memset(this->data, 0, DataPageData::DataRegionSize);
		}

	protected:
		//heapPage layout, 具有固定顺序
		Byte data[DataRegionSize];  //data region
		SlotInfo slots[1]; // grows backwords to data
		Int32 slot_count;
		Int16 free_space_offset; // the starting offset of free space
		Int16 padding;
		SlotId max_slot_id; // this is for memory alignment 校准
		PageId next_page;

		//This is from Page, should be at the end
		PageId page_id;
	};

	template <typename TData>
	class PageImpl : public TData {
	public:
		PageImpl(PageId page_id = InvalidPageId) {
			this->page_id = page_id;
		}

		PageId get_page_id() const {
			return this->page_id;
		}

		bool operator==(const PageImpl& page) const {
			const PageImpl* page_ptr = &page;
			//return this->page_id == page.get_page_id();
			return memcmp((Byte*)this, (Byte*)page_ptr, PageSize) == 0; //整个区域从底层进行比较
		}

		bool operator!=(const PageImpl& page) const {
			return !this->operator==(page);
		}
	};
	typedef PageImpl<PageData> Page;


	template <typename TData>
	class DataPageImpl : public PageImpl<TData> {
	public:
		DataPageImpl(PageId page_id = InvalidPageId)
			:PageImpl<TData>(page_id) {}

		Int32 get_row_count() const { return this->slot_count; }
		Int16 get_free_sapce_offset() const { return this->free_space_offset; }

		//when inserted new one, the needed space is length of data + length of slot_info
		UInt16 get_avaliable_space() const {
			return TData::DataRegionSize - this->free_space_offset - ((-this->max_slot_id + 1) * sizeof(SlotInfo));
		}

		RowId insert_empty_row(Int32 length) {
			SlotId slot_id = InvalidSoltId;

			//try to find empty slot id
			for (int i = 0; i >= this->max_slot_id; i--) {
				if (this->slots[i] == SlotInfo::EmptySlot) {
					slot_id = i;
					break;
				}
			}

			if (slot_id == InvalidSoltId) {
				this->max_slot_id--;
				slot_id = this->max_slot_id;
			}

			this->slot_count++;

			this->slots[slot_id].length = length;
			this->slots[slot_id].offset = this->free_space_offset;

			this->free_space_offset += length;

			if (slot_id < this->max_slot_id)
				this->max_slot_id = slot_id;

			return RowId(this->page_id, slot_id);
		}

		//TODO need to handle not enough free 
		RowId insert_row_ptr(const RowPtr& row_ptr) {
			RowId row_id = insert_empty_row(row_ptr.length);
			memcpy((void*)&this->data[this->slots[row_id.slot_id].offset], row_ptr.data, row_ptr.length);
			
			return row_id;
		}

		template <typename T>
		RowId insert_row_value(const T& value) {
			RowPtr ptr{ (Byte*)(&value), sizeof(T) };
			return insert_row_ptr(ptr);
		}

		RowPtr get_row_ptr(const SlotId& slot_id) {
			
			return RowPtr(&(this->data[this->slots[slot_id].offset]), this->slots[slot_id].length);
		}

		RowPtr get_row_ptr(const RowId& row_id) {
			return get_row_ptr(row_id.slot_id);
		}

		Int16 get_row_offset(const SlotId& slot_id) {
			return this->slots[slot_id].offset;
		}

		Int16 get_row_offset(const RowId& row_id) {
			return get_row_offset(row_id.slot_id);
		}

		void delete_row(const SlotId& slot_id) {
			Int16 start_offset = this->slots[slot_id].offset;
			Int16 length = this->slots[slot_id].length;
			Int16 end_offset = start_offset + length;

			//if (end_offset < this->free_space_offset)
			//	memcpy(&data[start_offset], &data[end_offset], this->free_space_offset - end_offset);

			//We need to move the data after endOffset to startOffset if there is any
			if (end_offset < this->free_space_offset)
			{
				memcpy(&(this->data[start_offset]), &(this->data[end_offset]), this->free_space_offset - end_offset);
			}

			this->slots[slot_id] = SlotInfo::EmptySlot;

			for (int i = slot_id - 1; i >= this->max_slot_id; i--) {
				if (this->slots[i] != SlotInfo::EmptySlot) {
					this->slots[i].offset -= length;
				}
			}

			//如果是删除最后一个row, 直接删除
			if (slot_id == this->max_slot_id) {
				this->max_slot_id--;
			}

			this->free_space_offset -= length;
			this->slot_count--;
		}

		void delete_row(const RowId& row_id) {
			delete_row(row_id.slot_id);
		}

		void set_next_page(PageId page) { this->next_page = page; }

		PageId get_next_page_id() { return this->next_page; }

		//每一个页面上存储的row(tuple) 的迭代器
		class Enumerator : public IEnumerator<RowPtr> {
		public:
			typedef shared_ptr<Enumerator> Ptr;

			Enumerator(DataPageImpl<TData>* page)
			:page(page), current_slot_id(InvalidSoltId){}

			virtual bool move_next() override {
				if (this->current_slot_id == EndofSlot)
					return false;

				this->current_slot_id--;
				for(; this->current_slot_id >= this->page->max_slot_id; this->current_slot_id--)
					if (this->page->slots[this->current_slot_id] != SlotInfo::EmptySlot) {
						return true;
					}

				this->current_slot_id = EndofSlot;
				return false;
			}

			virtual const RowPtr current() override {
				if (this->current_slot_id == InvalidSoltId) {
					throw InvalidOperationException(T("current called before move next"));
					//return nullptr;
				}

				if (this->current_slot_id == EndofSlot) {
					throw OutOfRangeException(T("Reached the end"));
				}
				//auto& page_slots = this->page->slots;

				return RowPtr(&(this->page->data[this->page->slots[this->current_slot_id].offset]), 
					this->page->slots[this->current_slot_id].length);
			}

		private:
			DataPageImpl<TData>* page;
			SlotId current_slot_id;
		};

		IEnumerator<RowPtr>::Ptr get_enumerator() {
			return make_shared<Enumerator>(this);
		}

		RowPtr insert_data_row(vector<DbValue::Ptr> fields) {
			auto field_length = DataRow::CalculateRowSize(fields);
			RowPtr row_ptr = this->get_row_ptr(this->insert_empty_row(field_length));
			DataRow::CopyDataRow(row_ptr.data, fields);
			return row_ptr;
		}

	};

	typedef DataPageImpl<DataPageData> DataPage;

}
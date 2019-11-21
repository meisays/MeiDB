#pragma once

#include <vector>
#include <memory>
using namespace std;

#include "Tokenizer.h"
//#include ""

namespace MeiDB {
	struct LogicalScan {
		Int32 ObjectId;
	};

	struct LogicalSelectItem {
		PString Name;
	};

	struct LogicalPlan {
	public:
		typedef shared_ptr<LogicalPlan> Ptr;

		void add_scan(const LogicalPlan& scan) {
			this->_scans.push_back(scan);
		}

		void add_select_item(const LogicalSelectItem& item) {
			this->_select_items.push_back(item);
		}


	public:
		vector<LogicalPlan> _scans;
		vector<LogicalSelectItem> _select_items;

	};
}
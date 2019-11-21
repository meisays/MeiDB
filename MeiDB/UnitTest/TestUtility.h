#pragma once

#include "Framework\UnitTest.h"

#include "..\Engine\Client\Client.h"
#include "..\Engine\StorageEngine\Page.h"
using namespace MeiDB;

#include <iostream>
#include <vector>
#include <map>
using namespace std;


template <typename T>
inline void AssertRowPtr(const T& value, const RowPtr& row_ptr, const String& msg = String()) {
	Int32 length = row_ptr.length;
	T test_value = *((T*)(row_ptr.data));

	Assert::AreEqual<int>(length, sizeof(T), msg);
	Assert::AreEqual(value, test_value, msg);
}
#include <iostream>

using namespace std;

#include "Framework/UnitTest.h"

#include "..\Engine\Client\Client.h"

namespace MeiDB {
	namespace ClientTest {
		void case1() {
			DbInt int_value(10);
			Assert::AreEqual(10, int_value.value);
			Assert::IsFalse(int_value.is_null);
		}
		void case2() {

		}

		void test_all_case() {
			case1();
			case2();
		}
	}
}
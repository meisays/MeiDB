#include <iostream>

#include "Framework/UnitTest.h"
//using namespace MeiDB::UnitTest;

int main() {
	std::cout << "-------------start--------------" << endl;
	//TestRunner::RunTests();
	//std::wcin.get();
	
	//MeiDB::ParserTest::test_all_case();
	//MeiDB::ClientTest::test_all_case();
	MeiDB::PageTest::test_all_case();

	return 0;
}
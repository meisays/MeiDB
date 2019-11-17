#include <iostream>

#include "Framework/UnitTest.h"
//using namespace MeiDB::UnitTest;

int main() {
	std::cout << "-------------start--------------" << endl;
	
	MeiDB::ParserTest::test_all_case();
	//TestRunner::RunTests();

	//std::wcin.get();

	return 0;
}
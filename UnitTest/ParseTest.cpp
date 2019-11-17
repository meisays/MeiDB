#include <iostream>
using namespace std;

#include "Framework/UnitTest.h"
using namespace MeiDB::ParserTest;

#include "..\MeiDB\Engine\QueryProcessor\Tokenizer.h"
#include "..\MeiDB\Engine\QueryProcessor\Parser.h"

//void case1() {
//	std::cout << "\t\tcase one ..." << std::endl;
//}

namespace MeiDB {
	namespace ParserTest {
		//const String out_prefix = "\t\t";

		void case1() {
			std::cout << "\t\tcase one ..." << std::endl;

			String query(T("from t1"));
			SqlToken::List tokens = Tokenizer::Parse(query);

			auto current = tokens.begin();

			auto it = tokens.end();

			//FromClause::Ptr result = FromClause::Parse(current, it);

			//Assert::IsNotNullPtr(result);

			//Assert::AreEqual(STR("t1"), *(result->TableName), T("Table name not equal"));

			//Assert::AreEqual(tokens.end(), current);
		}

		void test_all_case() {
			std::cout << "\tparst test start..." << std::endl;
			case1();
		}
	}
}
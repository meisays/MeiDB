#include <iostream>
using namespace std;

#include "Framework/UnitTest.h"
using namespace MeiDB::ParserTest;

#include "..\Engine\QueryProcessor\Tokenizer.h"
#include "..\Engine\QueryProcessor\Parser.h"

//void case1() {
//	std::cout << "\t\tcase one ..." << std::endl;
//}

namespace MeiDB {
	namespace ParserTest {
		//const String out_prefix = "\t\t";

		void case1() {
			std::cout << "\tcase no.1 ..." << std::endl;

			String query(T("from t1"));
			SqlToken::List tokens = Tokenizer::Parse(query);

			auto current = tokens.begin();

			auto it = tokens.end();

			FromClause::Ptr result = FromClause::Parse(current, it);

			Assert::IsNotNullPtr(result);

			Assert::AreEqual(STR("t1"), *(result->TableName), T("Table name not equal"));

			Assert::AreEqual(tokens.end(), current);
		}

		void case2() {
			std::cout << "\tcase no.2 ..." << std::endl;
			String query(T("from *"));
			SqlToken::List tokens = Tokenizer::Parse(query);

			auto current = tokens.begin();
			auto end = tokens.end();
			FromClause::Ptr result = FromClause::Parse(current, end);

			Assert::IsNullPtr(result);
			Assert::AreEqual(tokens.begin(), current);
		}

		void case3() {
			std::cout << "\tcase no.3 ..." << std::endl;
			String query(T("*"));
			SqlToken::List tokens = Tokenizer::Parse(query);

			auto current = tokens.begin();
			auto end = tokens.end();
			
			TargetElement::Ptr result = TargetElement::Parse(current, end);
			
			Assert::IsNotNullPtr(result);
			Assert::IsTrue(result->isStar);
			Assert::AreEqual(current, end);
		}

		void case4() {
			std::cout << "\tcase no.4 ..." << std::endl;
			//test for targetElement::parse with column name
			String query(T("c1"));
			SqlToken::List tokens = Tokenizer::Parse(query);

			auto current = tokens.begin(), end = tokens.end();
			TargetElement::Ptr result = TargetElement::Parse(current, end);


			Assert::IsNotNullPtr(result);
			Assert::AreEqual(STR("c1"), *(result->ColumnName));
			Assert::AreEqual(current, end);
		}

		void case5() {
			std::cout << "\tcase no.5 ..." << std::endl;
			//test invalid targetElement
			String query(T("from *"));
			SqlToken::List tokens = Tokenizer::Parse(query);

			auto current = tokens.begin(), end = tokens.end();
			TargetElement::Ptr result = TargetElement::Parse(current, end);

			Assert::IsNullPtr(result);
			Assert::AreEqual(tokens.begin(), current);
		}

		void case6() {
			std::cout << "\tcase no.6 ..." << std::endl;

			String query(T("c1, *, c2"));
			SqlToken::List tokens = Tokenizer::Parse(query);

			auto current = tokens.begin(), end = tokens.end();

			TargetList::Ptr result = TargetList::Parse(current, end);
			
			Assert::IsNotNullPtr(result);
			Assert::AreEqual(current, end); // all tokens can be parsed
			Assert::AreEqual<int>(3, result->Elements.size());

			Assert::AreEqual(STR("c1"), *(result->Elements[0]->ColumnName));
			Assert::IsTrue(result->Elements[1]->isStar);
			Assert::AreEqual(STR("c2"), *(result->Elements[2]->ColumnName));
		}

		void case7() {
			std::cout << "\tcase no.7 ..." << std::endl;

			String query(T("select *, c1, c2 from t1"));
			SqlToken::List tokens = Tokenizer::Parse(query);
			auto current = tokens.begin(), end = tokens.end();

			SelectStmt::Ptr result = SelectStmt::Parse(current, end);
			Assert::IsNotNullPtr(result);
			Assert::AreEqual(current, end);

			auto targetList = result->TargetList;
			Assert::IsNotNullPtr(targetList);
			Assert::AreEqual<int>(3, targetList->Elements.size());
			Assert::IsTrue(targetList->Elements[0]->isStar);
			Assert::AreEqual(STR("c1"), *(targetList->Elements[1]->ColumnName));
			Assert::AreEqual(STR("c2"), *(targetList->Elements[2]->ColumnName));

			auto fromClause = result->FromClause;
			Assert::IsNotNullPtr(fromClause);
			Assert::AreEqual(STR("t1"), *(fromClause->TableName));
		}

		void cese8() {
			std::cout << "\tcase no.8 ..." << std::endl;

			/*test select star stmt*/
			String query(T("select * from table1"));
			SqlToken::List tokens = Tokenizer::Parse(query);

			auto it = tokens.begin(), end = tokens.end();
			SelectStmt::Ptr result = SelectStmt::Parse(it, end);

			Assert::IsNotNullPtr(result);
			Assert::AreEqual(it, end);

			auto target_list = result->TargetList;
			Assert::IsNotNullPtr(target_list);
			Assert::AreEqual<int>(1, target_list->Elements.size());
			Assert::IsTrue(target_list->Elements[0]->isStar);

			auto from_clause = result->FromClause;
			Assert::IsNotNullPtr(from_clause);
			Assert::AreEqual(STR("table1"), *(from_clause->TableName));


		}

		void cese9() {
			std::cout << "\tcase no.9 ..." << std::endl;

			/*test select with where*/
			String query(T("select * from $objects where object_id = 1"));
			SqlToken::List tokens = Tokenizer::Parse(query);

			auto it = tokens.begin(), end = tokens.end();
			SelectStmt::Ptr result = SelectStmt::Parse(it, end);

			Assert::IsNotNullPtr(result);
			Assert::AreEqual(it, end);
			
			WhereClause::Ptr where_clause = result->WhereCluase;
			Assert::IsNotNullPtr(where_clause);
		}
  
		void test_all_case() {
			 std::cout << "\tparst test start..." << std::endl;
			//case1();
			//case2();
			//case3();
			//case4();
			//case5();
			//case6();
			//case7();
			cese8();
			cese9();
		}
	}
}
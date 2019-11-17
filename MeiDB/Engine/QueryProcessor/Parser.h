#pragma once

#include "Tokenizer.h"

#include <vector>
using namespace std;

namespace MeiDB{
	struct IntegerConstant {
		typedef shared_ptr<IntegerConstant> Ptr;
		int Value;
	};

	class Parser {
	public:
		static bool ConsumeToken(SqlToken::List::iterator& it, 
			SqlToken::List::iterator& end,
			SqlTokenType type);

		static PString ConsumeIdentifier(SqlToken::List::iterator& it, 
			SqlToken::List::iterator& end);

		static IntegerConstant::Ptr ConsumeIntegerConstant(SqlToken::List::iterator& it, 
			SqlToken::List::iterator& end);
	};

	struct FromClause {
		typedef shared_ptr<FromClause> Ptr;
		static FromClause::Ptr Parse(SqlToken::List::iterator& it, SqlToken::List::iterator& end);

		PString TableName;
	};

	struct TargetElement {
		typedef shared_ptr<TargetElement> Ptr;
		static TargetElement::Ptr Parse(SqlToken::List::iterator& it, SqlToken::List::iterator& end);

		bool isStar;
		PString ColumnName;
	};

	struct TargetList{
		typedef shared_ptr<TargetList> Ptr;
		static TargetList::Ptr Parse(SqlToken::List::iterator& it, SqlToken::List::iterator& end);

		vector<TargetElement::Ptr> Elements;
	};

	struct Expression
	{
		typedef shared_ptr<Expression> Ptr;
		static Expression::Ptr Parse(SqlToken::List::iterator& it, SqlToken::List::iterator& end);

		PString ColumnName;
		IntegerConstant::Ptr Value;
	};

	struct WhereClause
	{
		typedef shared_ptr<WhereClause> Ptr;
		static WhereClause::Ptr Parse(SqlToken::List::iterator& it, SqlToken::List::iterator& end);

		Expression::Ptr Expression;
	};

	struct SelectStmt
	{
		typedef shared_ptr<SelectStmt> Ptr;
		static SelectStmt::Ptr Parse(SqlToken::List::iterator& it, SqlToken::List::iterator& end);

		TargetList::Ptr TargetList;
		FromClause::Ptr FromClause;
		WhereClause::Ptr WhereCluase;
	};
}


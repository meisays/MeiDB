#include "Parser.h"

#include "..\..\Common\Convert.h"

namespace MeiDB {
	bool Parser::ConsumeToken(SqlToken::List::iterator& it, SqlToken::List::iterator& end, SqlTokenType type) {
		if (it != end && it->TokenType == type) {
			it++;
			return true;
		}
		return false;
	}

	PString Parser::ConsumeIdentifier(SqlToken::List::iterator& it, SqlToken::List::iterator& end) {
		if (it != end && it->TokenType == SqlTokenType::Identifier) {
			PString result = make_shared<String>(it->value);
			it++;
			return result;
		}
		
		return nullptr;
	}

	IntegerConstant::Ptr Parser::ConsumeIntegerConstant(SqlToken::List::iterator& it, SqlToken::List::iterator& end) {
		if (it->TokenType == SqlTokenType::Integer) {
			IntegerConstant::Ptr result = make_shared<IntegerConstant>();
			result->Value = Convert<Int32>::Parse(it->value);
			it++;
			return result;
		}

		return nullptr;
	}

	FromClause::Ptr MeiDB::FromClause::Parse(SqlToken::List::iterator& it, SqlToken::List::iterator& end) {
		SqlToken::List::iterator current = it;
		// just can parse one tablename

		if (Parser::ConsumeToken(current, end, SqlTokenType::From)) {

			PString tableName = Parser::ConsumeIdentifier(current, end);
			if (tableName != nullptr) {
				FromClause::Ptr result = make_shared<FromClause>();  //make shard 会把传入的参数用来构造一个新对象
				result->TableName = tableName;

				it = current;  // current 被cinsume 函数更新，因此需要我们把该更新同步到 it上去

				return result;
			}
		}

		return nullptr;
	}

	TargetElement::Ptr  MeiDB::TargetElement::Parse(SqlToken::List::iterator& it, 
		SqlToken::List::iterator& end) {
		SqlToken::List::iterator current = it;
		if (Parser::ConsumeToken(current, end, SqlTokenType::Star)) {
			TargetElement::Ptr result = make_shared<TargetElement>();
			result->isStar = true;

			it = current;
			return result;
		}

		PString columnName = Parser::ConsumeIdentifier(current, end);
		if (columnName != nullptr) {
			TargetElement::Ptr result = make_shared<TargetElement>();
			result->isStar = false;
			result->ColumnName = columnName;

			it = current;
			return result;
		}

		return nullptr;
	}

	TargetList::Ptr TargetList::Parse(SqlToken::List::iterator& it, SqlToken::List::iterator& end) {
		SqlToken::List::iterator current = it;
		TargetElement::Ptr element = TargetElement::Parse(current, end);

		TargetList::Ptr result = make_shared<TargetList>();
		while (element != nullptr) {
			it = current;
			result->Elements.push_back(element);
			element = nullptr;

			if (Parser::ConsumeToken(current, end, SqlTokenType::Comma)) {
				element = TargetElement::Parse(current, end);
			}
		}

		if (result->Elements.size() > 0) {
			return result;
		}

		return nullptr;
	}

	Expression::Ptr Expression::Parse(SqlToken::List::iterator& it, SqlToken::List::iterator& end) {
		SqlToken::List::iterator current = it;

		PString column_name = Parser::ConsumeIdentifier(current, end);
		if (column_name != nullptr) {
			it = current;

			if (Parser::ConsumeToken(current, end, SqlTokenType::Equal)) {
				
				IntegerConstant::Ptr intconst = Parser::ConsumeIntegerConstant(current, it);
				if (intconst != nullptr) {
					it = current;
					auto result = make_shared<Expression>();
					result->ColumnName = column_name;
					result->Value = intconst;
					return result;
				}
			}
		}

		return nullptr;
	}

	WhereClause::Ptr WhereClause::Parse(SqlToken::List::iterator& it, SqlToken::List::iterator& end) {
		SqlToken::List::iterator current = it;

		//WhereClause::Ptr clause = Parser::ConsumeToken();
		if (Parser::ConsumeToken(current, end, SqlTokenType::Where)) {
			Expression::Ptr expression = Expression::Parse(current, end);
			if (expression != nullptr) {
				auto result = make_shared<WhereClause>();
				result->Expression = expression;
				it = current;
				return result;
			}
		}

		return nullptr;
	}

	SelectStmt::Ptr SelectStmt::Parse(SqlToken::List::iterator& it, SqlToken::List::iterator& end) {
		SqlToken::List::iterator current = it;

		/*parse where keyword*/
		if (Parser::ConsumeToken(current, end, SqlTokenType::Select)) {
			TargetList::Ptr target_list = TargetList::Parse(current, end);
			
			/* parse target column*/
			if (target_list != nullptr ) {
				FromClause::Ptr from_clause = FromClause::Parse(current, end);
				if (from_clause != nullptr) {
					it = current;
					SelectStmt::Ptr result = make_shared<SelectStmt>();
					result->TargetList = target_list;
					result->FromClause = from_clause;

					//contain keyword 'where'
					WhereClause::Ptr where_clause = WhereClause::Parse(current, end);
					if (where_clause != nullptr) {
						it = current;
						result->WhereCluase = where_clause;
					}
					return result;
				}
			}
		}
		return nullptr;
	}

}
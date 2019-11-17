#pragma once

#include "..\..\Common\BasicType.h"
#include <vector>
namespace MeiDB {
	enum class SqlTokenType {
		//for reporting error
		Unknown,

		//Types
		Integer,
		Identifier,  //table name, column name

		//keywords
		Select,
		From,
		Where,

		//symbols
		Star,
		Comma,
		GreaterThan,
		LessThan,
		Equal,
		GreaterOrEqual,
		LessOrEqual,
		NotEqual,
	};

	struct SqlToken {
		typedef vector<SqlToken> List;
		SqlTokenType TokenType;
		String value;

		int Row;
		int Column;

		bool operator==(const SqlToken& token) const {
			return (this->TokenType == token.TokenType && 
				this->value == token.value);
		}
	};

	struct TokenizerException : public Exception {
		TokenizerException(const SqlToken& position, const String& msg = String())
			:Exception(msg) {
			this->Position = position;
		}

		SqlToken Position;
	};

	class Tokenizer
	{
	public:
		static vector<SqlToken> Parse(const String& query);

	public:
		//state when processing  parse a token 
		enum class State { 
			Begin,
			Identifier,
			InInteger
		};

	private:

	};

}


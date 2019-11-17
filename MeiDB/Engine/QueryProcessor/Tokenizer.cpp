#include "Tokenizer.h"

namespace MeiDB {
	vector<SqlToken> Tokenizer::Parse(const String& query) {
		vector<SqlToken> tokens; 

		const Char* reading = query.c_str();
		const Char* begin = nullptr;
		const Char* rowBegin = reading;

		int rowNumber = 1;
		State state = State::Begin;

		auto add_token = [&](int length, SqlTokenType type) {
			auto tokenBegin = begin ? begin : reading;
			String value(tokenBegin, tokenBegin + length); //construct  a new wstring

			switch (type) {
			case SqlTokenType::Identifier:
				type =
					value == T("select") ? SqlTokenType::Select :  //处理成大小写不敏感
					value == T("from") ? SqlTokenType::From :
					value == T("where") ? SqlTokenType::Where :
					SqlTokenType::Identifier;
				break;
			}

			SqlToken token;
			token.TokenType = type;
			token.Row = rowNumber;
			token.Column = tokenBegin - rowBegin + 1;
			token.value = value;

			tokens.push_back(token);
		};

		auto ReportError = [&](int length, const String& msg) {
			auto tokenBegin = begin ? begin : reading;
			String value(tokenBegin, tokenBegin + length);

			SqlToken token;
			token.TokenType = SqlTokenType::Unknown; // unkonw type
			token.Row = rowNumber;
			token.Column = tokenBegin - rowBegin + 1;
			token.value = value;
		};

		while (Char c = *reading) {
			switch (state) {
			case State::Begin:
				switch (c) {
				case T(','):  // a signal token
					add_token(1, SqlTokenType::Comma);
					break;
				case T('*'): // a signal token
					add_token(1, SqlTokenType::Star);
					break;
				case T('='): // a signal token
					add_token(1, SqlTokenType::Equal);
					break;
				case T(' '):
				case T('\t'):
				case T('\r'):
					break;

				case T('\n'):
					rowNumber++;
					rowBegin = reading + 1;
					break;
					
				case T('>'):
					switch (reading[1]) {  //不可能越界
					case T('='):
						add_token(2, SqlTokenType::GreaterOrEqual);
						reading++;
						break;
					default :
						add_token(1, SqlTokenType::GreaterThan);
					}
					break;
				case T('<'):
					switch (reading[1]) {
					case T('='):
						add_token(2, SqlTokenType::LessOrEqual);
						reading++;
						break;
					case T('>'):
						add_token(2, SqlTokenType::NotEqual);
						reading++;
						break;
					default:
						add_token(1, SqlTokenType::LessThan);
						break;
					}
				default:
					if (T('0') <= c && c <= T('9')) {
						begin = reading;
						state = State::InInteger;
					}
					else if ((T('a') <= c && c <= T('z')) || (T('A') <= c && c <= T('Z')) || c == T('_') || c == T('$')) {
						begin = reading;
						state = State::Identifier;
					}
					else {
						ReportError(1, T("unkonwn charcter: \"") + String(reading, reading + 1) + T("\"."));
					}
				}

				break; //end of begin case           
			case State::Identifier:
				if ((T('a') <= c && c <= T('z'))
					|| (T('A') <= c && c <= T('Z'))
					|| (T('0') <= c && c <= T('9'))
					|| c == T('_')
					|| c == T('.')
					|| c == T('-')) {
					//stay in State::indentifier
				}
				else {
					add_token(reading - begin, SqlTokenType::Identifier);
					state = State::Begin;
					reading--;
					begin = nullptr;
				}
				break; // end of indentifier case;

			case State::InInteger:
				if (T('0') <= c && c <= T('9')) {
					//stay in state::integer
				}
				else {
					add_token(reading - begin, SqlTokenType::Integer);
					state = State::Begin;
					reading--;
					begin = nullptr;
				}
				break; //end of integer case
			}

			reading++;
		} //end of while

		switch (state) {
		case State::InInteger:
			add_token(reading-begin, SqlTokenType::Integer);
			break;
		case State::Identifier:
			add_token(reading - begin, SqlTokenType::Identifier);
			break;
		}

		return tokens;
	}
}
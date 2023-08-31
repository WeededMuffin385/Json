export module Sandcore.Json.Parser;

import std;
import Sandcore.Json;
import Sandcore.Json.Tokenizer;

export namespace Sandcore {
	struct Parser {
		Token token;
		Tokenizer tokenizer;

		Parser(std::string string) : tokenizer(string) {}

		Json parse() {
			Json result;
			parse(result);

			return result;
		}

		void parse(Json& value) {
			token = tokenizer.get();
			switch (token.type) {
				case Token::Type::ObjectOpen: {
					parseObject(value);
					break;
				}
				case Token::Type::ArrayOpen: {
					parseArray(value);
					break;
				}
			}
		}

		void parseObject(Json& value) {
			while (token.type != Token::Type::ObjectClose) {
				token = tokenizer.get();
				std::string key = token.value;
				token = tokenizer.get(); // Should be colon
				token = tokenizer.get();
				switch (token.type) {
					case Token::Type::String:
						parseString(value[key]);
						break;
					case Token::Type::Number:
						parseNumber(value[key]);
						break;
					case Token::Type::ObjectOpen:
						parseObject(value[key]);
						break;
					case Token::Type::ArrayOpen:
						parseArray(value[key]);
						break;
				}
				token = tokenizer.get(); // should be , or }
			}
		}

		void parseArray(Json& value) {
			std::size_t i = 0;

			while (token.type != Token::Type::ArrayClose) {
				token = tokenizer.get();
				switch (token.type) {
					case Token::Type::String:
						parseString(value[i++]);
						break;
					case Token::Type::Number:
						parseNumber(value[i++]);
						break;
					case Token::Type::ObjectOpen:
						parseObject(value[i++]);
						break;
					case Token::Type::ArrayOpen:
						parseArray(value[i++]);
						break;
				}
				token = tokenizer.get(); // should be , or ]
			}
		}

		void parseString(Json& value) {
			value = token.value;
		}

		void parseNumber(Json& value) {
			value = std::stod(token.value);
		}
	};
}
export module Sandcore.Json.Tokenizer;

import std;

export namespace Sandcore {
	struct Token {
		enum class Type {
			None,
			ArrayOpen,
			ArrayClose,
			ObjectOpen,
			ObjectClose,
			String,
			Number,
			Colon,
			Comma,
		};

		std::string toString() {
			switch (type) {
				case Type::ArrayOpen:
					return "[ARRAY OPEN]";
				case Type::ArrayClose:
					return "[ARRAY CLOSE]";
				case Type::ObjectOpen:
					return "[OBJECT OPEN]";
				case Type::ObjectClose:
					return "[OBJECT CLOSE]";
				case Type::String:
					return std::format("[STRING]\"{}\"", value);
				case Type::Number:
					return std::format("[NUMBER]{}", value);
				case Type::Colon:
					return "[COLON]";
				case Type::Comma:
					return "[COMMA]";
				default:
					return "[NONE]";
			}
		}

		std::string value;
		Type type;
	};

	struct Tokenizer {
		std::stringstream stream;

		Tokenizer(std::string string) : stream(string){}

		bool empty() {
			return stream.eof();
		}

		Token get() {
			Token token;
			char c = getChar();

			switch (c) {
				case '"': {
					token.type = Token::Type::String;
					c = getChar();
					while (c != '"') {
						token.value += c;
						stream.get(c);
					}
					break;
				}
				case '.':
				case '-': 
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9': {
					token.type = Token::Type::Number;
					token.value += c;
					std::streampos prevCharPos = stream.tellg();
					c = getChar();
					while (isNumber(c)) {
						token.value += c;
						prevCharPos = stream.tellg();
						c = getChar();
					}
					stream.seekg(prevCharPos);
					break;
				}
				
				case '{':
					token.type = Token::Type::ObjectOpen;
					break;
				case '}':
					token.type = Token::Type::ObjectClose;
					break;
				case '[':
					token.type = Token::Type::ArrayOpen;
					break;
				case ']':
					token.type = Token::Type::ArrayClose;
					break;
				case ':':
					token.type = Token::Type::Colon;
					break;
				case ',':
					token.type = Token::Type::Comma;
					break;
				default:
					token.type = Token::Type::None;
					break;
			}

			return token;
		}



	private:
		bool isSpace(char c) {
			return (c != ' ') && (c != '\n') && (c != '\t');
		}

		bool isNumber(char c) {
			return (c >= '0' && c <= '9') || (c == '-') || (c == '.');
		}

		char getChar() {
			char c = ' ';
			while (!isSpace(c)) {
				if (empty()) throw std::exception("eof");
				stream.get(c);
			}
			return c;
		}
	};
}
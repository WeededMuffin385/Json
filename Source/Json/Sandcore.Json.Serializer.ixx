export module Sandcore.Json.Serializer;

import std;
import Sandcore.Json;

export namespace Sandcore {
	class Serializer {
	public:
		static std::string serialize(Json& value, bool format = false) {
			auto serializer = Serializer(format);
			serializer.serializeHelper(value);
			return serializer.result;
		}

		static std::string serialize(Json&& value, bool format = false) {
			return serialize(value, format);
		}
	private:
		Serializer(bool format = false) : format(format) {}

		std::string result;
		bool format;
		int depth = 0;

		void serializeHelper(Json::Type::Array& array) {
			auto insert = [this, &array](Json& value) {
				if (format) result += std::string(depth, '\t');
				serializeHelper(value);
			};

			if (array.empty()) return;

			auto last = --array.end();
			for (auto i = array.begin(); i != last; ++i) {
				insert(*i);

				result += ","; if (format) result += "\n";
			}

			insert(*last);
			if (format) result += "\n";
		}


		void serializeHelper(Json::Type::Object& object) {
			auto insert = [this](std::string key, Json& value) {
				if (format) result += std::string(depth, '\t');
				serializeHelper(key);
				result += ":";
				serializeHelper(value);
			};

			if (object.empty()) return;

			auto last = --object.end();
			for (auto i = object.begin(); i != last; ++i) {
				auto& [key, value] = *i;
				insert(key, value);
				result += ","; if (format) result += "\n";
			}

			auto& [key, value] = *last;
			insert(key, value);
			if (format) result += "\n";
		}

		void serializeHelper(Json::Type::String& string) {
			result += std::format("\"{}\"", string);
		}

		void serializeHelper(Json::Type::Number& number) {
			result += std::format("{}", number);
		}

		void serializeHelper(Json& value) {
			switch (value.getIdentification()) {
				case Json::Identification::Number:
					serializeHelper(value.get<Json::Type::Number>());
					break;
				case Json::Identification::String:
					serializeHelper(value.get<Json::Type::String>());
					break;
				case Json::Identification::Object:
					result += "{"; 
					if (format) formatOpen();
					serializeHelper(value.get<Json::Type::Object>());
					if (format) formatClose();
					result += "}";
					break;
				case Json::Identification::Array:
					result += "["; 
					if (format) formatOpen();
					serializeHelper(value.get<Json::Type::Array>());
					if (format) formatClose();
					result += "]";
					break;
				default:
					result += "Null";
					break;
			}
		}
		
		void formatOpen() {
			++depth;
			result += "\n";
		}

		void formatClose() {
			--depth;
			result += std::string(depth, '\t');
		}
	};
}
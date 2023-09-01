export module Sandcore.Json;
import std;

export namespace Sandcore {
	class Json {
	public:
		struct Type {
			using Array = std::unordered_map<std::size_t, Json>;
			using Object = std::unordered_map<std::string, Json>;
			using Number = double;
			using String = std::string;
		};
		enum class Identification {
			Null,
			Array,
			Object,
			Number,
			String,
		};
	private:
		template <typename T>
		static T* create(T& value) {
			return new T(value);
		}

		union Value {
			Type::Array* array;
			Type::Object* object;
			Type::Number* number;
			Type::String* string;

			Value(Identification identification) {
				switch (identification) {
					case Identification::Array:
						array = new Type::Array();
						break;
					case Identification::Object:
						object = new Type::Object();
						break;
					case Identification::Number:
						number = new Type::Number();
						break;
					case Identification::String:
						string = new Type::String();
						break;
				}
			}

			Value(const Value& value) = delete;

			Value(Type::Array& array) : array(create(array)) {}
			Value(Type::Object& object) : object(create(object)) {}
			Value(Type::Number& number) : number(create(number)) {}
			Value(Type::String& string) : string(create(string)) {}

			Value(Type::Array&& array) : Value(array) {}
			Value(Type::Object&& object) : Value(object) {}
			Value(Type::Number&& number) : Value(number) {}
			Value(Type::String&& string) : Value(string) {}
		};
	public:
		Json(Identification identification = Identification::Null) : value(identification), identification(identification) {
		}

		Json(const Json& json) : Json(json.identification) {
			switch (identification) {
				case Identification::String:
					value = *json.value.string;
					break;
				case Identification::Number:
					value = *json.value.number;
					break;
				case Identification::Object:
					value = *json.value.object;
					break;
				case Identification::Array:
					value = *json.value.array;
					break;
			}
		}

		void clean() {
			switch (identification) {
				case Identification::Array:
					delete value.array;
					break;
				case Identification::Object:
					delete value.object;
					break;
				case Identification::Number:
					delete value.number;
					break;
				case Identification::String:
					delete value.string;
					break;
			}
		}

		Json(Type::Array value) : value(value), identification(Identification::Array) {};
		Json(Type::Object value) : value(value), identification(Identification::Object) {};
		Json(Type::Number value) : value(value), identification(Identification::Number) {};
		Json(Type::String value) : value(value), identification(Identification::String) {};
		Json(const char* value) : value(value), identification(Identification::String) {};

		Identification getIdentification() { return identification; }

		template <typename T> T& get() = delete;
		template <> Type::Array& get<Type::Array>() { return *value.array; }
		template <> Type::Object& get<Type::Object>() { return *value.object; }
		template <> Type::Number& get<Type::Number>() { return *value.number; }
		template <> Type::String& get<Type::String>() { return *value.string; }

		Json& operator[](Type::String index) {
			if (identification == Identification::Null) {
				value = Type::Object();
				identification = Identification::Object;
			}
			if (identification != Identification::Object) throw std::exception("Able to use operator[] only with Object");
			return value.object->operator[](index);
		}

		Json& operator[](std::size_t index) {
			if (identification == Identification::Null) {
				value = Type::Array();
				identification = Identification::Array;
			}
			if (identification != Identification::Array) throw std::exception("Able to use operator[](size_t) only with array");
			return value.array->operator[](index);
		}

		static Json Object() {
			return Json(Identification::Object);
		}

		static Json Array() {
			return Json(Identification::Array);
		}

	private:
		Identification identification = Identification::Null;
		Value value;
	};
}
export module Sandcore.Json;

import std;

export namespace Sandcore {
	class Json {
	public:
		struct Type {
			using Array = std::vector<Json>;
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

		struct Iterator {
			Iterator(Type::Array::iterator array) : array(array), identification(Identification::Array) {}
			Iterator(Type::Object::iterator object) : object(object), identification(Identification::Object) {}

			Json& operator*() {
				switch (identification) {
					case Identification::Array:
						return array.operator*();
					case Identification::Object:
						return object.operator*().second;
				}
			}

			void operator++() {
				switch (identification) {
					case Identification::Array:
						++array;
						break;
					case Identification::Object:
						++object;
						break;
				}
			}

			bool operator!=(const Iterator& right) {
				switch (identification) {
					case Identification::Array:
						return array != right.array;
					case Identification::Object:
						return object != right.object;
				}
			}

		private:
			Json::Type::Array::iterator array;
			Json::Type::Object::iterator object;

			Json::Identification identification;
		};
	private:
		template <typename T>
		static T* create(const T& value) {
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

			Value(Value& value) = delete;
			Value(const Value& value) = delete;

			Value(const Type::Array& array) : array(create(array)) {}
			Value(const Type::Object& object) : object(create(object)) {}
			Value(const Type::Number& number) : number(create(number)) {}
			Value(const Type::String& string) : string(create(string)) {}

			Value(Type::Array&& array) : Value(array) {}
			Value(Type::Object&& object) : Value(object) {}
			Value(Type::Number&& number) : Value(number) {}
			Value(Type::String&& string) : Value(string) {}

			template <typename T> T& get() = delete;
			template <> Type::Array& get() { return *array; }
			template <> Type::Object& get() { return *object; }
			template <> Type::Number& get() { return *number; }
			template <> Type::String& get() { return *string; }

			template <typename T> const T& get() const = delete;
			template <> const Type::Array& get() const { return *array; }
			template <> const Type::Object& get() const { return *object; }
			template <> const Type::Number& get() const { return *number; }
			template <> const Type::String& get() const { return *string; }
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

		template <typename T> T& get() { return value.get<T>(); }
		template <typename T> const T& get() const { return value.get<T>(); }

		operator const Type::String() const { return get<Type::String>(); }
		operator const Type::Number() const { return get<Type::Number>(); }

		Json& operator[](Type::String index) {
			if (identification == Identification::Null) {
				value = Type::Object();
				identification = Identification::Object;
			}
			if (identification != Identification::Object) throw std::exception("Able to use operator[] only with Object");
			return value.object->operator[](index);
		}

		Json& operator[](std::size_t index) {
			if (identification != Identification::Array) throw std::exception("Able to use operator[](size_t) only with array");
			return value.array->operator[](index);
		}

		const Json& operator[](std::size_t index) const {
			if (identification != Identification::Array) throw std::exception("Able to use operator[](size_t) only with array");
			return value.array->operator[](index);
		}

		Json& at(Type::String index) {
			return value.object->at(index);
		}

		Json& at(std::size_t index) {
			return value.array->at(index);
		}

		const Json& at(Type::String index) const {
			return value.object->at(index);
		}

		const Json& at(std::size_t index) const {
			return value.array->at(index);
		}

		static Json Object() {
			return Json(Identification::Object);
		}

		static Json Array() {
			return Json(Identification::Array);
		}

		auto& emplace(std::string key, Json value) {
			return this->value.object->emplace(key, value).first->second;
		}

		auto& emplace_back(Json value) {
			return this->value.array->emplace_back(value);
		}

		bool contains(std::string key) const {
			return value.object->contains(key);
		}

		auto begin() const {
			switch (identification) {
				case Identification::Array:
					return Iterator(value.array->begin());
				case Identification::Object:
					return Iterator(value.object->begin());
			}
		}

		auto end() const {
			switch (identification) {
				case Identification::Array:
					return Iterator(value.array->end());
				case Identification::Object:
					return Iterator(value.object->end());
			}
		}

	private:
		Identification identification = Identification::Null;
		Value value;
	};
}

/*
* TODO: придумать что делать с range-based for (object версия не используется в проекте)
*/
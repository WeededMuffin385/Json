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
		union Value {
			void* pointer;

			Type::Array* array;
			Type::Object* object;
			Type::Number* number;
			Type::String* string;

			template<typename T> T& get() = delete;
			template<typename T> const T& get() const = delete;

			template<> Type::Array& get<Type::Array>() { return *array; }
			template<> Type::Object& get<Type::Object>() { return *object; }
			template<> Type::Number& get<Type::Number>() { return *number; }
			template<> Type::String& get<Type::String>() { return *string; }

			template<> const Type::Array& get<Type::Array>() const { return *array; }
			template<> const Type::Object& get<Type::Object>() const { return *object; }
			template<> const Type::Number& get<Type::Number>() const { return *number; }
			template<> const Type::String& get<Type::String>() const { return *string; }
		};

		Value value;
		Identification identification;

		void allocate(Identification identification) {
			switch (identification) {
				case Identification::Array:
					value.array = new Type::Array();
					break;
				case Identification::Object:
					value.object = new Type::Object();
					break;
				case Identification::Number:
					value.number = new Type::Number();
					break;
				case Identification::String:
					value.string = new Type::String();
					break;
			}
		}

		void clean(Identification identification) {
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

			value.pointer = nullptr;
		}

		void copy(const Json& other) {
			switch (identification) {
				case Identification::Array:
					*value.array = *other.value.array;
					break;
				case Identification::Object:
					*value.object = *other.value.object;
					break;
				case Identification::Number:
					*value.number = *other.value.number;
					break;
				case Identification::String:
					*value.string = *other.value.string;
					break;
			}
		}
	public:
		Json(Identification identification = Identification::Null) : identification(identification) { 
			allocate(identification); 
		}

		Json(const Type::Array& array) : Json(Identification::Array) { *value.array = array; }
		Json(const Type::Object& object) : Json(Identification::Object) { *value.object = object; }
		Json(const Type::Number& number) : Json(Identification::Number) { *value.number = number; }
		Json(const Type::String& string) : Json(Identification::String) { *value.string = string; }

		Json(Type::Array&& array) : Json(Identification::Array) { *value.array = std::move(array); }
		Json(Type::Object&& object) : Json(Identification::Object) { *value.object = std::move(object); }
		Json(Type::Number&& number) : Json(Identification::Number) { *value.number = std::move(number); }
		Json(Type::String&& string) : Json(Identification::String) { *value.string = std::move(string); }

		Json& operator=(const Type::Array& array) {
			if (identification != Identification::Array) {
				clean(identification);
				identification = Identification::Array;
				allocate(identification);
			}

			*value.array = array;
			return *this;
		}
		Json& operator=(const Type::Object& object) {
			if (identification != Identification::Object) {
				clean(identification);
				identification = Identification::Object;
				allocate(identification);
			}

			*value.object = object;
			return *this;
		}
		Json& operator=(const Type::Number& number) {
			if (identification != Identification::Number) {
				clean(identification);
				identification = Identification::Number;
				allocate(identification);
			}

			*value.number = number;
			return *this;
		}
		Json& operator=(const Type::String& string) {
			if (identification != Identification::String) {
				clean(identification);
				identification = Identification::String;
				allocate(identification);
			}

			*value.string = string;
			return *this;
		}

		Json& operator=(Type::Array&& array) {
			if (identification != Identification::Array) {
				clean(identification);
				identification = Identification::Array;
				allocate(identification);
			}

			*value.array = std::move(array);
			return *this;
		}
		Json& operator=(Type::Object&& object) {
			if (identification != Identification::Object) {
				clean(identification);
				identification = Identification::Object;
				allocate(identification);
			}

			*value.object = std::move(object);
			return *this;
		}
		Json& operator=(Type::Number&& number) {
			if (identification != Identification::Number) {
				clean(identification);
				identification = Identification::Number;
				allocate(identification);
			}

			*value.number = std::move(number);
			return *this;
		}
		Json& operator=(Type::String&& string) {
			if (identification != Identification::String) {
				clean(identification);
				identification = Identification::String;
				allocate(identification);
			}

			*value.string = std::move(string);
			return *this;
		}

		~Json() { 
			// std::println("Json killed: {}", (std::uint64_t)value.pointer);
			clean(identification); 
		}

		Json(const Json& other) : Json(other.identification) { copy(other); }

		Json(Json&& other) : identification(other.identification) {
			value.pointer = other.value.pointer;
			other.value.pointer = nullptr;
		}

		Json& operator=(const Json& other) {
			if (this != &other) {
				clean(identification);
				identification = other.identification;

				allocate(identification);
				copy(other);
			}

			return *this;
		}

		Json& operator=(Json&& other) {
			if (this != &other) {
				clean(identification);
				identification = other.identification;

				value.pointer = other.value.pointer;
				other.value.pointer = nullptr;
			}

			return *this;
		}

	public:
		template<typename T> struct TypeToIdentification;
		template<> struct TypeToIdentification<Type::Array> { static inline Identification identification = Identification::Array; };
		template<> struct TypeToIdentification<Type::Object> { static inline Identification identification = Identification::Object; };
		template<> struct TypeToIdentification<Type::Number> { static inline Identification identification = Identification::Number; };
		template<> struct TypeToIdentification<Type::String> { static inline Identification identification = Identification::String; };

		template<typename T> T& get() {
			if (identification != TypeToIdentification<T>::identification) throw std::exception("Wrong type!");
			return value.get<T>();
		}

		template<typename T> const T& get() const {
			if (identification != TypeToIdentification<T>::identification) throw std::exception("Wrong type!");
			return value.get<T>();
		}

	public:
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

	public:
		operator const Type::String() const { return get<Type::String>(); }
		operator const Type::Number() const { return get<Type::Number>(); }

		Json& operator[](Type::String index) {
			if (identification == Identification::Null) {
				allocate(Identification::Object);
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

		Identification getIdentification() { return identification; }
	};
}
export module Sandcore.Json;

import std;

export namespace Sandcore {
	const bool debug = false;

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

		template<typename T> struct TypeToIdentification { static inline Identification value = Identification::Null; };
		template<> struct TypeToIdentification<Type::Array> { static inline Identification value = Identification::Array; };
		template<> struct TypeToIdentification<Type::Object> { static inline Identification value = Identification::Object; };
		template<> struct TypeToIdentification<Type::Number> { static inline Identification value = Identification::Number; };
		template<> struct TypeToIdentification<Type::String> { static inline Identification value = Identification::String; };

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

		template<typename T> void allocate() = delete;
		template<> void allocate<Type::Array>() { value.array = new Type::Array(); identification = Identification::Array; }
		template<> void allocate<Type::Object>() { value.object = new Type::Object(); identification = Identification::Object; }
		template<> void allocate<Type::Number>() { value.number = new Type::Number(); identification = Identification::Number; }
		template<> void allocate<Type::String>() { value.string = new Type::String(); identification = Identification::String; }

		void allocate(Identification identification) {
			switch (identification) {
				case Identification::Array:
					allocate<Type::Array>();
					break;
				case Identification::Object:
					allocate<Type::Object>();
					break;
				case Identification::Number:
					allocate<Type::Number>();
					break;
				case Identification::String:
					allocate<Type::String>();
					break;
				default:
					this->identification = Identification::Null;
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
			value.pointer = nullptr;
		}

		template<typename T> void copy(const Json& other) {
			value.get<T>() = other.value.get<T>();
		}

		void copy(const Json& other) {
			switch (identification) {
				case Identification::Array:
					copy<Type::Array>(other);
					break;
				case Identification::Object:
					copy<Type::Object>(other);
					break;
				case Identification::Number:
					copy<Type::Number>(other);
					break;
				case Identification::String:
					copy<Type::String>(other);
					break;
			}
		}
	public:
		Json(Identification identification = Identification::Null) { 
			allocate(identification);
		}

		~Json() {
			if constexpr (debug) std::println("Json killed: {}", (std::uint64_t)value.pointer);
			clean();
		}

		template<typename T> void copyConstructorHelper(const T& other) {
			allocate<T>();
			value.get<T>() = other;
		}
		Json(const Type::Array& other) { copyConstructorHelper(other); }
		Json(const Type::Object& other) { copyConstructorHelper(other); }
		Json(const Type::Number& other) { copyConstructorHelper(other); }
		Json(const Type::String& other) { copyConstructorHelper(other); }

		template<typename T> void moveConstructorHelper(T& other) {
			allocate<T>();
			value.get<T>() = std::move(other);
		}
		Json(Type::Array&& other) { moveConstructorHelper(other); }
		Json(Type::Object&& other) { moveConstructorHelper(other); }
		Json(Type::Number&& other) { moveConstructorHelper(other); }
		Json(Type::String&& other) { moveConstructorHelper(other); }
	public:
		Json(const Json& other) : Json(other.identification) {
			copy(other);
		}
		Json(Json&& other) : identification(other.identification) {
			value.pointer = other.value.pointer;
			other.value.pointer = nullptr;
		}

		Json& operator=(const Json& other) {
			if (this != &other) {
				clean();
				allocate(identification);
				copy(other);
			}

			return *this;
		}
		Json& operator=(Json&& other) {
			if (this != &other) {
				clean();
				identification = other.identification;

				value.pointer = other.value.pointer;
				other.value.pointer = nullptr;
			}

			return *this;
		}
	public:
		template<typename T> Json& copyHelper(const T& other) {
			if (identification != TypeToIdentification<T>::value) {
				clean();
				allocate<T>();
			}

			value.get<T>() = other;
			return *this;
		}
		Json& operator=(const Type::Array& other) { return copyHelper(other); }
		Json& operator=(const Type::Object& other) { return copyHelper(other); }
		Json& operator=(const Type::Number& other) { return copyHelper(other); }
		Json& operator=(const Type::String& other) { return copyHelper(other); }

		template<typename T> Json& moveHelper(T& other) {
			if (identification != TypeToIdentification<T>::value) {
				clean();
				allocate<T>();
			}

			value.get<T>() = std::move(other);
			return *this;
		}
		Json& operator=(Type::Array&& other) { return moveHelper(other); }
		Json& operator=(Type::Object&& other) { return moveHelper(other); }
		Json& operator=(Type::Number&& other) { return moveHelper(other); }
		Json& operator=(Type::String&& other) { return moveHelper(other); }

	public:
		template<typename T> T& get() {
			if (identification != TypeToIdentification<T>::value) throw std::exception("Wrong type!");
			return value.get<T>();
		}

		template<typename T> const T& get() const {
			if (identification != TypeToIdentification<T>::value) throw std::exception("Wrong type!");
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
			if (identification == Identification::Null) allocate<Type::Object>();
			if (identification != Identification::Object) throw std::exception("Value is not an Object");
			return value.object->operator[](index);
		}

		Json& operator[](std::size_t index) {
			if (identification == Identification::Null) allocate<Type::Array>();
			if (identification != Identification::Array) throw std::exception("Value is not an Array");
			return value.array->operator[](index);
		}

		const Json& operator[](std::size_t index) const {
			if (identification != Identification::Array) throw std::exception("Value is not an Array");
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
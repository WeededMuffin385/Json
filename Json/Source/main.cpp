import std;
import Sandcore.Json;
import Sandcore.Json.Parser;
import Sandcore.Json.Tokenizer;
import Sandcore.Json.Serializer;


void example1() {
	auto data = Sandcore::Json();

	data["a"] = 0.93;
	data["b"] = "Hello world";
	data["c"]["a"] = "Another wild";
	data["c"]["b"] = 23;
	data["e"] = "Holy C";

	std::cout << Sandcore::Serializer::serialize(data, true) << "\n";
}

void example2() {
	std::string string = R"({"A": "Hello world", "B": 3.14, "C":{"A":3,"B":4}, "D":[1,12,123, "HERESJHONY"]})";
	Sandcore::Json a = Sandcore::Parser::parse(string);
	std::cout << Sandcore::Serializer::serialize(a, true);
}

int main() {
	//example1();
	example2();
}

import std;
import Sandcore.Json;
import Sandcore.Json.Parser;
import Sandcore.Json.Tokenizer;
import Sandcore.Json.Serializer;


void example1() {
	auto data = Sandcore::Json();

	data["a"] = 0.93;
	data["b"] = "Hello world";
	data["c"][0] = "Another wild";
	data["c"][1] = 23;
	data["e"] = "Holy C";

	std::cout << Sandcore::Serializer::serialize(data, true);
}

int main() {
	std::string string = R"({"A": "Hello world", "B": 3.14, "C":{"A":3,"B":4}, "D":[1,12,123, "AHSHIT"]})";
	Sandcore::Json b = Sandcore::Parser::parse(string);

	{
		//auto a = Sandcore::Parser::parse(string);
		// b = a;
	}
	std::cout << Sandcore::Serializer::serialize(b, true);
}

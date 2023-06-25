#include "Json.h"

Json::Json(std::string input) {
	this->raw = input;
}

void Json::parse(void) {
	std::string main = this->raw.substr(1, this->raw.length() - 2);
	
	// Split into sections "key":"value"
	this->vect.clear();
	std::stringstream ss(main);

	std::string buff;
	while (getline(ss, buff, ',')) {
		this->vect.push_back(buff);
	}
}

std::string Json::getValue(std::string _key) {
	// Search for matching key
	
	std::string buffer;
	std::string key;
	for (int i = 0; i < this->vect.size(); i++) {
		buffer = this->vect.at(i);
		key = buffer.substr(1, buffer.find(':') - 2);

		if (key.compare(_key) == 0) {
			return buffer.substr(buffer.find(':') + 2, (buffer.length() - buffer.find(':')) - 3);
		}
	}

	return "";
}
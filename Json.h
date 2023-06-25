#ifndef JSON_H
#define JSON_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

class Json {
	public:
		Json() {};
		Json(std::string);
		void parse(void);
		std::string getValue(std::string);

	private:
		std::string raw;
		std::vector<std::string> vect;
};

#endif

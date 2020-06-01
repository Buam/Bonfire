#pragma once
#include <string>
#include <sstream>

namespace Bonfire {
	namespace PreProcessor {
		static std::string process(const char* source) {
			// Remove all comments
			std::string result;

			std::istringstream iss(source);

			// Iterate over every line and remove everything that is behind //
			for (std::string line; std::getline(iss, line);) {
				size_t c_pos = line.find("//");
				if (c_pos != std::string::npos) {
					line.erase(c_pos, line.size() - 1);
				}
				result += line;
				result += "\n";
			}

			return result;
		}
	}
}
#pragma once
#include <string>

namespace Bonfire {
	const std::string whitespace = " \t\n\r\0";

	bool is_whitespace(char c) {
		return whitespace.find(c) != std::string::npos;
	}

	template<typename ... Args>
	std::string string_format(const char* format, Args ... args)
	{
		/*size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		if (size <= 0) { throw std::runtime_error("Error during formatting."); }
		std::unique_ptr<char[]> buf(new char[size]);
		snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside*/
		char buf[1024];
		sprintf(buf, format, args ...);

		std::string s(buf);

		return s;
	}
}
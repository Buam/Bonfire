#pragma once
#include <cstdio>

namespace Bonfire {
	namespace FileUtils {
		enum FileError {
			OK,
			NOT_FOUND,
			PERM,
			OTHER
		};

		FileError load_file(const char* path, char*& buffer, int64_t& length) {
			FILE* f;

			int err = fopen_s(&f, path, "rb");
			if (err == 0) {
				fseek(f, 0, SEEK_END);
				length = ftell(f);
				fseek(f, 0, SEEK_SET);

				// Allocate buffer (+1 because of zero character)
				buffer = new char[length + 1];
				if (buffer) {
					fread_s(buffer, length, sizeof(char), length, f);
				}
				fclose(f);
				buffer[length] = '\0';
				++length;
			}
			else {
				switch (err) {
				case 1:
					return PERM;
				case 2:
					return NOT_FOUND;
				}
				return OTHER;
			}
			return OK;
		}

		FileError write_file(const char* path, std::string data) {
			FILE* f;
			int err = fopen_s(&f, path, "wb");
			if (err == 0) {
				fprintf(f, "%s", data.c_str());
				fclose(f);
			}
			else {
				switch (err) {
				case 1:
					return PERM;
				case 2:
					return NOT_FOUND;
				}
				return OTHER;
			}
			return OK;
		}

		void change_extension(std::string& in, std::string new_ext) {
			size_t ext_pos = in.find_last_of(".");
			in.erase(ext_pos, in.size() - 1);
			in += new_ext;
		}
	}
}
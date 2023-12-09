#ifndef __FILESYSTEM_HEADER__
#define __FILESYSTEM_HEADER__

#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <fcntl.h>           /* Definition of AT_* constants */

namespace fs {
	static inline bool exists(const std::string& path) {
		return access(path.c_str(),F_OK) != -1;
	}
	static inline bool is_directory(const std::string& path) {
		return opendir(path.c_str());
	}
	static inline bool is_file(const std::string& path) {
		struct stat buffer;
		return stat(path.c_str(), &buffer) == 0;
	}
	static inline bool file_exists(const std::string& path) {
		return is_file(path) && exists(path);
	}
	static inline int file_get_contents(std::string_view path, std::string& guts,std::string& error) {
		std::ifstream include_file(path.data(),std::ios::in);

		if(!include_file.is_open()) {
			error = "couldn't find file. is_open failed";
			return -1;
		}
		if(!include_file.good()) {
			error = "file not good. good() is false";
			return -2;
		}
		struct stat statbuf;

		if(stat(path.data(), &statbuf) == -1) {
			error = "couldn't stat file.";
			return -3;
		}

		include_file.seekg(0,include_file.end);
		int length = include_file.tellg();
		guts.reserve(length);
		include_file.seekg(0,include_file.beg);
		std::vector<char> buffer;
		buffer.resize(length+1);
		std::fill(buffer.begin(),buffer.end(),0);
		include_file.read((char*)&buffer[0],length);
		include_file.close();
		guts.resize(length);
		std::copy(buffer.begin(),buffer.end(),guts.begin());
		return length;
	}
	static inline std::string current_working_directory() {
		char* cwd = ::get_current_dir_name();
		std::string path = cwd == nullptr ? "" : cwd;
		if(cwd) {
			free(cwd);
		}
		return path;
	}

	static inline void file_append(std::string_view path,const std::string& guts) {
		std::ofstream outfile;
		outfile.open(path.data(), std::ios_base::app); // append instead of overwrite
		outfile << guts;
	}

	static inline std::string sanitize_user_name(std::string_view user_name) {
		std::string cleaned = "";

		for(const auto& ch : user_name) {
			if(isalpha(ch) || isdigit(ch)) {
				cleaned += ch;
			}
		}
		return cleaned;
	}


	static inline std::string sanitize_file(std::string_view file) {
		std::string cleaned = "";

		for(const auto& ch : file) {
			if(isalpha(ch) || isdigit(ch) || ch == '.' || ch == '_') {
				cleaned += ch;
			}
		}
		return cleaned;
	}


	static inline bool has_extension(std::string_view path,std::string_view ext) {
		if(ext.length() == 0) {
			return false;
		}
		if(path.length() < ext.length()) {
			return false;
		}
		std::string extension = std::string(".") + ext.data();
		return path.substr(path.length() - extension.length()).compare(extension.c_str()) == 0;
	}
};//end namespace
  //
#endif

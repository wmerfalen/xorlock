#include <iostream>
#include <SDL2/SDL.h>
#include "time.hpp"
#include <ctime>

namespace xorlock_time {
  std::string now_string(){
		time_t now = time(0);
		tm *ltm = localtime(&now);
		std::string dateString = "", tmp = "";
		tmp = std::to_string(ltm->tm_mday);
		if(tmp.length() == 1) {
			tmp.insert(0, "0");
		}
		dateString += tmp;
		dateString += "-";
		tmp = std::to_string(1 + ltm->tm_mon);
		if(tmp.length() == 1) {
			tmp.insert(0, "0");
		}
		dateString += tmp;
		dateString += "-";
		tmp = std::to_string(1900 + ltm->tm_year);
		dateString += tmp;
		dateString += " ";
		tmp = std::to_string(ltm->tm_hour);
		if(tmp.length() == 1) {
			tmp.insert(0, "0");
		}
		dateString += tmp;
		dateString += ":";
		tmp = std::to_string(1 + ltm->tm_min);
		if(tmp.length() == 1) {
			tmp.insert(0, "0");
		}
		dateString += tmp;
		dateString += ":";
		tmp = std::to_string(1 + ltm->tm_sec);
		if(tmp.length() == 1) {
			tmp.insert(0, "0");
		}
		dateString += tmp;

		return dateString;
  }
};

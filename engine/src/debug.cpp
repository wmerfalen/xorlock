#ifndef __DEBUG_HEADER__
#define __DEBUG_HEADER__
#include <iostream>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <vector>

#ifdef LD
#undef LD
#endif

#define LD(A) std::cout << "[DEBUG]:" << __LINE__ << ": " << A << "\n";
#define DUMP_LINE() std::cout << __FILE__ << ":" << __LINE__ << "(" << __FUNCTION__ << ")\n";

#endif

#include <chrono>
#include <iomanip>

namespace clk {
	using unit_t = std::chrono::time_point<std::chrono::high_resolution_clock>;
	static unit_t m_start;
	static unit_t m_end;
	using duration_t = std::chrono::duration<double>;

	void init() {
	}
	auto now() {
		return std::chrono::high_resolution_clock::now();
	}

	auto start() {
		return m_start = now();
	}
	duration_t end() {
		return (m_end = now()) - m_start;
	}

};

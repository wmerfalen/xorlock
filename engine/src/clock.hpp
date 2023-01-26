#include <iostream>
#include <chrono>
#include <iomanip>
#include <array>
#include <vector>

namespace clk {
	using unit_t = std::chrono::time_point<std::chrono::high_resolution_clock>;
	static unit_t m_start;
	static unit_t m_end;
	using duration_t = std::chrono::duration<double>;
#ifdef RUN_BENCHMARKING
	static constexpr std::size_t SAMPLE_MAX = 32;
	static std::array<duration_t,SAMPLE_MAX> samples;
	static uint64_t ctr;
	static std::vector<double> averages;
#endif

	void init() {
#ifdef RUN_BENCHMARKING
		ctr = 0;
#endif
	}
	auto now() {
		return std::chrono::high_resolution_clock::now();
	}

	void start() {
		m_start = std::chrono::high_resolution_clock::now();
	}
#ifdef RUN_BENCHMARKING
	double avg() {
		double a = 0;
		std::size_t sz = 0;
		for(const auto& s : samples) {
			a += s.count();
			++sz;
		}
		return a / sz;
	}
	void report() {
		std::size_t i=0;
		for(const auto& a : averages) {
			std::cout << std::fixed << std::setprecision(9) << "avg[" << i << "]: " << a << "ms\n";
			++i;
		}
	}
	duration_t end() {
		m_end = std::chrono::high_resolution_clock::now();
		if(SAMPLE_MAX == ctr) {
			averages.emplace_back(avg());
			report();
			ctr = 0;
		}
		samples[ctr] = (m_end - m_start) * 1000;
		return samples[ctr++];
	}
#else
	duration_t end() {
		m_end = std::chrono::high_resolution_clock::now();
		return (m_end - m_start) * 1000;
	}
#endif

};

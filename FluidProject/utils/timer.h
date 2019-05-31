#pragma once

#include <chrono>
#include <ctime>
#include <string>
class Timer {

public:
	Timer() {
		
	}
	virtual void start() {

	}
	virtual double stop() {

	}
};

class WallTimer {
	std::chrono::time_point<std::chrono::high_resolution_clock>  t_start;
	std::chrono::time_point<std::chrono::high_resolution_clock>  t_end;

public:
	double  t_total;
	WallTimer() {
		t_total = 0;
	}
	virtual void start() {
		t_start = std::chrono::high_resolution_clock::now();
	}
	virtual double stop() {
		t_end = std::chrono::high_resolution_clock::now();
		t_total += std::chrono::duration<double, std::milli>(t_end - t_start).count();
		return std::chrono::duration<double, std::milli>(t_end - t_start).count();
	}
};

class CPUTimer {
	std::clock_t c_start;
	std::clock_t c_end; 
public:
	virtual void start() {
		c_start = std::clock();
	}
	virtual double stop() {
		c_end = std::clock();
		return 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC;
	}
};
class CPUWTimer {

};
/*
#include <Windows.h>
class CPUTimer {
	double c_start;
	double c_end;

public:
	double  t_total;
	double get_cpu_time() {
		FILETIME a, b, c, d;
		if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0) {
			//  Returns total user time.
			//  Can be tweaked to include kernel times as well.
			return
				(double)(d.dwLowDateTime |
				((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
		}
		else {
			//  Handle error
			return 0;
		}
	}
	CPUTimer() {
		t_total = 0;
	}
	virtual void start() {
		c_start = get_cpu_time();
	}
	virtual double stop() {
		c_end = get_cpu_time();
		t_total += (c_end - c_start) * 1000;
		return  (c_end - c_start) * 1000;
	}
};*/

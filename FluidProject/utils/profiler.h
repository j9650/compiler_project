#pragma once
#include "timer.h"
#include "logger.h"
#include <string>
#include <unordered_map>
#include <iostream>
#include <mutex>

class Profiler {
private:
	std::mutex mtx;
public:
	//std::unordered_map<std::string, double> profmap;
	std::unordered_map<std::string, double>		 CPUWindowsmap;
	std::unordered_map<std::string, double>		 WallTimermap;
	Profiler() {
		CPUWindowsmap["Init"] = 0;
		CPUWindowsmap["Start"] = 0;
		CPUWindowsmap["Complete"] = 0;
		CPUWindowsmap["Check"] = 0;
		CPUWindowsmap["Pause"] = 0;
		CPUWindowsmap["Terminate"] = 0;
		CPUWindowsmap["Resume"] = 0;		
		
		WallTimermap["Init"] = 0;
		WallTimermap["Start"] = 0;
		WallTimermap["Complete"] = 0;
		WallTimermap["Check"] = 0;
		WallTimermap["Pause"] = 0;
		WallTimermap["Terminate"] = 0;
		WallTimermap["Resume"] = 0;
	}

	void addWallTimeStamp(std::string& statename, double t) {
		{	std::lock_guard<std::mutex> lk(mtx);
		WallTimermap[statename] += t;
		}
	}

	void addCPUTimeStamp(std::string& statename, double t) {
		{	std::lock_guard<std::mutex> lk(mtx);
		CPUWindowsmap[statename] += t;
		}
	}

	void printTimeStamp() {
		{	std::lock_guard<std::mutex> lk(mtx);
			SyncLogger::printntnp("WallTimermap: ", " ");
			for (auto & state : WallTimermap)
				SyncLogger::printnt (state.first, ":", state.second, "\t");
			SyncLogger::print("");

			SyncLogger::printntnp("CPUWindowsmap: ", " ");
			for (auto & state : CPUWindowsmap)
				SyncLogger::printnt(state.first, ":", state.second, "\t");
			SyncLogger::print("");
		}
	}
};

class CPUProfiler {
private:
	std::mutex mtx;
public:
	int numCurrentWorkingThread;
	CPUTimer cputimer;

	WallTimer walltimer;
	std::unordered_map<int, double>		 CPUWindowsmap;
	std::unordered_map<int, double>		 WallTimermap;
	CPUProfiler() {
		numCurrentWorkingThread = 1;
		cputimer.start();
		walltimer.start();
	}

	void addThread() {
		{	
			std::lock_guard<std::mutex> lk(mtx); 
			if (CPUWindowsmap.find(numCurrentWorkingThread) == CPUWindowsmap.end()) 
				CPUWindowsmap[numCurrentWorkingThread] = 0;
			else
				CPUWindowsmap[numCurrentWorkingThread] += cputimer.stop();
			
			if (WallTimermap.find(numCurrentWorkingThread) == WallTimermap.end())
				WallTimermap[numCurrentWorkingThread] = 0;
			else
				WallTimermap[numCurrentWorkingThread] += walltimer.stop();
			cputimer.start();
			walltimer.start();
			numCurrentWorkingThread++;
		}
	}

	void endThread() {
		{
			std::lock_guard<std::mutex> lk(mtx);
			if (CPUWindowsmap.find(numCurrentWorkingThread) == CPUWindowsmap.end())
				CPUWindowsmap[numCurrentWorkingThread] = 0;
			else
				CPUWindowsmap[numCurrentWorkingThread] += cputimer.stop();

			if (WallTimermap.find(numCurrentWorkingThread) == WallTimermap.end())
				WallTimermap[numCurrentWorkingThread] = 0;
			else
				WallTimermap[numCurrentWorkingThread] += walltimer.stop();
			cputimer.start();
			walltimer.start();
			numCurrentWorkingThread--;
		}
	}

	void printTimeStamp() {
		{	std::lock_guard<std::mutex> lk(mtx);
		SyncLogger::printntnp("WallTimermap: ", " ");
		for (auto & state : WallTimermap)
			SyncLogger::printnt(state.first, ":", state.second, "\t");
		SyncLogger::print("");

		SyncLogger::printntnp("CPUWindowsmap: ", " ");
		for (auto & state : CPUWindowsmap)
			SyncLogger::printnt(state.first, ":", state.second, "\t");
		SyncLogger::print("");
		}
	}
};
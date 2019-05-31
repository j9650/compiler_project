#pragma once
//https://stackoverflow.com/questions/5028302/small-logger-class
/*
* File:   Log.h
* Author: Alberto Lepe <dev@alepe.com>
*
* Created on December 1, 2015, 6:00 PM
*/

#ifndef LOG_H
#define LOG_H

#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream, std::stringbuf
#include <mutex>
#include <fstream>
#include <cassert>

enum typelog {
	DEBUG,
	INFO,
	WARN
};


class Logger {

public:
	static std::string file;
	static std::ofstream resultfile;
	Logger() {
		//try
		//{
		//	resultfile.open(file, std::ofstream::out);
		//}
		//catch (const std::exception& nono)
		//{
		//	std::cout << nono.what() << std::endl;
		//	assert(0);
		//}
		
	}
	Logger(typelog type) {
		opened = false;
	}
	~Logger() {
		if (opened) {

			std::cout << std::endl;
		}
		resultfile.close();
		opened = false;
	}
	//template <typename S, typename... Strs>
	//void print(const S &msg, Strs ... strs) {
	//	std::stringstream ss;
	//	print_impl(ss, msg, strs...);
	//	std::cout << ss.str() << std::endl;
	//}

	template <typename S, typename... Strs>
	static void print_impl(std::stringstream & ss, const S &msg, Strs ... strs) {
		ss << msg<< " ";
		print_impl(ss, strs...);
		return;
	}
	template <typename S>
	static void print_impl(std::stringstream & ss, const S &msg) {
		ss << msg;
		return;
	}
private:
	bool opened;
	typelog msglevel = DEBUG;
	inline std::string getLabel(typelog type) {
		std::string label;
		switch (type) {
		case DEBUG: label = "DEBUG"; break;
		case INFO:  label = "INFO "; break;
		case WARN:  label = "WARN "; break;
		}
		return label;
	}
};

class ASyncLogger :public Logger {
	static std::mutex mtx;
public:
	template <typename S, typename... Strs>
	static void print(const S &msg, Strs ... strs) {
		//return;

		std::stringstream ss;
		print_impl(ss, msg, strs...);

		std::cout << ss.str() << std::endl;
		std::cout.flush();


	}
};

class SyncLogger :public Logger {
	static std::mutex mtx;
public:
	static std::string prefix;
	template <typename S, typename... Strs>
	static void printntnp(const S &msg, Strs ... strs) {
		//return;

		std::stringstream ss;
		print_impl(ss, msg, strs...);
		{
			std::lock_guard<std::mutex> lk(mtx);
			std::cout.flush();
			std::cout << prefix + ss.str();
			std::cout.flush();

			resultfile <<ss.str();
			resultfile.flush();
		}

	}

	template <typename S, typename... Strs>
	static void printnt(const S &msg, Strs ... strs) {
		//return;

		std::stringstream ss;
		print_impl(ss, msg, strs...);
		{
			std::lock_guard<std::mutex> lk(mtx);
			std::cout.flush();
			std::cout << prefix + ss.str();
			std::cout.flush();

			resultfile << prefix + ss.str() ;
			resultfile.flush();
		}

	}

	template <typename S, typename... Strs>
	static void print(const S &msg, Strs ... strs) {
		//return;
		
		std::stringstream ss;
		print_impl(ss, msg, strs...);
		{
			std::lock_guard<std::mutex> lk(mtx);
			std::cout.flush();
			std::cout << prefix + ss.str() << std::endl;
			std::cout.flush();

			resultfile << prefix +  ss.str() << std::endl;
			resultfile.flush();
		}
		
	}
};

#endif  /* LOG_H */
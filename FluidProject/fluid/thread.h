#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <cassert>
#include <thread>
#include <pthread.h>
#include <mutex>
#include "../apps/kmeans.h"
#include "../utils/logger.h"
namespace sth {
	static const int kStartThread = 80;
	static const int kSuspendThread = 81;
	static const int kResumeThread = 82;
	static const int kTerminateThread = 83;

};

class StoppableThread {
protected:
	int status;
	int suspendcount;
	std::string name;
	std::vector<StoppableThread* > parents;
	std::vector<StoppableThread* > children;
public:

	static std::mutex mtx;
	static std::random_device rd;
	static bool rdlk;
	static uint32_t random() {
		uint32_t at = 0;
		if (rdlk) {
			std::lock_guard<std::mutex> lk(mtx);
			at = rd();
		}
		else {
			at = rd();
		}

		return at;
	}

	std::thread thread;
	//123//pthread_t thread;
	std::thread::native_handle_type threadhandle;

	StoppableThread() {
		suspendcount  = 0;
	};
	inline int getState() const {
		return status;
	}
	//template<typename FunctionType, typename ... Args>
	//StoppableThread(FunctionType fun, Args ... args): suspendcount(-1), status(-1){
	//	thread = std::thread(fun, args...);
	//	threadhandle = thread.native_handle();
	//	std::cout << "Thread Running" << std::endl;
	//	status = sth::kStartThread;
	//}

	template<typename FunctionType, typename ... Args>
	void restart(FunctionType fun, Args ... args) {
		//TerminateThread(threadhandle, 0);
		thread = std::thread(fun, args...);
		//123//pthread_create(&thread, NULL, fun, args...);
		threadhandle = thread.native_handle();
	}

	//~StoppableThread() {
	//	std::cout << name << " is terminating...StoppableThread " <<std::endl;
	//}

	void detach() {
		//assert(0);
		//HANDLE duphandle = threadhandle;
		//DuplicateHandle(GetCurrentProcess(), threadhandle, GetCurrentProcess(), &duphandle, DUPLICATE_SAME_ACCESS, 1, DUPLICATE_SAME_ACCESS);
		//threadhandle = duphandle;
		thread.detach();
		//123//pthread_detach(thread);
	}



	void suspend() {
		assert(0);
		//suspendcount = SuspendThread(threadhandle);
	}
	
	void suspend(StoppableThread* t) {
		t->suspend();
	}
	void resume() {
		//ResumeThread(threadhandle);
	}
/*	std::string GetLastErrorAsString()
	{
		//Get the error message, if any.
		DWORD errorMessageID = ::GetLastError();
		if (errorMessageID == 0)
			return std::string(); //No error message has been recorded

		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

		std::string message(messageBuffer, size);

		//Free the buffer.
		LocalFree(messageBuffer);

		return message;
	}*/
	void terminate() {
		//Sleep(2);
		
		{
			//assert(0);
			if (rdlk == false) {
				rdlk = true;
				random();
			}
			std::lock_guard<std::mutex> lk(mtx);
			//auto rt = TerminateThread(threadhandle, 0);
			unsigned long long th = threadhandle;
			pthread_cancel(pthread_t(threadhandle));
			SyncLogger::print("Cancel Thread: ", name, " Canceling", " (", std::to_string(th), ")");
			rdlk = false;
/*			if (rt != 0)
				return;
			else {
				//std::cout << GetLastErrorAsString() << std::endl;
				assert(0);
			}*/
		}
	}

	void join() {
		thread.join();
		//123//pthread_join(thread, NULL);
	}
};


class DetachedThread : public StoppableThread {
private:
//	std::condition_variable cv;
public:
	template<typename FunctionType, typename ... Args>
	DetachedThread(std::string threadname, FunctionType fun, Args ... args)  {
		name = threadname;
		suspendcount = -1;

		thread = std::thread(fun, args...);
		//123//pthread_create(&thread, NULL, fun, args...);
		threadhandle = thread.native_handle();
		unsigned long long th = threadhandle;
		SyncLogger::print("Detached Thread: ", name, " Running", " (", std::to_string(th), ")");
		detach();
		status = sth::kStartThread;
	}

	//~DetachedThread() {
	//	std::cout << name << " is terminating... DetachedThread " << std::endl;
	//}
};
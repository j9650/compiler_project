#pragma once
#include "thread.h"
#include "tasklet.h"
#include "signal.h"
#include "syncds.h"
#include "valve.h"
#include "guardscheduler.h"
#include "guardstates.h"

#include "../utils/logger.h"//sync logger
#include "../utils/timer.h"//sync timer
#include <set>
#include <tuple>
#include <string>
#include <utility>
#include <functional>
#include <iostream>


class GuardScheduler;
class GuardState;
class GuardStateFactory;
class GuardTask;
//Guard is a detachable thread
//Guard's life time is determined by its producers and its consumers.

class Guard {
protected:



	std::mutex mtx;


public:
	std::condition_variable cv;
	bool started;
	bool setspecdone;
	bool specsuccess;
	WallTimer StateTimerWT;
	CPUTimer  StateTimerCT;
	WallTimer TaskTimerWT;
	CPUTimer  TaskTimerCT;
	GuardTask* task; // task contains input and output fluid data
	set_t<Guard*>* producers;// sync data structure.
	set_t<Guard*>* consumers;
	std::set<Valve*> vs;
	std::set<Valve*> endvs; // addding end valves
	std::set<Guard*> children; // adding children for end checking
	DetachedThread* worker;
	DetachedThread* scheduler;
	GuardScheduler* gs;
	GuardState* gstate;
	std::string name;
	bool terminate;
	int execmodel;//0---- exec1; 1---- exec2
	int guardId;
	Guard() {
		//assert(0);
	}
	Guard(GuardScheduler* _gs, std::string guardname, std::set<Valve*> _vs, std::set<Valve*> _endvs, GuardTask* t, std::set<Guard*> _producers) {
		
		init(_gs, guardname, _vs, _endvs, t, _producers);
		//start();
	}
	Guard(GuardScheduler* _gs, std::string guardname, std::set<Valve*> _vs, GuardTask* t, std::set<Guard*> _producers) {
		
		init(_gs, guardname, _vs, t, _producers);
		//start();
	}

	virtual void init(GuardScheduler* _gs, std::string guardname, std::set<Valve*> _vs, std::set<Valve*> _endvs, GuardTask* t, std::set<Guard*> _producers);
	virtual void init(GuardScheduler* _gs, std::string guardname, std::set<Valve*> _vs, GuardTask* t, std::set<Guard*> _producers);
	// guard starts once its producer starts.
	//
	virtual void start();

	// A guard will only finishes its execution when 
	// all its producers become <Terminate State> **and** it finishes its task execution.
	void end();

	//call by another Guard.
	void addAConsumer(Guard* consumer) {
		consumers->insert(consumer);
		//std::cout << "C " << consumer->name << " adds " << this->name << std::endl;
	}
	
	void addToProducers(std::set<Guard*> _producers) {
		for (auto producer : _producers) {
			assert(producer != this);
			producer->addAConsumer(this);
			//std::cout << "P " << producer->name << " adds " << this->name<< std::endl;
		}
	}

	
	//123//static 
	void putSignal(Guard* g, Signal sig) {
		g->sq->push(sig);
		//std::cout << "Receive signal " << g->name << " MM " << sig.msg() << std::endl;
		g->cv.notify_one();
	}

	//123//static 
	void putSignal(Guard* g, std::string _sig) {
		Signal sig(_sig);
		g->sq->push(sig);
//		std::cout << "Receive signal " << g->name << " MM " << sig.msg() << std::endl;
		g->cv.notify_one();
	}


	virtual void runGuardWorkingThread_();
	queue_t<Signal>* sq;
	virtual void runTask(); /*{
		//worker = new DetachedThread(name + "'s task ", &(Guard::_runTask), task); //123//
		worker = new DetachedThread(name + "'s task ", &(_runTask), this, task);
	}*/ //123//

	//static void _runTask(GuardTask* t);

	//123//static void runGuardWorkingThread(Guard* g) {
	//	g->runGuardWorkingThread_();
//
	//	return;
	//}
};

//void Guard::_runTask(GuardTask* t) {
//	std::cout << "before run, name: " << "name" << std::endl;
//	t->run();
//	//std::cout << "before run" << std::endl;
//	if (t->guard) putSignal(t->guard, "Completed");
//}

class GuardEx2 : public Guard {
public:
	GuardEx2(GuardScheduler* _gs, std::string guardname, std::set<Valve*> _vs, GuardTask* t, std::set<Guard*> _producers) {
		init(_gs, guardname, _vs, t, _producers);
		//start();
	}
	virtual void init(GuardScheduler* _gs, std::string guardname, std::set<Valve*> _vs, GuardTask* t, std::set<Guard*> _producers);
};

class GuardEx3 : public Guard {
public:
	GuardEx3(GuardScheduler* _gs, std::string guardname, std::set<Valve*> _vs, GuardTask* t, std::set<Guard*> _producers) {
		init(_gs, guardname, _vs, t, _producers);
		//start();
	}
	virtual void init(GuardScheduler* _gs, std::string guardname, std::set<Valve*> _vs, GuardTask* t, std::set<Guard*> _producers);
};


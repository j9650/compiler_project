#pragma once
#include "signal.h"
#include "guard.h"
#include <string>

#include "../utils/timer.h"
#include "../utils/profiler.h"
class Guard;
// Guard States Transitions are ATOMIC!
// How to enforce?

class GuardState {
public:
	virtual void handle(Guard* g, Signal sig) = 0;
};


class GuardStateExec1 : public GuardState {
public:
	virtual void handle(Guard* g, Signal sig) { assert(0); }
};

class GuardStateExec2: public GuardStateExec1 {
public:
	virtual void handle(Guard* g, Signal sig) { assert(0); }
};

class GuardStateExec3 : public GuardStateExec2 {
public:
	virtual void handle(Guard* g, Signal sig) { assert(0); }
};


////Singleton...
//template<typedef StatesType>
//class GuardStateTransit : public GuardStateSingleton {
//protected:
//	static GuardState * statehandler;
//public:
//	static GuardState* Instance() {
//		if (statehandler == nullptr) {
//			statehandler = new GuardStateTransit<StatesType>();
//		}
//		return statehandler;
//	}
//	void handle(Guard* g, GuardState* gs, Signal sig);
//};




//Singleton...
class GuardStateToInit : public GuardStateExec1 {
public:
	static GuardState * statehandler;
	static std::string statename;
	CPUTimer  cputimer;
	WallTimer		walltimer;
	static GuardState* Instance() {
		//cputimer.start();
		//walltimer.start();
		if (statehandler == nullptr) {
			statehandler = new GuardStateToInit();
		}
		return statehandler;
	}
	void handle(Guard* g, Signal sig);
};

//Singleton...
class GuardStateToStart : public GuardStateExec1 {
public:
	static GuardState * statehandler;
	static std::string statename;
	CPUTimer  cputimer;
	WallTimer		walltimer;
	static GuardState* Instance() {
		//cputimer.start();
		//walltimer.start();
		if (statehandler == nullptr) {
			statehandler = new GuardStateToStart();
		}
		return statehandler;
	}
	void handle(Guard* g, Signal sig);
};


//Singleton...
class GuardStateToComplete : public GuardStateExec1 {
public:
	static GuardState * statehandler;
	static std::string statename;
	CPUTimer  cputimer;
	WallTimer		walltimer;
	static GuardState* Instance() {
		//cputimer.start();
		//walltimer.start();
		if (statehandler == nullptr) {
			statehandler = new GuardStateToComplete();
		}
		return statehandler;
	}
	void handle(Guard* g, Signal sig);
};

class GuardStateToCheck : public GuardStateExec1 {
public:
	static GuardState * statehandler;
	static std::string statename;
	CPUTimer  cputimer;
	WallTimer		walltimer;
	static GuardState* Instance() {
		//cputimer.start();
		//walltimer.start();
		if (statehandler == nullptr) {
			statehandler = new GuardStateToCheck();
		}
		return statehandler;
	}

	void handle(Guard* g, Signal sig);
};

class GuardStateToEndcheck : public GuardStateExec1 {
public:
	static GuardState * statehandler;
	static std::string statename;
	CPUTimer  cputimer;
	WallTimer		walltimer;
	static GuardState* Instance() {
		//cputimer.start();
		//walltimer.start();
		if (statehandler == nullptr) {
			statehandler = new GuardStateToEndcheck();
		}
		return statehandler;
	}

	void handle(Guard* g, Signal sig);
};

class GuardStateToPause : public GuardStateExec2 {
public:
	static GuardState * statehandler;
	static std::string statename;
	CPUTimer  cputimer;
	WallTimer		walltimer;
	static GuardState* Instance() {
		//cputimer.start();
		//walltimer.start();
		if (statehandler == nullptr) {
			statehandler = new GuardStateToPause();
		}
		return statehandler;
	}

	void handle(Guard* g, Signal sig);

};

class GuardStateToTerminate : public GuardStateExec2 {//GuardStateSingleton...
public:
	static GuardState * statehandler;
	static std::string statename;
	CPUTimer  cputimer;
	WallTimer		walltimer;
	static GuardState* Instance() {
		//cputimer.start();
		//walltimer.start();
		if (statehandler == nullptr) {
			statehandler = new GuardStateToTerminate();
		}
		return statehandler;
	}

	void handle(Guard* g, Signal sig);
};

class GuardStateToResume : public GuardStateExec3 {//GuardStateSingleton...
public:
	static GuardState * statehandler;
	static std::string statename;
	CPUTimer  cputimer;
	WallTimer		walltimer;
	static GuardState* Instance() {
		//GlobalProfiler.addTimeStamp("Resume");
		//cputimer.start();
		//walltimer.start();
		if (statehandler == nullptr) {
			statehandler = new GuardStateToResume();
		}
		return statehandler;
	}

	void handle(Guard* g, Signal sig);
};







class GuardStateFactory {
protected:
	static GuardState * statehandler;
public:

	static GuardState* Instance() {
		if (statehandler == nullptr) {
			statehandler = GuardStateToInit::Instance();
		}
		return statehandler;
	}
};

//123// change all statehandler to non-static
//123// change all Instance to non-static
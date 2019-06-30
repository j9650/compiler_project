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
	virtual std::string get_statename() = 0;
};


class GuardStateExec1 : public GuardState {
public:
	virtual void handle(Guard* g, Signal sig) { assert(0); }
	virtual std::string get_statename() { assert(0); }
};

class GuardStateExec2: public GuardStateExec1 {
public:
	virtual void handle(Guard* g, Signal sig) { assert(0); }
	virtual std::string get_statename() { assert(0); }
};

class GuardStateExec3 : public GuardStateExec2 {
public:
	virtual void handle(Guard* g, Signal sig) { assert(0); }
	virtual std::string get_statename() { assert(0); }
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
	std::string get_statename() {
		std::string statename_ = statename;
		return statename_;
	}
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
	std::string get_statename() {
		std::string statename_ = statename;
		return statename_;
	}
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
	std::string get_statename() {
		std::string statename_ = statename;
		return statename_;
	}
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
	std::string get_statename() {
		std::string statename_ = statename;
		return statename_;
	}
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
	std::string get_statename() {
		std::string statename_ = statename;
		return statename_;
	}
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
	std::string get_statename() {
		std::string statename_ = statename;
		return statename_;
	}

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
	std::string get_statename() {
		std::string statename_ = statename;
		return statename_;
	}
};

class GuardStateToWait : public GuardStateExec1 {//GuardStateSingleton...
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
	std::string get_statename() {
		std::string statename_ = statename;
		return statename_;
	}
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
	std::string get_statename() {
		std::string statename_ = statename;
		return statename_;
	}
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
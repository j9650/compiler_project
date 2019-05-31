#pragma once
#include "guard.h"
#include "../utils/timer.h"
#include "tasklet.h"
#include <tuple>
#include <unordered_map>
#include <utility>      // std::pair
#include <thread>
#include <pthread.h>
class Guard;
class GuardTask;
class GuardScheduler {
protected:

public:
	CPUTimer  cputimer;
	WallTimer		walltimer;
	std::vector<Guard*> guardlist;
	std::vector<bool>   guarddep;
	SyncQueue<Guard*> sigqueue;
	SyncQueue<std::pair<Guard*, std::string>> sigqueuespec;
	std::condition_variable cv;
	uint32_t currGuardID;
	uint32_t aggressiveness;
	bool    dependence;
	virtual Guard* nextGuardToStart(Guard * guard) = 0;
	static void putGuard(GuardScheduler* gs, Guard* guard) {
		(gs->sigqueue).push(guard);
		//std::cout << "Receive signal " << g->name << " MM " << sig.msg() << std::endl;
		(gs->cv).notify_one();
	}

	static void putGuard(GuardScheduler* gs, Guard* guard, std::string str) {
		(gs->sigqueuespec).push(std::make_pair(guard, str));
		//std::cout << "Receive signal " << g->name << " MM " << sig.msg() << std::endl;
		(gs->cv).notify_one();
	}
};



class AggressiveGS : public GuardScheduler {
protected:
	int token;
	std::mutex mtx;
	virtual void management_(Guard* guard);


public:
	void sync(GuardTask* t);
	void synctask(GuardTask* t);
	void terminate();
	virtual void iSync();
	AggressiveGS(int _ag);
	std::mutex thmtx;


	virtual Guard* newGuard(std::string guardname, std::set<Valve*> _vs, std::set<Valve*> _endvs, GuardTask* t, std::set<Guard*> _producers);
	virtual Guard* newGuard(std::string guardname, std::set<Valve*> _vs, GuardTask* t, std::set<Guard*> _producers);

	//why static?? since no forward declearation for virual methods in VS :(
	virtual Guard* nextGuardToStart(Guard * guard);

	virtual void runGS_(AggressiveGS* gs);
	static void runGS(AggressiveGS* gs) {
		gs->runGS_(gs);
	}

};


class AggressiveGSPause : public GuardScheduler {
protected:
	int token;
	std::mutex mtx;
public:
	void sync(GuardTask* t);
	AggressiveGSPause(int _ag);
	virtual void iSync();
	virtual Guard* newGuard(std::string guardname, std::set<Valve*> _vs, GuardTask* t, std::set<Guard*> _producers);
	virtual void runGS_(AggressiveGSPause* gs);
	//why static?? since no forward declearation for virual methods in VS :(
	virtual Guard* nextGuardToStart(Guard * guard);
	static void runGS(AggressiveGSPause* gs) {
		gs->runGS_(gs);
	}

};


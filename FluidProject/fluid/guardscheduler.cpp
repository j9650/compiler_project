#include "guardscheduler.h"
#include <set>
#include <thread>
#include <pthread.h>
AggressiveGS::AggressiveGS(int _ag = 1)
{	
	token = _ag;
	aggressiveness = _ag;
	currGuardID = 0;
	dependence = false;

	DetachedThread(" ===AggressiveGS=== ", &(AggressiveGS::runGS), this);
}
void AggressiveGS::runGS_(AggressiveGS* gs) {
	while (1) {
		std::unique_lock<std::mutex> lk(gs->mtx);
		while ((gs->sigqueue).empty()) {
			gs->cv.wait(lk);
		}
		//lk.unlock();
		cputimer.start();
		walltimer.start();
		auto guard = (gs->sigqueue).pop();
		//
		//SyncLogger::print(guard->guardId, "\t", "finished");

		guard->gs->nextGuardToStart(guard);
		cputimer.stop();
		walltimer.stop();
	}
}

void AggressiveGS::management_(Guard* guard) {

}
Guard* AggressiveGS::newGuard(std::string guardname, std::set<Valve*> _vs, std::set<Valve*> _endvs, GuardTask* t, std::set<Guard*> _producers) {
	auto guard = new Guard(this, guardname, _vs, _endvs, t, _producers);
	{
		std::lock_guard<std::mutex> lk(mtx);


		//std::lock_guard<std::mutex> lk(mtx);
		guard->guardId = guardlist.size();
		guardlist.push_back(guard);
		guarddep.push_back(dependence);
		dependence = false;
		//
		//if (guardlist.size() <= aggressiveness) { 
		if (token > 0 && guarddep[currGuardID] == false) {
			guard->start();
			//123putSignal//guard->putSignal(guard, "Check");
			guard->putSignal(guard, "Check");
			SyncLogger::print("send check to ", guard->name, currGuardID, guardlist.size(), token);
			token--;
			currGuardID++;
		}
	}
	return guard;
}
Guard* AggressiveGS::newGuard(std::string guardname, std::set<Valve*> _vs, GuardTask* t, std::set<Guard*> _producers) {
	auto guard = new Guard(this, guardname, _vs, t, _producers);
	{
		std::lock_guard<std::mutex> lk(mtx);


		//std::lock_guard<std::mutex> lk(mtx);
		guard->guardId = guardlist.size();
		guardlist.push_back(guard);
		guarddep.push_back(dependence);
		dependence = false;
		//
		//if (guardlist.size() <= aggressiveness) { 
		if (token > 0 && guarddep[currGuardID] == false) {
			guard->start();
			//123putSignal//guard->putSignal(guard, "Check");
			guard->putSignal(guard, "Check");
			SyncLogger::print("send check to ", guard->name, currGuardID, guardlist.size(), token);
			token--;
			currGuardID++;
		}
	}
	return guard;
}
void AggressiveGS::iSync() {
	dependence = true;
}
void AggressiveGSPause::iSync() {
	dependence = true;
}
void AggressiveGSPause::sync(GuardTask* t) {
	std::set<Guard*> gset;
	for (auto & guard : guardlist)
		if (*(guard->task->isfinished()) == false)
			gset.insert(guard);

	while (gset.size() > 0)
	{
		for (auto it = gset.begin(); it != gset.end(); )
			if (*((*it)->task->isfinished()) == true)
				it = gset.erase(it);
			else
				++it;
	}
		; // HARD HACK!
	token = aggressiveness;
}
void AggressiveGS::sync(GuardTask* t) {
	auto newguardlist = guardlist;
	std::set<Guard*> gset;
	for (auto & guard : newguardlist)
		if (*(guard->task->isfinished()) == false)
			gset.insert(guard);

	std::cout << "before gset size! token: " << token << std::endl;
	int i=0;
	while (gset.size() > 0)
	{
		i++;
		for (auto it = gset.begin(); it != gset.end(); )
			if (*((*it)->task->isfinished()) == true)
				it = gset.erase(it);
			else {
				//if(i>1000000 && i % 1000000 ==0)
				//	std::cout << "gset size!: " << i << (*it)->name <<std::endl;
				++it;
			}
	}
	 // HARD HACK!
	std::cout << "after gset size!: " << gset.size() << std::endl;
	token = aggressiveness;
	//guardlist.clear();
}
void AggressiveGS::synctask(GuardTask* t) {
	auto task = t;

	std::cout << "before gset size! token: " << token << std::endl;
	int i=0;
	bool tt=true;
	while (tt)
	{
		i++;
		
		if (*(task->isfinished()) == true){
			tt = false;
			std::cout << "after gset size!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!: " << token << std::endl;
		}
		else {
			//////if(i>1000000 && i % 1000000 ==0)
			//////	std::cout << "gset size!: " << i << (*it)->name <<std::endl;
			//std::cout << "false!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!: " << token << std::endl;
			
		}
	}
	 // HARD HACK!
	std::cout << "after gset size!: " << token << std::endl;
	//token = aggressiveness;
	token = token + 1;
	//guardlist.clear();
}
void AggressiveGS::terminate() {
	auto newguardlist = guardlist;
	std::set<Guard*> gset;
	for (auto & guard : newguardlist)
		if (*(guard->task->isfinished()) == false)
			gset.insert(guard);

	std::cout << "before gset size! token: " << token << std::endl;
	int i=0;
	while (gset.size() > 0)
	{
		i++;
		for (auto it = gset.begin(); it != gset.end(); )
			if (*((*it)->task->isfinished()) == true)
				it = gset.erase(it);
			else {
				//////if(i>1000000 && i % 1000000 ==0)
				//////	std::cout << "gset size!: " << i << (*it)->name <<std::endl;
				++it;
			}
	}
	 // HARD HACK!
	std::cout << "after gset size!: " << gset.size() << std::endl;
	token = aggressiveness;
	//guardlist.clear();
}

AggressiveGSPause::AggressiveGSPause(int _ag)
{
	token = _ag;
	aggressiveness = _ag;
	currGuardID = 0;
	dependence = false;
	DetachedThread(" +++AggressiveGSPause+++ ", &(AggressiveGSPause::runGS), this);
}

void AggressiveGSPause::runGS_(AggressiveGSPause* gs) {
	while (1) {
		std::unique_lock<std::mutex> lk(gs->mtx);
		while ((gs->sigqueue).empty()) {
			gs->cv.wait(lk);
		}
		auto guard = (gs->sigqueue).pop();
		lk.unlock();
		cputimer.start();
		walltimer.start();
		//SyncLogger::print(guard->guardId, "\t", "finished");

		guard->gs->nextGuardToStart(guard);
		cputimer.stop();
		walltimer.stop();
	}
}

Guard* AggressiveGSPause::newGuard(std::string guardname, std::set<Valve*> _vs, GuardTask* t, std::set<Guard*> _producers) {

		auto guard = new GuardEx2(this, guardname, _vs, t, _producers);
		{std::lock_guard<std::mutex> lk(mtx);
		//
		guard->guardId = guardlist.size();
		guardlist.push_back(guard);
		guarddep.push_back(dependence);
		dependence = false;
		//
		//if (guardlist.size() <= aggressiveness) { 
		if (token > 0 && guarddep[currGuardID] == false) {
			guard->start();
			//123putSignal//guard->putSignal(guard, "Check");
			guard->putSignal(guard, "Check");
			SyncLogger::SyncLogger::print("send check to ", guard->name, currGuardID, guardlist.size(), token);
			token--;
			currGuardID++;
		}
	}
	return guard;
}


Guard* AggressiveGS::nextGuardToStart(Guard * guard) {
	Guard * nextguard = NULL;
	{
		//std::lock_guard<std::mutex> lk(mtx);
		if (guard->guardId + 1 < guarddep.size())

		SyncLogger::print("Scheduler::: XXXXXX", guard->name, guard->guardId, guarddep[guard->guardId], guard->guardId + 1, guarddep[guard->guardId + 1]);

		if (guard->guardId + 1 < guarddep.size()) guarddep[guard->guardId + 1] = false;
		token++;
		SyncLogger::print(guard->name, "nextGuardToStart", guard->guardId, guardlist.size(), token);
		if (currGuardID >= guarddep.size()) {
			nextguard = NULL;
			return nextguard;
		}
		if (guarddep[currGuardID] == false) {
			while (token > 0) {
				if (currGuardID >= guarddep.size() || guarddep[currGuardID] == true) {
					nextguard = NULL;
					break;
				}

				Signal sig_check("Check");
				auto nextGuard = guardlist[currGuardID];
				//123//if (nextGuard) { nextGuard->start(); Guard::putSignal(nextGuard, "Check"); }
				if (nextGuard) { nextGuard->start(); nextGuard->putSignal(nextGuard, "Check"); }
				if (nextGuard) { SyncLogger::print("Scheduler::: send check to ", nextGuard->name, currGuardID, guardlist.size(), token); 
					
				}
				token--;
				currGuardID++;
			}
		}
	}
	return nextguard;
}




Guard* AggressiveGSPause::nextGuardToStart(Guard * guard) {
	Guard * nextguard = NULL;
	{
		std::lock_guard<std::mutex> lk(mtx);
		if (guard->guardId + 1 < guarddep.size())
		SyncLogger::print("Scheduler::: XXXXXX", guard->name, guard->guardId, guarddep[guard->guardId], guard->guardId + 1, guarddep[guard->guardId + 1]);
		if (guard->guardId + 1 < guarddep.size()) guarddep[guard->guardId + 1] = false;
		token++;
		SyncLogger::print(guard->name, "nextGuardToStart", guard->guardId, guardlist.size(), token);
		if (currGuardID >= guarddep.size()) {
			nextguard = NULL;
			return nextguard;
		}
		if (guarddep[currGuardID] == false) {
			while (token > 0) {
				if (currGuardID >= guarddep.size() || guarddep[currGuardID] == true) {
					nextguard = NULL;
					break;
				}

				Signal sig_check("Check");
				auto nextGuard = guardlist[currGuardID];
				//123//if (nextGuard) { nextGuard->start(); Guard::putSignal(nextGuard, "Check"); }
				if (nextGuard) { nextGuard->start(); nextGuard->putSignal(nextGuard, "Check"); }
				if (nextGuard) SyncLogger::print("Scheduler::: send check to ", nextGuard->name, currGuardID, guardlist.size(), token);
				token--;
				currGuardID++;
			}
		}
	}
	return nextguard;
}
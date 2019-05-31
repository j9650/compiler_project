#include "guard.h"
// guard starts once its producer starts.
//
static void runGuardWorkingThread(Guard* g) {
	g->runGuardWorkingThread_();

	return;
}
void Guard::start() {
	{	std::lock_guard<std::mutex> lk(mtx);
	if (!started) {
		started = true;
		//(StateTimerWT).start();
		//(StateTimerCT).start();
		gstate = GuardStateFactory::Instance();
		//123//DetachedThread(name + " mainThread__ ", &(Guard::runGuardWorkingThread), this);
		
		scheduler = new DetachedThread(name + " mainThread__ ", &(runGuardWorkingThread), this);
		//DetachedThread(name + " mainThread__ ", &(runGuardWorkingThread), this);
		SyncLogger::print(name, "Start");
	} //if and only if started once, else leave with nothing.
	}   // scope of the lock.
}

void Guard::init(GuardScheduler* _gs, std::string guardname, std::set<Valve*> _vs, std::set<Valve*> _endvs, GuardTask* t, std::set<Guard*> _producers) {
	name = guardname + "::Guard";
	task = t; vs = _vs; endvs = _endvs; gs = _gs; gstate = nullptr;
	terminate=false;
	execmodel = 0;
	t->setGuard(this);
	producers = new SyncSet<Guard*>; consumers = new SyncSet<Guard*>;
	for (auto& p : _producers) producers->insert(p);
	addToProducers(_producers);
	sq = new SyncQueue<Signal>();// sq->push(Signal("Check"));
	started = false;
}

void Guard::init(GuardScheduler* _gs, std::string guardname, std::set<Valve*> _vs, GuardTask* t, std::set<Guard*> _producers) {
	name = guardname + "::Guard";
	task = t; vs = _vs; gs = _gs; gstate = nullptr;
	terminate=false;
	execmodel = 0;
	t->setGuard(this);
	producers = new SyncSet<Guard*>; consumers = new SyncSet<Guard*>;
	for (auto& p : _producers) producers->insert(p);
	addToProducers(_producers);
	sq = new SyncQueue<Signal>();// sq->push(Signal("Check"));
	started = false;
}
//123//
//void _runTask(GuardTask* t) {
//	std::cout << "before run, name: " << name << std::endl;
//	t->run();
//	//std::cout << "before run" << std::endl;
//	if (t->guard) putSignal(t->guard, "Completed");
//}
//
void _runTask(Guard* g, GuardTask* t) {
	std::cout << "before run, name: " << g->name << " pthread_self: "<< pthread_self() << std::endl;
	t->run();
	//std::cout << "after run, name: " << g->name << std::endl;
	//SyncLogger::print("after run, name: ", g->name, "\n");
	//123//if (t->guard) g->putSignal(t->guard, "Completed");

	//if (t->guard) {g->putSignal(t->guard, "Completed"); /*std::cout << "after run, name: " << g->name << std::endl;*/}
	if (t->guard) {g->putSignal(t->guard, "Endcheck"); } // the state goes to Endcheck after each time excution finished
}
void Guard::runTask() {
	//worker = new DetachedThread(name + "'s task ", &(Guard::_runTask), task); //123//
	worker = new DetachedThread(name + "'s task ", &(_runTask), this, task);
}

// A guard will only finishes its execution when 
// all its producers become <Terminate State> **and** it finishes its task execution.
void Guard::end() {

}
void Guard::runGuardWorkingThread_() {
	WallTimer GuardTimerWT;
	CPUTimer  GuardTimerCT;
	(GuardTimerWT).start();  (GuardTimerCT).start();
	std::cout << this->name << "runGuardWorkingThread_" << std::endl;
	while (1) {
		SyncLogger::print("before ", name, "\t Handling");
		if (execmodel != 2) {
			std::unique_lock<std::mutex> lk(mtx);
			while (sq->empty()) {
				cv.wait(lk);
				;;
			}
		} else{
			std::unique_lock<std::mutex> lk(mtx);
			while (sq->empty()) {
				cv.wait(lk);
				;;
			}

		}
		Signal signal = sq->pop();
		//lk.unlock();
		SyncLogger::print(name, "\t Handling", signal.msg());
		if (execmodel == 2) {
			if (signal.msg().compare("SPECSUCC") != 0) gstate->handle(this, signal);
			if (signal.msg().compare("SPECSUCC") == 0) {
				SyncLogger::print("EXEC spec GUARDFINSHED@@@@@@@@", name, "Wall: ", (GuardTimerWT).stop(), "CPU:", (GuardTimerCT).stop());
				return;
			}
		}
		
		else {
			gstate->handle(this, signal);

			//no lock is here
			if (execmodel != 2 && terminate) {

				SyncLogger::print("GUARDFINSHED@@@@@@@@", name, "Wall: ", (GuardTimerWT).stop(), "CPU:", (GuardTimerCT).stop());
				return;
			}
		}
	}
}

void GuardEx2::init(GuardScheduler* _gs, std::string guardname, std::set<Valve*> _vs, GuardTask* t, std::set<Guard*> _producers) {
    name = guardname + "::Guard";
    task = t; vs = _vs; gs = _gs; gstate = nullptr;
    terminate = false;
    execmodel = 1;
    t->setGuard(this);
    producers = new SyncSet<Guard*>; consumers = new SyncSet<Guard*>;
    for (auto& p : _producers) producers->insert(p);
    addToProducers(_producers);
    sq = new SyncQueue<Signal>();// sq->push(Signal("Check"));
    started = false;
}

void GuardEx3::init(GuardScheduler* _gs, std::string guardname, std::set<Valve*> _vs, GuardTask* t, std::set<Guard*> _producers) {
    name = guardname + "::Guard";
    task = t; vs = _vs; gs = _gs; gstate = nullptr;
    terminate = false;
    execmodel = 2;
    t->setGuard(this);
    producers = new SyncSet<Guard*>; consumers = new SyncSet<Guard*>;
    for (auto& p : _producers) producers->insert(p);
    addToProducers(_producers);
    sq = new SyncQueue<Signal>();// sq->push(Signal("Check"));
    started = false;
    setspecdone = false;
    specsuccess = false;
}
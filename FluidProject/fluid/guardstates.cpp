#include "guardstates.h"
#include "guard.h"

extern Profiler GlobalProfiler;
extern CPUProfiler GlobalCPUProfiler;
GuardState* GuardStateToInit::statehandler = nullptr;
GuardState* GuardStateToStart::statehandler = nullptr;
GuardState* GuardStateToComplete::statehandler = nullptr;
GuardState* GuardStateToCheck::statehandler = nullptr;
GuardState* GuardStateToEndcheck::statehandler = nullptr;
GuardState* GuardStateToPause::statehandler = nullptr;
GuardState* GuardStateToTerminate::statehandler = nullptr;
GuardState* GuardStateFactory::statehandler = nullptr;


std::string GuardStateToInit::statename = "Init";
std::string GuardStateToStart::statename = "Start";
std::string GuardStateToComplete::statename = "Complete";
std::string GuardStateToCheck::statename = "Check";
std::string GuardStateToEndcheck::statename = "Endcheck";
std::string GuardStateToPause::statename = "Pause";
std::string GuardStateToTerminate::statename = "Terminate";
std::string GuardStateToResume::statename = "Resume";

//CPUTimer GuardStateToInit::cputimer;
//CPUTimer GuardStateToStart::cputimer;
//CPUTimer GuardStateToComplete::cputimer;
//CPUTimer GuardStateToCheck::cputimer;
//CPUTimer GuardStateToPause::cputimer;
//CPUTimer GuardStateToTerminate::cputimer;
//CPUTimer GuardStateToResume::cputimer;
//
//WallTimer GuardStateToInit::walltimer;
//WallTimer GuardStateToStart::walltimer;
//WallTimer GuardStateToComplete::walltimer;
//WallTimer GuardStateToCheck::walltimer;
//WallTimer GuardStateToPause::walltimer;
//WallTimer GuardStateToTerminate::walltimer;
//WallTimer GuardStateToResume::walltimer;

class GuardStateActions {
public:
	//void ActionAtCompleted(Guard* g);
	//void ActionAtTerminated(Guard* g);
	//void ActionAtChecking(Guard* g);
	//void ActionAtPausing(Guard* g);

	static void ActionAtCompleted(Guard* g) {
		GlobalCPUProfiler.addThread();
		////Handle the signal...
		//auto producer_set = g->producers->getSet();

		//for (auto producer : producer_set) {
		//	Signal sig_terminate("Send to producers");
		//	std::cout << g->name << " Sending " << sig_terminate.msg() << std::endl;
		//	putSignal(producer, sig_terminate);
		//}
		Signal sig_check("Check");
		//Guard * nextGuard = g->gs->nextGuardToStart(g);

		SyncLogger::print("COMPLETE>>>>>>>>>>", g->name, "Wall: ", (g->TaskTimerWT).stop(), "CPU:", (g->TaskTimerCT).stop());
		GuardScheduler::putGuard(g->gs, g);
		GuardScheduler::putGuard(g->gs, g, "Complete");
		//if (nextGuard) Guard::putSignal(nextGuard, sig_check);
		//if (nextGuard) SyncLogger::print(g->name, "send check to ", nextGuard->name);
		
		g->terminate = true;
		
		GlobalCPUProfiler.endThread();
	}
	static void ActionAtTerminated(Guard* g) {
		//Handle the signal...
		//auto producer_set = g->producers->getSet();

		//for (auto producer : producer_set) {
		//	Signal sig_terminate("Terminate");
		//	//			std::cout << g->name << " Sending " << sig_terminate.msg() << std::endl;
		//	Guard::putSignal(producer, sig_terminate);
		//}
		GlobalCPUProfiler.addThread();
		if(!g->terminate) {
			g->terminate = true;
			g->worker->terminate();
		}
		g->task->finish = true;
		SyncLogger::print("TERMINATE+++++++", g->name, "Wall: ", (g->TaskTimerWT).stop(), "CPU:", (g->TaskTimerCT).stop());
		//Guard * nextGuard = g->gs->nextGuardToStart(g);


		GuardScheduler::putGuard(g->gs, g);
		//if (nextGuard) Guard::putSignal(nextGuard, sig_check);
		//if (nextGuard) SyncLogger::print(g->name, "send check to ", nextGuard->name);
		//Guard::putSignal(g, "Completed");
		//Signal sig_check("Check");
		GlobalCPUProfiler.endThread();
	}
	static void ActionAtSpecTerminated(Guard* g) {
		//Handle the signal...
		//auto producer_set = g->producers->getSet();

		//for (auto producer : producer_set) {
		//	Signal sig_terminate("Terminate");
		//	//			std::cout << g->name << " Sending " << sig_terminate.msg() << std::endl;
		//	Guard::putSignal(producer, sig_terminate);
		//}
		GlobalCPUProfiler.addThread();
		//g->terminate = true;
		g->worker->terminate();
		/*g->task->finish = true;*/
		SyncLogger::print("ActionAtSpecTerminated+++++++", g->name, "Wall: ", (g->TaskTimerWT).stop(), "CPU:", (g->TaskTimerCT).stop());
		//Guard * nextGuard = g->gs->nextGuardToStart(g);


		GuardScheduler::putGuard(g->gs, g, "Terminate");
		//if (nextGuard) Guard::putSignal(nextGuard, sig_check);
		//if (nextGuard) SyncLogger::print(g->name, "send check to ", nextGuard->name);
		//Guard::putSignal(g, "Completed");
		//Signal sig_check("Check");
		GlobalCPUProfiler.endThread();
	}

	static void ActionAtCheckingNoStarting(Guard* g) { //123//switch to ActionAtCheckingNoStarting
		GlobalCPUProfiler.addThread();
		//////std::cout << g->name << "static void ActionAtCheckingNoStarting" << std::endl;
		while (1) {
			bool check = true;
			std::cout << g->name << " before checking valves" << std::endl;
			while (check) {
				check = false;
				for (auto& v : g->vs) {
					if (v->check() == false)
						check = true;
				}
			}
			//for (auto& v : g->vs) {
			//	std::cout << "RRRRRRRRRRRRRRRRR  " << v->get()
			//}

			std::cout << g->name << " after checking valves" << std::endl;
			if (check == false) {
				auto producer_set = g->producers->getSet();
				Signal sig_terminate("Terminate");
				for (auto & producer : producer_set) {
					//			std::cout << g->name << " Sending " << sig_terminate.msg() << std::endl;
					//123///Guard::putSignal(producer, sig_terminate);
					producer->putSignal(producer, sig_terminate);

				}
				//g->runTask();
				//123//Guard::putSignal(g, "Start");
				g->putSignal(g, "Start");
				GlobalCPUProfiler.endThread();
				return;
			}
			//////std::cout << "Check not Pass" << std::endl;
		}
	}
	static void ActionAtCheckingAndStart(Guard* g) {
		GlobalCPUProfiler.addThread();
		while (1) {
			bool check = true;
			while (check) {
				check = false;
				for (auto& v : g->vs) {
					if (v->check() == false)
						check = true;
				}
			}

			if (check == false) {
				GuardScheduler::putGuard(g->gs, g, "Start");
				GlobalCPUProfiler.endThread();
				return;
			}
			//////std::cout << "Check not Pass" << std::endl;
		}
	}

	static void ActionAtCheckingAndTerminate(Guard* g) { //123//switch to ActionAtCheckingAndTerminate
		GlobalCPUProfiler.addThread();
		while (1) {
			bool check = true;
			while (check) {
				check = false;
				for (auto v : g->vs) {
					if (v->check() == false)
						check = true;
				}
			}

			if (check == false) {
				//pass the valves!!
				//123//Guard::putSignal(g, "Start");
				g->putSignal(g, "Start");
				GlobalCPUProfiler.endThread();
				return;
			}
			assert(0);
		}
	}

	static void ActionAtEndchecking(Guard* g) { //123//switch to ActionAtCheckingAndTerminate
		GlobalCPUProfiler.addThread();

		bool check = false;
		// first check end valves (maybe empty)
		for (auto v : g->endvs) {
			if (v->check() == false)
				check = true;
		}
		// TODO: check input data and children

		if (check == false) {
			//pass the valves!!
			//123//Guard::putSignal(g, "Start");
			g->putSignal(g, "Completed");
			GlobalCPUProfiler.endThread();
			return;
		}
		if (check == true) {
			g->putSignal(g, "Start");
			GlobalCPUProfiler.endThread();
			return;
		}
		assert(0);
		
	}

	// this is asynced call, main program will continue to execute.
	static void ActionAtStart(Guard* g) {
		GlobalCPUProfiler.addThread();
		SyncLogger::prefix = "Guard-" + std::to_string(g->guardId) + " ";
		std::cout << g->name << "static void ActionAtStart" << std::endl;
		g->runTask(); 
		//SyncLogger::print("TASKFINISHED", g->name, "Wall: ", (g->TaskTimerWT).stop(), "CPU:", (g->TaskTimerCT).stop());

		GlobalCPUProfiler.endThread();

	}

	static void ActionAtPausing(Guard* g) {
		if (g->worker) g->worker->suspend();
	}

};


//Singleton...
//template<typedef StatesType>
//void GuardStateTransit<StatesType>::handle(Guard* g, GuardState* gs, Signal sig) {
//	if (sig.msg().compare("Check") == 0) {
//		GuardStateActions::ActionAtChecking(g);
//		return GuardStateTransit<StateCheck>::Instance();
//	}
//	else if (sig.msg().compare("Check") == 0) {
//	
//	}
//}

void GuardStateToInit::handle(Guard* g, Signal sig) {
	//GlobalProfiler.addWallTimeStamp(statename, (g->StateTimerWT).stop());
	//GlobalProfiler.addCPUTimeStamp(statename, (g->StateTimerCT).stop());
	std::cout << g->name << " GuardStateToInit::handle: " << sig.msg() << std::endl;
	cputimer.start();
	walltimer.start();
	(g->StateTimerWT).start();
	(g->StateTimerCT).start();
	if (sig.msg().compare("Check") == 0 && g->execmodel == 0) {
		g->gstate = GuardStateToCheck::Instance();

		GuardStateActions::ActionAtCheckingNoStarting(g);
	} 
	else if (sig.msg().compare("Check") == 0 && g->execmodel == 1) {
		g->gstate = GuardStateToCheck::Instance();

		GuardStateActions::ActionAtCheckingAndTerminate(g);
	}
	else if (sig.msg().compare("Start") == 0 && g->execmodel == 2) {
		g->gstate = GuardStateToStart::Instance();
		GuardStateActions::ActionAtStart(g);

	}
	else if (sig.msg().compare("Terminate") == 0 && g->execmodel == 2) {
		//g->gstate = GuardStateToInit::Instance();
		//GuardStateActions::ActionAtSpecTerminated(g);
		assert(0);
	}
	else if (sig.msg().compare("Completed") == 0 && g->execmodel == 2) {
		//g->gstate = GuardStateToInit::Instance();
		//GuardStateActions::ActionAtSpecTerminated(g);
		assert(0);
	}
	else if (sig.msg().compare("Terminate") == 0) {
		g->gstate = GuardStateToTerminate::Instance();
		GuardStateActions::ActionAtTerminated(g);
	}
	else {
		assert(0); // not valid!
	}
	GlobalProfiler.addWallTimeStamp(statename, (g->StateTimerWT).stop());
	GlobalProfiler.addCPUTimeStamp(statename, (g->StateTimerCT).stop());
	return;
}

void GuardStateToCheck::handle(Guard* g, Signal sig) {
	std::cout << g->name << " GuardStateToCheck::handle: " << sig.msg() << std::endl;
	(g->StateTimerWT).start();
	(g->StateTimerCT).start();
	if (sig.msg().compare("Check") == 0 && g->execmodel == 0) {
		assert(0);
		g->gstate = GuardStateToCheck::Instance();

		GuardStateActions::ActionAtCheckingNoStarting(g);
	}
	else if (sig.msg().compare("Check") == 0 && g->execmodel == 1) {
		assert(0);
		g->gstate = GuardStateToCheck::Instance();

		GuardStateActions::ActionAtCheckingAndTerminate(g);
	}
	else if (sig.msg().compare("Start") == 0) {
		g->gstate = GuardStateToStart::Instance();
		GuardStateActions::ActionAtStart(g);

	}
	else if (sig.msg().compare("Terminate") == 0 && g->execmodel == 2) {
		g->gstate = GuardStateToInit::Instance();
		GuardStateActions::ActionAtSpecTerminated(g);
	}
	else if (sig.msg().compare("Terminate") == 0) {
		assert(0);
		g->gstate = GuardStateToTerminate::Instance();
		GuardStateActions::ActionAtTerminated(g);
	}
	else if (sig.msg().compare("Completed") == 0 && g->execmodel == 2) {
		(g->StateTimerWT).start();
		(g->StateTimerCT).start();
		g->gstate = GuardStateToComplete::Instance();
		GuardStateActions::ActionAtCompleted(g);
		GlobalProfiler.addWallTimeStamp(statename, (g->StateTimerWT).stop());
		GlobalProfiler.addCPUTimeStamp(statename, (g->StateTimerCT).stop());
	}
	else {
		assert(0); // not valid!
	}
	GlobalProfiler.addWallTimeStamp(statename, (g->StateTimerWT).stop());
	GlobalProfiler.addCPUTimeStamp(statename, (g->StateTimerCT).stop());
	return;
}

void GuardStateToStart::handle(Guard* g, Signal sig) {

	std::cout << g->name << " GuardStateToStart::handle: " << sig.msg() << std::endl;
	if (sig.msg().compare("Check") == 0 && g->execmodel == 0) {
		assert(0);
		g->gstate = GuardStateToCheck::Instance();

		GuardStateActions::ActionAtCheckingNoStarting(g);
	}
	else if (sig.msg().compare("Check") == 0 && g->execmodel == 1) {
		assert(0);
		g->gstate = GuardStateToCheck::Instance();

		GuardStateActions::ActionAtCheckingAndTerminate(g);
	}
	else if (sig.msg().compare("Check") == 0 && g->execmodel == 2) {
		g->gstate = GuardStateToCheck::Instance();
		GuardStateActions::ActionAtCheckingAndStart(g);
	}
	else if (sig.msg().compare("Completed") == 0) {
		(g->StateTimerWT).start();
		(g->StateTimerCT).start();
		g->gstate = GuardStateToComplete::Instance();
		GuardStateActions::ActionAtCompleted(g);
		GlobalProfiler.addWallTimeStamp(statename, (g->StateTimerWT).stop());
		GlobalProfiler.addCPUTimeStamp(statename, (g->StateTimerCT).stop());
	}
	else if (sig.msg().compare("Terminate") == 0 && g->execmodel == 2) {
		g->gstate = GuardStateToInit::Instance();
		GuardStateActions::ActionAtSpecTerminated(g);
	}
	else if (sig.msg().compare("Terminate") == 0) {
		g->gstate = GuardStateToTerminate::Instance();
		GuardStateActions::ActionAtTerminated(g);
	}
	else if (sig.msg().compare("Endcheck") == 0) {
		//(g->StateTimerWT).start();
		//(g->StateTimerCT).start();
		g->gstate = GuardStateToEndcheck::Instance();
		GuardStateActions::ActionAtEndchecking(g);
		//GlobalProfiler.addWallTimeStamp(statename, (g->StateTimerWT).stop());
		//GlobalProfiler.addCPUTimeStamp(statename, (g->StateTimerCT).stop());
	}
	else {
		assert(0); // not valid!
	}

	return;
}

void GuardStateToEndcheck::handle(Guard* g, Signal sig) {
	std::cout << g->name << " GuardStateToCheck::handle: " << sig.msg() << std::endl;
	(g->StateTimerWT).start();
	(g->StateTimerCT).start();
	if (sig.msg().compare("Check") == 0 && g->execmodel == 0) {
		assert(0);
		g->gstate = GuardStateToCheck::Instance();

		GuardStateActions::ActionAtCheckingNoStarting(g);
	}
	else if (sig.msg().compare("Check") == 0 && g->execmodel == 1) {
		assert(0);
		g->gstate = GuardStateToCheck::Instance();

		GuardStateActions::ActionAtCheckingAndTerminate(g);
	}
	else if (sig.msg().compare("Start") == 0) {
		g->gstate = GuardStateToStart::Instance();
		GuardStateActions::ActionAtStart(g);

	}
	else if (sig.msg().compare("Completed") == 0) {
		(g->StateTimerWT).start();
		(g->StateTimerCT).start();
		g->gstate = GuardStateToComplete::Instance();
		GuardStateActions::ActionAtCompleted(g);
		GlobalProfiler.addWallTimeStamp(statename, (g->StateTimerWT).stop());
		GlobalProfiler.addCPUTimeStamp(statename, (g->StateTimerCT).stop());
	}
	else if (sig.msg().compare("Terminate") == 0 && g->execmodel == 2) {
		g->gstate = GuardStateToInit::Instance();
		GuardStateActions::ActionAtSpecTerminated(g);
	}
	else if (sig.msg().compare("Terminate") == 0) {
		assert(0);
		g->gstate = GuardStateToTerminate::Instance();
		GuardStateActions::ActionAtTerminated(g);
	}
	else {
		assert(0); // not valid!
	}
	GlobalProfiler.addWallTimeStamp(statename, (g->StateTimerWT).stop());
	GlobalProfiler.addCPUTimeStamp(statename, (g->StateTimerCT).stop());
	return;
}

void GuardStateToComplete::handle(Guard* g, Signal sig) {
	std::cout << g->name << " GuardStateToComplete::handle: " << sig.msg() << std::endl;
	cputimer.start();
	walltimer.start();
	if (sig.msg().compare("Check") == 0 && g->execmodel == 0) {
		g->gstate = GuardStateToCheck::Instance();

		GuardStateActions::ActionAtCheckingNoStarting(g);
	}
	else if (sig.msg().compare("Check") == 0 && g->execmodel == 1) {
		g->gstate = GuardStateToCheck::Instance();

		GuardStateActions::ActionAtCheckingAndTerminate(g);
	}
	else if (sig.msg().compare("Terminate") == 0 && g->execmodel == 2) {
		assert(0);
	}
	else if (sig.msg().compare("Start") == 0 && g->execmodel == 2) {
		g->gstate = GuardStateToStart::Instance();
		GuardStateActions::ActionAtStart(g);

	}
	else if (sig.msg().compare("Completed") == 0 && g->execmodel == 2) {
		//assert(0);
	}
	else {
		assert(0); // not valid!
	}
	GlobalProfiler.addWallTimeStamp(statename, walltimer.stop());
	GlobalProfiler.addCPUTimeStamp(statename, cputimer.stop());
	return;
}


void GuardStateToPause::handle(Guard* g, Signal sig) {
	std::cout << g->name << " GuardStateToPause::handle: " << sig.msg() << std::endl;
	cputimer.start();
	walltimer.start();
	if (sig.msg().compare("Check") == 0) {
		GuardStateActions::ActionAtCheckingNoStarting(g);
		g->gstate = GuardStateToCheck::Instance();
	}
	else {
		assert(0); // not valid!
	}
	GlobalProfiler.addWallTimeStamp(statename, walltimer.stop());
	GlobalProfiler.addCPUTimeStamp(statename, cputimer.stop());
	return;
}

void GuardStateToTerminate::handle(Guard* g, Signal sig) {
	std::cout << g->name << " GuardStateToTerminate::handle: " << sig.msg() << std::endl;
	cputimer.start();
	walltimer.start();
	if (sig.msg().compare("Check") == 0) {
		//From Complete to Check??
		//Restart??
		GuardStateActions::ActionAtCheckingNoStarting(g);
		g->gstate = GuardStateToCheck::Instance();
	}
	else {
		assert(0); // not valid!
	}
	GlobalProfiler.addWallTimeStamp(statename, walltimer.stop());
	GlobalProfiler.addCPUTimeStamp(statename, cputimer.stop());
	return;
}

void GuardStateToResume::handle(Guard* g, Signal sig) {
	std::cout << g->name << " GuardStateToResume::handle: " << sig.msg() << std::endl;
	cputimer.start();
	walltimer.start();
	if (sig.msg().compare("Check") == 0) {
		GuardStateActions::ActionAtCheckingNoStarting(g);
		g->gstate = GuardStateToCheck::Instance();
		return;
	}
	else {
		assert(0); // not valid!
	}
}




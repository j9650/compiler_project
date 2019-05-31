#include "bellman_ford.h"
#include "bellman_ford_fluid.h"

using namespace example;

void Bellman_fordFluidX::ShortestPath(Graph *graph) {

	auto tf = new TaskFactory();
	auto gs = new AggressiveGS(16);
	auto tp0 = tf->newTask<void(*)(void *), void *>("Begin", &nullfun, NULL);
	auto tp0status = new __fluid__<bool>(tp0->isfinished());
	auto v0 = new ValveEQ<bool>(tp0status, true);

	tp0->run();

	int* anything__call_num_ = new int(0);
	auto call_num_ = new __fluid__<int>(anything__call_num_); // #pragma call_num {__call_num__ call_num_;}
	auto tpb1 = std::bind(&Bellman_fordFluid::Relaxing_edge, this, graph, std::placeholders::_1);
	auto tp1 = tf->newTask<decltype(tpb1), int*>("Relaxing_edge" + std::to_string(0), tpb1, call_num_->p);
	Guard* g0 = gs->newGuard("GuardRelaxing_edge" + std::to_string(0), { v0 }, tp1, {});

	int* anything__call_num = new int(0);
	auto call_num = new __fluid__<int>(anything__call_num); // #pragma call_num {__call_num__ call_num;}
	//call_num = call_num_;
	call_num = call_num_; // #pragma call_num {call_num = call_num_;}

	for (int iter = 1; iter < graph->v_num; iter++) {


		int *anything__call_num_ = new int(0);
		auto call_num_ = new __fluid__<int>(anything__call_num_);  // #pragma call_num {__call_num__ call_num_;} //__call_num__  call_num_.clear();

		auto v0 = new ValveGT<int>(call_num, graph->v_num * rate); //ValveGT(call_num,  graph->e_num * 0.5);
		auto tpb1 = std::bind(&Bellman_fordFluid::Relaxing_edge, this, graph, std::placeholders::_1);
		auto tp1 = tf->newTask<decltype(tpb1), int*>("Relaxing_edge" + std::to_string(iter), tpb1, call_num_->p);
		Guard* g1 = gs->newGuard("GuardRelaxing_edge" + std::to_string(iter), { v0 }, tp1, {});
		
		//call_num = call_num_;
		call_num = call_num_; // #pragma call_num {call_num = call_num_;}

		g0 = g1;

		//Relaxing_edge(graph,call_num);
		//std::cout << "iter: " << iter <<std::endl;
	}
	auto tp3 = tf->newTask<void(*)(void *), void *>("End", &nullfun, NULL);
	auto tp3status = new __fluid__<bool>(tp3->parent->isfinished());
	auto v3 = new ValveEQ<bool>(tp3status, true);
	//while (*(tp3->parent->isfinished()) == false)
	//	; // HARD HACK!
	gs->sync(tp3);
	return;
}
void Bellman_fordFluidXS::ShortestPath(Graph *graph) {

	auto tf = new TaskFactory();
	auto gs = new AggressiveGS(16);
	auto tp0 = tf->newTask<void(*)(void *), void *>("Begin", &nullfun, NULL);
	auto tp0status = new __fluid__<bool>(tp0->isfinished());
	auto v0 = new ValveEQ<bool>(tp0status, true);

	tp0->run();

	int* call_num_ = new int(0);
	auto tp1status_ = new __fluid__<int>(call_num_); //__call_num__  new call_num_;
	auto tpb1 = std::bind(&Bellman_fordFluid::Relaxing_edge_stable, this, graph, std::placeholders::_1);
	auto tp1 = tf->newTask<decltype(tpb1), int*>("Relaxing_edge" + std::to_string(0), tpb1, call_num_);
	Guard* g0 = gs->newGuard("GuardRelaxing_edge" + std::to_string(0), { v0 }, tp1, {});

	int* call_num = new int(0);
	auto tp1status = new __fluid__<int>(call_num);
	for (int iter = 1; iter < graph->v_num; iter++) {

		call_num = call_num_;
		tp1status = tp1status_; // __call_num__ call_num = call_num_;

		call_num_ = new int(0);
		tp1status_ = new __fluid__<int>(call_num_); //__call_num__  call_num_.clear();

		auto v0 = new ValveGT<int>(tp1status, graph->v_num * rate); //ValveGT(call_num,  graph->e_num * 0.5);
		auto tpb1 = std::bind(&Bellman_fordFluid::Relaxing_edge_stable, this, graph, std::placeholders::_1);
		auto tp1 = tf->newTask<decltype(tpb1), int*>("Relaxing_edge" + std::to_string(iter), tpb1, call_num_);
		Guard* g1 = gs->newGuard("GuardRelaxing_edge" + std::to_string(iter), { v0 }, tp1, {});

		g0 = g1;

		//Relaxing_edge(graph,call_num);
	}
	auto tp3 = tf->newTask<void(*)(void *), void *>("End", &nullfun, NULL);
	auto tp3status = new __fluid__<bool>(tp3->parent->isfinished());
	auto v3 = new ValveEQ<bool>(tp3status, true);
	//while (*(tp3->parent->isfinished()) == false)
	//	; // HARD HACK!
	gs->sync(tp3);
	return;
}
void Bellman_fordFluidXSB::ShortestPath(Graph *graph) {

	auto tf = new TaskFactory();
	auto gs = new AggressiveGS(16);
	auto tp0 = tf->newTask<void(*)(void *), void *>("Begin", &nullfun, NULL);
	auto tp0status = new __fluid__<bool>(tp0->isfinished());
	auto v0 = new ValveEQ<bool>(tp0status, true);

	tp0->run();

	int* call_num_ = new int(0);
	auto tp1status_ = new __fluid__<int>(call_num_); //__call_num__  new call_num_;
	auto tpb1 = std::bind(&Bellman_fordFluid::Relaxing_edge_sb, this, graph, 1, std::placeholders::_1);
	auto tp1 = tf->newTask<decltype(tpb1), int*>("Relaxing_edge" + std::to_string(0), tpb1, call_num_);
	Guard* g0 = gs->newGuard("GuardRelaxing_edge" + std::to_string(0), { v0 }, tp1, {});

	int* call_num = new int(0);
	auto tp1status = new __fluid__<int>(call_num);
	for (int iter = 1; iter < graph->v_num; iter++) {

		call_num = call_num_;
		tp1status = tp1status_; // __call_num__ call_num = call_num_;

		call_num_ = new int(0);
		tp1status_ = new __fluid__<int>(call_num_); //__call_num__  call_num_.clear();

		//auto v1_ = new ValveGT<int>(tp1status, graph->v_num * rate); //ValveGT(call_num,  graph->e_num * 0.5);
		Bellman_ford **sb = new Bellman_ford*;
		*sb = this;
		auto sbstatus = new __fluid__<Bellman_ford*>(sb);
		auto v1_ = v21.init(sbstatus, g0->task->isfinished(), iter); //ValveGT(call_num,  graph->e_num * 0.5);
		auto tpb1 = std::bind(&Bellman_fordFluid::Relaxing_edge_sb, this, graph, iter+1, std::placeholders::_1);
		auto tp1 = tf->newTask<decltype(tpb1), int*>("Relaxing_edge" + std::to_string(iter), tpb1, call_num_);
		Guard* g1 = gs->newGuard("GuardRelaxing_edge" + std::to_string(iter), { v1_ }, tp1, {});

		g0 = g1;

		//Relaxing_edge(graph,call_num);
	}
	auto tp3 = tf->newTask<void(*)(void *), void *>("End", &nullfun, NULL);
	auto tp3status = new __fluid__<bool>(tp3->parent->isfinished());
	auto v3 = new ValveEQ<bool>(tp3status, true);
	//while (*(tp3->parent->isfinished()) == false)
	//	; // HARD HACK!
	gs->sync(tp3);
	return;
}
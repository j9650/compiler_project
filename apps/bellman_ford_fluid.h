#pragma once
#include "../fluid/guard.h"
#include "../fluid/valve.h"
#include "../fluid/fluid.h"
#include "../fluid/guardscheduler.h"
#include "bellman_ford.h"
namespace example {

	class Bellman_fordFluidX : public Bellman_fordFluid {
	public:
		virtual void ShortestPath(Graph *graph);
	};
	
	//STABLE single thread
	class Bellman_fordFluidXS : public Bellman_fordFluid {
	public:
		virtual void ShortestPath(Graph *graph);
	};
	//STABLE single thread
	class Bellman_fordFluidXSB : public Bellman_fordFluid {
	public:
		virtual void ShortestPath(Graph *graph);
		ValveSB<Bellman_ford*, bool*, int> v21;
	};
	/*
	//priority queue single thread
	class Bellman_fordFluidXpq : public Bellman_fordFluid {
	public:
		virtual void ShortestPath(Graph *graph);
	};
	//multi thread
	class Bellman_fordFluidX2 : public Bellman_fordFluid {
	public:
		Bellman_fordFluidX2(int _thread_num) {
			thread_num = _thread_num;
		}
		int thread_num;
		virtual void ShortestPath(Graph *graph);
	};
	//STABLE multi thread
	class Bellman_fordFluidX2S : public Bellman_fordFluid {
	public:
		Bellman_fordFluidX2S(int _thread_num) {
			thread_num = _thread_num;
		}
		int thread_num;
		virtual void ShortestPath(Graph *graph);
	};
	//priority queue multi thread
	class Bellman_fordFluidX2pq : public Bellman_fordFluid {
	public:
		Bellman_fordFluidX2pq(int _thread_num) {
			thread_num = _thread_num;
		}
		int thread_num;
		virtual void ShortestPath(Graph *graph);
	};
	*/
};
#pragma once
#include <cmath>
#include <map>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <random>
#include <mutex>
#include <cassert>
#include <vector>
#include <stdio.h>

#define MAX_COUNT 1200000
#define STABLE_TH 6
#define STABLE_DI 1
namespace example {
	class COLOR { //__Fluid__
	public:
		int color;
		int stable;
	};
	class COLOR_ { //__Fluid__
	public:
		int dist;
		int stable;
	};
	class GraphGC {
	public:
		int v_num;
		int e_num;
		int *vertex;
		int *edge;
		int *weights;
		COLOR_ *dist;
		COLOR *color;
		int *randlist;
		int *ga;
		int *shn;

		std::vector<std::string> logs;
	};


	class Graphcolor {
	public:
		static std::string loopindicator;
		double rate;
		double sum;

		int graphcolor(std::string in);
		void InitGraph(GraphGC *graph, std::string in);
  		void Randomcolor(GraphGC *graph);
  		void Checkcolor(GraphGC *graph);
		
		virtual void Coloring(GraphGC *graph);
		virtual void Kernel(GraphGC *graph, int iter);
		virtual void Docolor(GraphGC *graph, int iter, int *tt);
		void huafen(GraphGC *graph, int vid, int iter);

	};
	class GraphcolorFluid : public Graphcolor {
	public:
		GraphcolorFluid() {
					
		}
		//virtual void ShortestPath(GraphGC *graph);
		virtual void Kernel(GraphGC *graph, int iter, int *call_num);
	};
}
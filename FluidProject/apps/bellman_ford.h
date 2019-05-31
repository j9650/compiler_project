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
#define STABLE_TH 8
#define STABLE_DI 1
namespace example {
	class DIST { //__Fluid__
	public:
		int dist;
		int stable;
	};
	class STATUS { //__Fluid__
	public:
		int iter;
		int stable;
	};
	class Graph {
	public:
		int v_num;
		int e_num;
		int *vertex;
		int *edge;
		int *weights;
		DIST *dist;
		STATUS *state;
		bool stop;
	};


	class Bellman_ford {
	public:
		static std::string loopindicator;
		Graph *graph;
		double rate;
		double sum;

		bool stable_check(bool *finished, int iter) {
			if (*finished) return true;
			int cc = 0;
			for (int xx = 0; xx<graph->v_num; xx++)
				if(graph->state[xx].stable>STABLE_TH && graph->state[xx].stable>=iter) cc++;

			//if((cc>rate*srcImage.size && iter == srcImage.iteration) || (*finished)) 
			if((cc>rate*(graph->v_num)) || (*finished)) 
				return true;
			else
				return false;
		}

		int bellman_ford(std::string in);
		void InitGraph(Graph *graph, std::string in);
		virtual void ShortestPath(Graph *graph);
		virtual void Relaxing_edge(Graph* graph);
		void relaxing_edge(Graph* graph, int edge_index, int begin);
		void relaxing_vertex(Graph* graph, int vertex_index);
		void relaxing_vertex_stable(Graph* graph, int vertex_index);
		void relaxing_vertex_sb(Graph* graph, int vertex_index);

	};
	class Bellman_fordFluid : public Bellman_ford {
	public:
		Bellman_fordFluid() {
					
		}
		//virtual void ShortestPath(Graph *graph);
		virtual void Relaxing_edge(Graph* graph, int *call_num);
		virtual void Relaxing_edge_stable(Graph* graph, int *call_num);
		virtual void Relaxing_edge_sb(Graph* graph, int iter, int *call_num);
	};
}
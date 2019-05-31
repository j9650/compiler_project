#include "sssp.h"
#include "../fluid/guard.h"
#include "../fluid/valve.h"
#include "../fluid/fluid.h"
#include "../fluid/guardscheduler.h"
#include "../utils/logger.h"
namespace dragon_li {
	namespace sssp {
		//template< typename Settings >
		class ValveDistPercent : public ValvePerc<std::vector<int>*> {
		private:
			__fluid__< std::vector<int>*>* f;
			std::vector<int>* curr;
			std::vector<int>* past;
			double raito;
		public:


			ValveDistPercent(__fluid__< std::vector<int>*>* _f, double percent) {
				f = nullptr;
				if (_f == nullptr) return;
				perc = percent; f = _f;
				raito = 0;
				auto cpudist = *(f->value());
				curr = new std::vector<int>; past = new std::vector<int>;
				curr->resize(cpudist->size(), 0);
				past->resize(cpudist->size(), 0);
				for (int i = 0; i < cpudist->size(); i++) {
					(*curr)[i] = 0;
					(*past)[i] = (*cpudist)[i];
				}
			}

			bool check() {
				if (f == nullptr) return true;
				raito = 0;
				auto cpudist = *(f->value());
				for (int i = 0; i < cpudist->size(); i++) {
					(*curr)[i] = (*cpudist)[i];
					double rate = (*past)[i] ? ((*curr)[i] - (*past)[i]) / (double)(*past)[i] : 1;
					raito += rate * rate;
				}
				auto temp = curr; curr = past; past = temp;
				if (raito / cpudist->size() <= perc) {
					return true;
				}
				else
					return false;
			}
		};

/*		template< typename Settings >
		class SSSPBFFluid : public SSSPBF<Settings> {
		public:
			AggressiveGS * gs;
			TaskFactory*   tf;
			virtual void UpdateEdges(dragon_li::util::GraphCsr< Types > * graph, int* idx)
			{
				int E = graph->edgeCount;
				for (*idx = 0; *idx < E; (*idx)++) {
					int u = graph->Edges[(*idx)].fromVertex;
					int v = graph->Edges[(*idx)].toVertex;
					int weight = graph->Edges[(*idx)].weight;
					if (cpuSearchDistance[u] != INT_MAX && cpuSearchDistance[u] + weight < cpuSearchDistance[v])
						cpuSearchDistance[v] = cpuSearchDistance[u] + weight;
				}
			}
			virtual void ssspcpu(dragon_li::util::GraphCsr< Types > & graph, VertexIdType src)
			{
				int numberThreads = 4;
				tf = new TaskFactory();
				gs = new AggressiveGS(numberThreads);
				//gs->cputimer.start();
				//gs->walltimer.start();
				int V = graph.vertexCount;
				int E = graph.edgeCount;
				//int dist[V];
				cpuSearchDistance.resize(graph.vertexCount, INF_WEIGHT);
				SyncLogger::print("numberThreads: ", numberThreads);
				// Step 1: Initialize distances from src to all other vertices
				// as INFINITE
				for (int i = 0; i < V; i++)
					cpuSearchDistance[i] = INF_WEIGHT;
				cpuSearchDistance[src] = 0;

				// Step 2: Relax all edges |V| - 1 times. A simple shortest 
				// path from src to any other vertex can have at-most |V| - 1 
				// edges
				int  counter = E;
				auto tp1status = new __fluid__<int>(&counter);
				auto v1 = new ValveGT<int>(tp1status, E*0);

				for (int i = 1; i <= V - 1; i++)
				{
					int* counter = new int(0);
					auto Update = std::bind(&SSSPBFFluid<Settings>::UpdateEdges, this, &graph, counter);
					auto UpdateTask = tf->newTask<decltype(Update)>("UpdateEdges " + std::to_string(i), Update);
					Guard* g1 = gs->newGuard("GuardUpdateEdges" + std::to_string(i), {v1}, UpdateTask, {});
					
					tp1status = new __fluid__<int>(counter);
					v1 = new ValveGT<int>(tp1status, E*1.0/ i);
					std::cout << "VVVV====================: " << i << std::endl;
				}
				auto tp3 = tf->newTask<void(*)(void *), void *>("End", &nullfun, NULL);
				auto tp3status = new __fluid__<bool>(tp3->parent->isfinished());
				auto v3 = new ValveEQ<bool>(tp3status, true);
				//while (*(tp3->parent->isfinished()) == false)
				//	; // HARD HACK!
				gs->sync(tp3);
				// Step 3: check for negative-weight cycles.  The above step 
				// guarantees shortest distances if graph doesn't contain 
				// negative weight cycle.  If we get a shorter path, then there
				// is a cycle.
				for (int i = 0; i < E; i++)
				{
					int u = graph.Edges[i].fromVertex;
					int v = graph.Edges[i].toVertex;
					int weight = graph.Edges[i].weight;
					if (cpuSearchDistance[u] != INT_MAX && cpuSearchDistance[u] + weight < cpuSearchDistance[v])
						printf("Graph contains negative weight cycle");
				}

				//printArr(V);
				std::cout << "BellmanFord" << std::endl;
				//				std::cout << "CPU search depth = " << depth << "\n";
				SyncLogger::print("GS cputimer: ", gs->cputimer.t_total);
				SyncLogger::print("GS walltimer: ", gs->walltimer.t_total);
				return;
			}
		};*/
		/*template< typename Settings >
		class SSSPBFParallelSLOW : public SSSPBF<Settings> {
		public:
			AggressiveGS * gs;
			TaskFactory*   tf;
			std::vector<std::mutex> cpuSearchDistanceMtxs;
			virtual void subUpdates(dragon_li::util::GraphCsr< Types > & graph, int start, int end) {
				for (int idx = start; idx < end; idx++) {
					int u = graph.Edges[idx].fromVertex;
					int v = graph.Edges[idx].toVertex;
					int weight = graph.Edges[idx].weight;
					if (cpuSearchDistance[u] != INT_MAX && cpuSearchDistance[u] + weight < cpuSearchDistance[v])
						cpuSearchDistance[v] = cpuSearchDistance[u] + weight;
				}
			}
			virtual void UpdateEdges(dragon_li::util::GraphCsr< Types > & graph)
			{
				//int E = graph.edgeCount;
				int nthread = 1;
				for (int jj = 0; jj < graph.edgeCount; jj+= graph.edgeCount/ nthread)
				{
					int end = jj + graph.edgeCount / nthread > graph.edgeCount - 1? graph.edgeCount - 1 : jj + graph.edgeCount / nthread;
					auto subUpdate = std::bind(&SSSPBFParallel<Settings>::subUpdates, this, graph, jj, end);
					auto subUpdateTask = tf->newTask<decltype(subUpdate)>("SubUpdates " + std::to_string(jj), subUpdate);
					Guard* g1 = gs->newGuard("GuardSubUpdates" + std::to_string(jj), {}, subUpdateTask, {});

				}
				auto tp3 = tf->newTask<void(*)(void *), void *>("End", &nullfun, NULL);
				auto tp3status = new __fluid__<bool>(tp3->parent->isfinished());
				auto v3 = new ValveEQ<bool>(tp3status, true);
				//while (*(tp3->parent->isfinished()) == false)
				//	; // HARD HACK!
				gs->sync(tp3);
			}
			virtual void ssspcpu(dragon_li::util::GraphCsr< Types > & graph, VertexIdType src)
			{
				tf = new TaskFactory();
				gs = new AggressiveGS(12);
				int V = graph.vertexCount;
				int E = graph.edgeCount;
				//int dist[V];
				cpuSearchDistance.resize(graph.vertexCount, INF_WEIGHT);

				// Step 1: Initialize distances from src to all other vertices
				// as INFINITE
				for (int i = 0; i < V; i++)
					cpuSearchDistance[i] = INF_WEIGHT;
				cpuSearchDistance[src] = 0;

				// Step 2: Relax all edges |V| - 1 times. A simple shortest 
				// path from src to any other vertex can have at-most |V| - 1 
				// edges
				for (int i = 1; i <= V - 1; i++)
				{
					UpdateEdges(graph);
					
					std::cout << "VVVV====================: " << i << std::endl;
				}

				// Step 3: check for negative-weight cycles.  The above step 
				// guarantees shortest distances if graph doesn't contain 
				// negative weight cycle.  If we get a shorter path, then there
				// is a cycle.
				for (int i = 0; i < E; i++)
				{
					int u = graph.Edges[i].fromVertex;
					int v = graph.Edges[i].toVertex;
					int weight = graph.Edges[i].weight;
					if (cpuSearchDistance[u] != INT_MAX && cpuSearchDistance[u] + weight < cpuSearchDistance[v])
						printf("Graph contains negative weight cycle");
				}

				//printArr(V);
				std::cout << "BellmanFord" << std::endl;
				//				std::cout << "CPU search depth = " << depth << "\n";

				return;
			}
		};*/
	};
};


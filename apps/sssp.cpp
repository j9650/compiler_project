//#pragma once


#include "sssp.h"

namespace dragon_li {
	namespace sssp {
		//https://www.geeksforgeeks.org/dynamic-programming-set-23-bellman-ford-algorithm/
		// A utility function used to print the solution
		template<typename Settings>
		void SsspCpu<Settings>::printArr(int n) {
			printf("Vertex   Distance from Source\n");
			for (int i = 0; i < n; ++i)
				printf("%d \t\t %d\n", i, cpuSearchDistance[i]);
		}
		template<typename Settings>
		void SsspCpu<Settings>::UpdateEdges(dragon_li::util::GraphCsr< Types > & graph) {
			//int E = graph.edgeCount;
			for (int j = 0; j < graph.edgeCount; j++)
			{
				int u = graph.Edges[j].fromVertex;
				int v = graph.Edges[j].toVertex;
				int weight = graph.Edges[j].weight;
				if (cpuSearchDistance[u] != INT_MAX && cpuSearchDistance[u] + weight < cpuSearchDistance[v])
					cpuSearchDistance[v] = cpuSearchDistance[u] + weight;
			}
		}


		// The main function that finds shortest distances from src to
		// all other vertices using Bellman-Ford algorithm.  The function
		// also detects negative weight cycle
		template<typename Settings>
		void SSSPBF<Settings>::ssspcpu(dragon_li::util::GraphCsr< typename Settings::Types > & graph, typename Settings::VertexIdType src)
		{
			int V = graph.vertexCount;
			int E = graph.edgeCount;
			//int dist[V];
			this->cpuSearchDistance.resize(graph.vertexCount, this->INF_WEIGHT);

			// Step 1: Initialize distances from src to all other vertices
			// as INFINITE
			for (int i = 0; i < V; i++)
				this->cpuSearchDistance[i] = this->INF_WEIGHT;
			this->cpuSearchDistance[src] = 0;

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
				if (this->cpuSearchDistance[u] != INT_MAX && this->cpuSearchDistance[u] + weight < this->cpuSearchDistance[v])
					printf("Graph contains negative weight cycle");
			}

			//printArr(V);
			std::cout << "BellmanFord" << std::endl;
			//				std::cout << "CPU search depth = " << depth << "\n";

			return;
		}
		template< typename Settings >
		void SSSPDJ<Settings>::ssspcpu(dragon_li::util::GraphCsr< typename Settings:: Types > & graph, typename Settings::VertexIdType srcVertexId) {

			this->cpuSearchDistance.resize(graph.vertexCount, this->INF_WEIGHT);

			std::queue<typename Settings::SizeType> ssspQueue;
			ssspQueue.push(srcVertexId); //start from src;
			this->cpuSearchDistance[srcVertexId] = 0;

			ssspQueue.push(-1); //Depth Marker
			typename Settings::SizeType depth = 0;

			while (!ssspQueue.empty()) {

				typename Settings::VertexIdType nextVertex = ssspQueue.front();
				ssspQueue.pop();

				if (nextVertex == -1) { //Depth Marker
					depth++;

					if (!ssspQueue.empty()) { //Not the last depth marker
						ssspQueue.push(-1); //Push a new depth marker
					}
					continue;
				}

				typename Settings::SizeType vertexDistance = this->cpuSearchDistance[nextVertex];

				typename Settings::SizeType rowStart = graph.rowOffsets[nextVertex];
				typename Settings::SizeType rowEnd = graph.rowOffsets[nextVertex + 1];

				for (typename Settings::SizeType i = rowStart; i < rowEnd; i++) {
					typename Settings::VertexIdType neighborVertex = graph.columnIndices[i];
					typename Settings::EdgeWeightType neighborWeight = graph.columnWeights[i];
					typename Settings::EdgeWeightType neighborDistance = this->cpuSearchDistance[neighborVertex];
					typename Settings::EdgeWeightType newDistance = vertexDistance + neighborWeight;
					if (newDistance < neighborDistance) {
						this->cpuSearchDistance[neighborVertex] = newDistance;
						ssspQueue.push(neighborVertex);
					}
				}

			}

			std::cout << "CPU search depth = " << depth << "\n";
			//printArr(graph.vertexCount);
			//return 0;
		}

	}
};

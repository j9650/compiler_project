#pragma once
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>
namespace dragon_li {
	namespace sssp {

		template<
			typename _Types,
			typename _VertexIdType,
			typename _EdgeWeightType
		>
			class Types : public _Types {

			public:
				typedef _VertexIdType VertexIdType;
				typedef _EdgeWeightType EdgeWeightType;
		};

	}
}
namespace dragon_li {
	namespace util {

		template<
			typename _Types,
			int _THREADS,
			int _CTAS,
			int _CDP_THREADS_BITS,
			int _CDP_THRESHOLD
		>
			class Settings {

			public:
				typedef _Types Types;
				typedef typename _Types::SizeType SizeType;

				static const SizeType THREADS = _THREADS;
				static const SizeType CTAS = _CTAS;
				static const SizeType CDP_THREADS_BITS = _CDP_THREADS_BITS;
				static const SizeType CDP_THREADS = 1 << CDP_THREADS_BITS;
				static const SizeType CDP_THRESHOLD = _CDP_THRESHOLD;

		};

	}
}

namespace dragon_li {
	namespace sssp {

		template<
			typename _Settings,
			typename _Types,
			int _INF_WEIGHT
		>
			class Settings : public _Settings {

			public:
				typedef _Types Types;
				typedef typename _Types::VertexIdType VertexIdType;
				typedef typename _Types::EdgeWeightType EdgeWeightType;
				typedef typename _Settings::SizeType SizeType;

				static const EdgeWeightType INF_WEIGHT = _INF_WEIGHT;
		};

	}
}
namespace dragon_li {

	namespace util {
		//void errorMsg(std::string str) {
		//	assert(0);
		//}
		//void report(std::string str) {
		//	assert(0);
		//}
		
	}
}
namespace dragon_li {
	namespace util {


		template<typename _SizeType>
		class Types {
		public:
			typedef _SizeType SizeType;
		};
	}
}





namespace dragon_li {
	namespace util {


		template<typename Types>
		class GraphFileVertexData {

			typedef typename Types::VertexIdType VertexIdType;
			typedef typename Types::EdgeWeightType EdgeWeightType;
			typedef typename Types::SizeType SizeType;

		public:
			class GraphFileEdgeData {
			public:
				VertexIdType fromVertex;
				VertexIdType toVertex;
				EdgeWeightType weight;

				GraphFileEdgeData() {}
				GraphFileEdgeData(VertexIdType from, VertexIdType to, EdgeWeightType w) :
					fromVertex(from), toVertex(to), weight(w) {}
			};

			VertexIdType vertexId;
			SizeType degree; //count of outgoding edges

							 //Edge list
			std::list< GraphFileEdgeData > edges;

			GraphFileVertexData() : vertexId(-1), degree(0) {}

			GraphFileVertexData(VertexIdType id, SizeType d = 0) : vertexId(id), degree(d) {}

			GraphFileVertexData & operator= (const GraphFileVertexData & vertex) {
				if (this == &vertex) return *this;
				degree = vertex.degree;
				vertexId = vertex.vertexId;
				edges = vertex.edges;
				return *this;
			}

			GraphFileEdgeData & insertEdge(
				VertexIdType to, EdgeWeightType w) {
				edges.push_back(GraphFileEdgeData(vertexId, to, w));
				degree++;
				return edges.back();
			}
			GraphFileEdgeData & insertEdge(
				GraphFileEdgeData & edge) {
				edges.push_back(edge);
				degree++;
				return edges.back();
			}
		};

		template< typename Types >
		class GraphFile {

			typedef typename Types::VertexIdType VertexIdType;
			typedef typename Types::EdgeWeightType EdgeWeightType;
			typedef typename Types::SizeType SizeType;

		public:
			SizeType vertexCount;
			SizeType edgeCount;

			std::vector< GraphFileVertexData< Types > > vertices;

			std::vector< SizeType > histogram;

			GraphFile() : vertexCount(0), edgeCount(0) {}

			virtual int build(const std::string & fileName) = 0;

			int computeHistogram() {

				if (vertices.empty())
				{
					//errorMsg("Graph has not been built!");
					assert(0);

				}

				SizeType maxLogDegree = -1;
				histogram.resize(1, 0);
				for (SizeType i = 0; i < vertexCount; i++) {
					SizeType degree = vertices[i].degree;
					SizeType logDegree = -1;
					while (degree > 0) {
						degree >>= 1;
						logDegree++;
					}
//					report("Log degree " << logDegree);

					//std::cout << "Log degree " << logDegree << std::endl;
					if (logDegree > maxLogDegree) {
						maxLogDegree = logDegree;
						histogram.resize(maxLogDegree + 2, 0);
					}
					histogram[logDegree + 1]++;
				}

				return 0;
			}
		};

	}
}
namespace dragon_li {
	namespace util {

		template<typename Types>
		class GraphFileGR : public GraphFile<Types> {

			typedef typename Types::VertexIdType VertexIdType;
			typedef typename Types::EdgeWeightType EdgeWeightType;
			typedef typename Types::SizeType SizeType;

		public:
			GraphFileGR() : GraphFile<Types>() {}

			int build(const std::string & fileName) {

				std::ifstream grFile(fileName.c_str());
				if (!grFile.is_open()) {
					//errorMsg("Error opening file " << fileName);
					assert(0);

					return -1;
				}

				char keyWord;
				char tmpFileBuf[256];
				SizeType edgeRead = 0;

				grFile >> keyWord;

				while (!grFile.fail()) {

					if (keyWord == 'p') {

						//Problem line, format: p sp total_vertex_count total_edge_count

						grFile >> tmpFileBuf;

						//p followed by sp
						if (!std::strcmp(tmpFileBuf, "sp")) {

							//get vertex and edge count
							grFile >> this->vertexCount;
							grFile >> this->edgeCount;

							this->vertices.resize(this->vertexCount);

							//initialize vertex data
							for (SizeType i = 0; i < this->vertexCount; i++)
								this->vertices[i] = GraphFileVertexData< Types >(i);
						}
						else {
//							errorMsg("Error GR File format for " << fileName);
							assert(0);

							grFile.close();
							return -1;
						}

					}
					else if (keyWord == 'a') { //Arc or edge description line
											   //format: a from_vertex to_vertex edge_weight

						SizeType fromVertexId, toVertexId;
						EdgeWeightType weight;

						//get edge
						grFile >> fromVertexId >> toVertexId >> weight;

						//GR File always start vertex ID from 1
						fromVertexId--;
						toVertexId--;

						edgeRead++;

						//check boundary
						if (fromVertexId >= this->vertexCount) {
//							errorMsg("VertexId " << fromVertexId << " exceeds limit");
							assert(0);

							grFile.close();
							return -1;
						}
						if (toVertexId >= this->vertexCount) {
//							errorMsg("VertexId " << toVertexId << " exceeds limit");
							assert(0);

							grFile.close();
							return -1;
						}

						if (edgeRead > this->edgeCount) {
//							errorMsg("Exceeding edge count " << this->edgeCount);
							assert(0);

							grFile.close();
							return -1;
						}

						//insert edge to vertex data
						this->vertices[fromVertexId].insertEdge(toVertexId, weight);

					}
					else if (keyWord != 'c') { //not comment line, then unknown keyword
//						errorMsg("Error GR File format for " << fileName);
						assert(0);

						grFile.close();
						return -1;
					}

					grFile.getline(tmpFileBuf, 256); //skip to next line

					grFile >> keyWord;

				}

				grFile.close();

				if (this->computeHistogram())
					return -1;

				return 0;

			}
		};


	}
}
namespace dragon_li {
	namespace util {

		template<typename Types>
		class GraphFileMetis : public GraphFile<Types> {

			typedef typename Types::VertexIdType VertexIdType;
			typedef typename Types::EdgeWeightType EdgeWeightType;
			typedef typename Types::SizeType SizeType;

		public:
			GraphFileMetis() : GraphFile<Types>() {}

			int build(const std::string & fileName) {
				std::ifstream metisFile(fileName.c_str());
				if (!metisFile.is_open()) {
//					errorMsg("Error opening file " + fileName);
					assert(0);
					return -1;
				}

				char keyWord;

				metisFile.get(keyWord);

				SizeType currentVertex = -1;
				SizeType edgeRead = 0;

				while (!metisFile.fail()) {

					if (keyWord == '%') { //comment
						while (!metisFile.eof()) {
							metisFile.get(keyWord);
							if (keyWord == '\n')
								break;
						}
					}
					else if (keyWord == ' ' || keyWord == '\t') {
						//skip
					}
					else if (keyWord == '\n') {
						currentVertex++;
					}
					else {
						metisFile.unget();

						if (currentVertex == -1) {
							//Proglem line
							//get vertex and edge count
							metisFile >> this->vertexCount;
							metisFile >> this->edgeCount;

							std::cout << "vertex count " << " " << this->vertexCount << ", edge count " << " " << this->edgeCount << std::endl;
							//assert(0);

							if (!this->vertexCount || !this->edgeCount) {
								assert(0);
								//errorMsg("Unrecognized graph file!");
								return -1;
							}
							this->edgeCount *= 2; //directed edges

							this->vertices.resize(this->vertexCount);

							//initialize vertex data
							for (SizeType i = 0; i < this->vertexCount; i++)
								this->vertices[i] = GraphFileVertexData< Types >(i);
						}
						else {
							//Edge description line for current vertex
							SizeType toVertexId;

							metisFile >> toVertexId;

							toVertexId--;

							edgeRead++;

							if (toVertexId >= this->vertexCount) {
//								errorMsg("VertexId " << toVertexId << " exceeds limit " << this->vertexCount);
								assert(0);

								metisFile.close();
								return -1;
							}

							if (edgeRead > this->edgeCount) {
//								errorMsg("Exceeding edge count " << this->edgeCount);
								assert(0);
								metisFile.close();
								return -1;
							}

							//insert edge to vertex data
							this->vertices[currentVertex].insertEdge(toVertexId, 1/*always 1 for metis graph*/);
						}
					}


					metisFile.get(keyWord);

				}

				metisFile.close();

				if (this->computeHistogram())
					return -1;

				return 0;

			}

		};

	}
}

#include <vector>
#include <list>
#include <iostream>

namespace dragon_li {
	namespace util {

		template < typename Types >
		class GraphCsr {

			typedef typename Types::VertexIdType VertexIdType;
			typedef typename Types::EdgeWeightType EdgeWeightType;
			typedef typename Types::SizeType SizeType;

			typedef typename GraphFileVertexData< Types >::GraphFileEdgeData GraphFileEdgeData;

		public:
			SizeType vertexCount;
			SizeType edgeCount;

			std::vector< VertexIdType > columnIndices;
			std::vector< EdgeWeightType > columnWeights;
			std::vector< SizeType > rowOffsets;
			std::vector< SizeType > histogram;
			
			//added new edge data;
			std::vector< GraphFileEdgeData > Edges;

			GraphCsr();

			int buildFromFile(const std::string & fileName, const std::string & format);

			int displayCsr(bool veryVerbose);

		};

		template < typename Types >
		GraphCsr< Types >::GraphCsr() :
			vertexCount(0), edgeCount(0) {
		}

		template< typename Types >
		int GraphCsr< Types >::buildFromFile(const std::string & fileName,
			const std::string & format) {

			GraphFile< Types > *graphFile;

			if (!format.compare("gr")) {
				graphFile = new GraphFileGR< Types >();
			}
			else if (!format.compare("metis")) {
				graphFile = new GraphFileMetis< Types >();
			}
			else {
				std::cout << "Unrecoginized graph format" << std::endl;
				return -1;
			}


			if (graphFile->build(fileName))
				return -1;

			histogram = graphFile->histogram;

			vertexCount = graphFile->vertexCount;
			edgeCount = graphFile->edgeCount;

			columnIndices.resize(edgeCount);
			columnWeights.resize(edgeCount);
			rowOffsets.resize(vertexCount + 1);

			for (size_t i = 0; i < vertexCount; i++) {
				if (i == 0)
					rowOffsets[0] = 0;
				else {
					rowOffsets[i] =
						rowOffsets[i - 1] + graphFile->vertices[i - 1].degree;
				}

				std::list< GraphFileEdgeData > &edges =
					graphFile->vertices[i].edges;
				
				Edges.insert(Edges.end(), edges.begin(), edges.end());

				size_t startId = rowOffsets[i];
				for (typename std::list< GraphFileEdgeData >::iterator
					edge = edges.begin(); edge != edges.end(); edge++) {
/*					assert(edge->fromVertex == i, "from vertex " << edge->fromVertex
						<< " does not match vertexId " << i);*/

					columnIndices[startId] = edge->toVertex;
					columnWeights[startId++] = edge->weight;
				}

			}

			rowOffsets[vertexCount] = rowOffsets[vertexCount - 1] +
				graphFile->vertices[vertexCount - 1].degree;

			delete graphFile;

			return 0;
		}

		template< typename Types >
		int GraphCsr< Types >::displayCsr(bool veryVerbose) {
			std::cout << "CSR Graph: vertex count " << vertexCount << ", edge count " << edgeCount << "\n";

			if (veryVerbose) {
				for (size_t vertex = 0; vertex < vertexCount; vertex++) {
					std::cout << vertex << ": ";
					for (size_t edge = rowOffsets[vertex]; edge < rowOffsets[vertex + 1]; edge++) {
						std::cout << columnIndices[edge] <<
							"(" << columnWeights[edge] << ")" << ", ";
					}
					std::cout << "total " << rowOffsets[vertex + 1] - rowOffsets[vertex] << "\n";
				}
			}

			std::cout << "Degree Histogram\n";
			int histogramSize = histogram.size();
			for (int i = -1; i < histogramSize - 1; i++) {
				std::cout << "\tDegree 2^" << i << ": " << histogram[i + 1] << "\n"; \
			}
			std::cout << "\n";

			return 0;
		}


	}

}

#include <queue>
#include <climits>

namespace dragon_li {
	namespace sssp {

		

		template< typename Settings >
		class SsspCpu {

		public:

			typedef typename Settings::Types Types;
			typedef typename Settings::SizeType SizeType;
			typedef typename Settings::VertexIdType VertexIdType;
			typedef typename Settings::EdgeWeightType EdgeWeightType;

			static const EdgeWeightType INF_WEIGHT = Settings::INF_WEIGHT;

			std::vector<EdgeWeightType> cpuSearchDistance;

			virtual void ssspcpu(dragon_li::util::GraphCsr< Types > & graph, VertexIdType srcVertexId) = 0;

			void printArr(int n);
			virtual void UpdateEdges(dragon_li::util::GraphCsr< Types > & graph);
		};

		template< typename Settings >
		class SSSPBF : public SsspCpu<Settings> {
		public:
			virtual void ssspcpu(dragon_li::util::GraphCsr< typename Settings::Types > & graph, typename Settings::VertexIdType src);
		};



		template< typename Settings >
		class SSSPDJ : public SsspCpu<Settings> {
		public:
            void ssspcpu(dragon_li::util::GraphCsr< typename Settings:: Types > & graph, typename Settings::VertexIdType srcVertexId);

		};

		//template<typename Settings>
		//std::vector<typename Settings::EdgeWeightType> SsspCpu<Settings>::cpuSearchDistance;
	}
}


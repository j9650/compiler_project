#include "bellman_ford.h"
#include "../utils/logger.h"
#define _CRT_RAND_S
#include "stdlib.h"
#include "../fluid/thread.h"
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <ctime>
#include <random>
#include <chrono>
#include <sstream>      // std::stringstream, std::stringbuf
#include <unordered_set>
#include <set>
#include <vector>

using namespace example;	

int Bellman_ford::bellman_ford(std::string in) {

	graph = new Graph;
	InitGraph(graph,in);

	ShortestPath(graph);
	
	printf("Bellman Ford!\n");
	return 0;
}

void Bellman_ford::InitGraph(Graph *graph, std::string in)
{
	//std::ifstream file (in);
  	std::ifstream file;
    std::cout << "before open file\n";
  	file.open(in, std::ifstream::in);
    std::cout << "after open file\n";

  	file >> graph->v_num;
  	file >> graph->e_num;
  	graph->e_num = graph->e_num * 2;
    std::cout << "OK" << std::endl;
  	graph->vertex = new int[graph->v_num+1];
    std::cout << "OK" << std::endl;
  	graph->edge = new int[graph->e_num ];
    std::cout << "OK" << std::endl;
  	graph->dist = new DIST[graph->v_num];
    std::cout << "OK" << std::endl;
    graph->state = new STATUS[graph->v_num];

  	std::stringstream ss;
  	std::string st;

  	int tot=0;
  	graph->vertex[0] = 0;
  	std::getline(file, st);
  	for (int i=0; i< graph->v_num; i++)
  	{
  		std::getline(file, st);
  		//file >> st;
  		//std::cout << i << " \"" << st << '\"' << std::endl;
  		//std::cout << i << " \"";
  		ss.clear();
  		ss.str("");
  		ss << st;
  		int e;
  		while(ss >> e)
  		{
  			//ss >> graph->edge[tot++];
  			graph->edge[tot++] = e;
  			//std::cout << graph->edge[tot-1] << ' ' ;
  		}
  		graph->vertex[i+1] = tot;
  		//std::cout << "\"" << std::endl;
  	}
  	std::cout << "tot: " << tot << std::endl;

    graph->weights = new int[graph->e_num];
    for(int i=0; i<graph->e_num; i++)
    {
      //graph->weights[i] = graph->edge[i] % 97;
      std::getline(file, st);
      graph->weights[i] = std::stoi(st);
      //std::cout << graph->weights[i] << std::endl;
    }
    for(int i=0; i<graph->v_num; i++)
      graph->dist[i].dist = 55555555;
    for(int i=0; i<graph->vertex[1]; i++)
      graph->dist[graph->edge[i]].dist = graph->weights[i];
    graph->dist[0].dist = 0;
}

void Bellman_ford::ShortestPath(Graph *graph)
{
	//printf("%d\n", 0);
  for (int iter = 0; iter < graph->v_num; iter++) {
    graph->stop = false;
    Relaxing_edge(graph);
  //printf("%d\n", iter);
    //if(graph->stop)
    //{
      //std::cout << "stop at iter: " << iter << std::endl;
      //break;
    //}
  }
}

void Bellman_ford::relaxing_edge(Graph* graph, int edge_index, int begin)
{
	int end = graph->edge[edge_index];
	if (graph->dist[end].dist > graph->dist[begin].dist + graph->weights[edge_index]){
		graph->dist[end].dist = graph->dist[begin].dist + graph->weights[edge_index];
    graph->state[begin].iter++;
  }
}

void Bellman_ford::Relaxing_edge(Graph *graph)
{
	int begin = 0;
  bool tt = false;
	for (int i = 0; i < graph->e_num; i++) {
		while(graph->vertex[begin+1] <= i) begin++;
    //auto j=graph->state[begin].iter;
		relaxing_edge(graph, i, begin);
    //if(j != graph->state[begin].iter) tt = true;
    //relaxing_vertex(graph, i);
	}
  //if(!tt) graph->stop = true;
}


void Bellman_ford::relaxing_vertex(Graph* graph, int vertex_index)
{
  for(int i=graph->vertex[vertex_index]; i<graph->vertex[vertex_index+1]; i++){
    int end = graph->edge[i];
    if (graph->dist[end].dist > graph->dist[vertex_index].dist + graph->weights[i])
      graph->dist[end].dist = graph->dist[vertex_index].dist + graph->weights[i];
  }
}

void Bellman_fordFluid::Relaxing_edge(Graph *graph, int *call_num)
{
  //int begin = 0;
  //std::cout << "Bellman_fordFluid::Relaxing_edge: " << *call_num << std::endl;
  for (int i = 0; i < graph->v_num; i++) {
    //while(graph->vertex[begin+1] <= i) begin++;
    //relaxing_edge(graph, i, begin);
    relaxing_vertex(graph, i);
    (*call_num)++;
  }
}


void Bellman_ford::relaxing_vertex_stable(Graph* graph, int vertex_index)
{
  for(int i=graph->vertex[vertex_index]; i<graph->vertex[vertex_index+1]; i++){
    int end = graph->edge[i];
    if (graph->dist[end].dist > graph->dist[vertex_index].dist + graph->weights[i])
    {
      graph->dist[end].dist = graph->dist[vertex_index].dist + graph->weights[i];
      graph->state[vertex_index].iter++;
    }
  }
}

void Bellman_fordFluid::Relaxing_edge_stable(Graph *graph, int *call_num)
{
  //int begin = 0;
  //std::cout << "Bellman_fordFluid::Relaxing_edge_stable: " << *call_num << std::endl;
  int randl = rand();
  for (int i = 0; i < graph->v_num; i++) {
    //while(graph->vertex[begin+1] <= i) begin++;
    //auto p = graph->state[i];
    if(graph->state[i].stable <= STABLE_TH || randl % 8 == 0)
    {
      auto before = graph->state[i].iter;
      //relaxing_edge(graph, i, begin);
      relaxing_vertex_stable(graph, i);
      if(graph->state[i].iter == before) graph->state[i].stable++;
      else graph->state[i].stable = 0;
    }
    else
      graph->state[i].stable++;
    (*call_num)++;
  }
  /*
    auto p = graph->dist[graph->edge[i]];
    if(graph->dist[graph->edge[i]].stable <= STABLE_TH || randl % (graph->dist[graph->edge[i]].stable) == 0)
    {
      auto before = graph->dist[graph->edge[i]].dist;
      relaxing_edge(graph, i, begin);
      if(graph->dist[graph->edge[i]].dist == before) graph->dist[graph->edge[i]].stable++;
      else graph->dist[graph->edge[i]].stable = 0;
    }
    else
      graph->dist[graph->edge[i]].stable++;
    (*call_num)++;
  }*/
}


void Bellman_ford::relaxing_vertex_sb(Graph* graph, int vertex_index)
{
  for(int i=graph->vertex[vertex_index]; i<graph->vertex[vertex_index+1]; i++){
    int end = graph->edge[i];
    if (graph->dist[end].dist > graph->dist[vertex_index].dist + graph->weights[i])
    {
      graph->dist[end].dist = graph->dist[vertex_index].dist + graph->weights[i];
      graph->state[vertex_index].iter++;
    }
  }
}
void Bellman_fordFluid::Relaxing_edge_sb(Graph *graph, int iter, int *call_num)
{
  //int begin = 0;
  //std::cout << "Bellman_fordFluid::Relaxing_edge_stable: " << *call_num << std::endl;
  //int randl = rand();
  for (int i = 0; i < graph->v_num; i++) {
    //while(graph->vertex[begin+1] <= i) begin++;
    //auto p = graph->state[i];
    //if(graph->state[i].stable <= STABLE_TH || randl % 8 == 0)
    //{
      auto before = graph->state[i].iter;
      //relaxing_edge(graph, i, begin);
      relaxing_vertex_stable(graph, i);
      if(graph->state[i].iter == before) graph->state[i].stable=iter;
      else graph->state[i].stable = 0;
    //}
    //else
    //  graph->state[i].stable++;
    //(*call_num)++;
  }
}

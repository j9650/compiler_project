#include "graphcolor.h"
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

void print_to_logs(GraphGC *graph, std::string st)
{
  graph->logs.push_back(st);
}

int Graphcolor::graphcolor(std::string in) {

	GraphGC graph;
	InitGraph(&graph,in);

  Randomcolor(&graph);
	//ShortestPath(&graph);
	Coloring(&graph);

  Checkcolor(&graph);
	printf("Graphcolor!\n");
	return 0;
}

void Graphcolor::Randomcolor(GraphGC *graph)
{
  graph->randlist = new int[graph->v_num];
  graph->color = new COLOR[graph->v_num];
  graph->ga = new int[graph->v_num];
  graph->shn = new int[graph->v_num];
  for(int i=0; i<graph->v_num; i++) {
    graph->randlist[i] = rand() % graph->v_num;
    graph->color[i].color = 0;
    graph->ga[i] = 0;
    graph->shn[i] = 0;
  }
}

void Graphcolor::Checkcolor(GraphGC *graph)
{
  int x,y;
  x = 0;
  for(int i=0; i<graph->e_num; i++)
  {
    while(graph->vertex[x+1] <= i) x++;
    y = graph->edge[i];
    if(graph->color[x].color == graph->color[y].color)
      //if(graph->color[x].color == 11) 
      std::cout << "error! color[" << x << "] == color[" << y << "] == " << graph->color[x].color << std::endl;
  }
  std::cout << "result correct!\n";

  //FILE *fp = fopen("sha.txt", "w");
  //for(int i=0; i<graph->v_num; i++)
  //{
  //  fprintf(fp, "%d, %d, %d, %d\n", i, graph->color[i].color, graph->ga[i], graph->randlist[i]);
  //  //std::cout << graph->color[i].color << ", " << graph->ga[i] << std::endl;
  //}
  //fclose(fp);
}

void Graphcolor::InitGraph(GraphGC *graph, std::string in)
{
	//std::ifstream file (in);
  	std::ifstream file;
  	file.open(in, std::ifstream::in);

  	file >> graph->v_num;
  	file >> graph->e_num;
  	graph->e_num = graph->e_num * 2;
    std::cout << "OK" << std::endl;
  	graph->vertex = new int[graph->v_num+1];
    std::cout << "OK" << std::endl;
  	graph->edge = new int[graph->e_num ];
    std::cout << "OK" << std::endl;
  	graph->dist = new COLOR_[graph->v_num];

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
      graph->weights[i] = graph->edge[i] % 97;
    for(int i=0; i<graph->v_num; i++)
      graph->dist[i].dist = 55555555;
    for(int i=0; i<graph->vertex[1]; i++)
      graph->dist[graph->edge[i]].dist = graph->weights[i];
    graph->dist[0].dist = 0;
}

void Graphcolor::Coloring(GraphGC *graph)
{
  int tt = graph->v_num;
  int iter = 0;

  while(tt) {
    iter++;
    Kernel(graph, iter);
    Docolor(graph, iter, &tt);
    std::cout <<tt<<std::endl;
  }
  std::cout << "num of color: " << iter << std::endl;
}

void Graphcolor::huafen(GraphGC *graph, int vid, int iter)
{
  int found_larger=0;
  int local_rand = graph->randlist[vid];
  //if(iter == 2 && vid == 2932)
  //  std::cout << "iter =====2 " << vid << std::endl;
  for(int i=graph->vertex[vid]; i<graph->vertex[vid+1]; i++) {
    int dest = graph->edge[i];
    if((graph->color[dest].color && graph->ga[dest] < iter)||(graph->ga[dest] < iter && graph->ga[dest])) continue;
    if((graph->randlist[dest]>local_rand) || (graph->randlist[dest]==local_rand && dest<vid)) found_larger = 1; //if(iter == 2 && vid == 2932) std::cout << "dest: " << dest <<std::endl;}
    //if(iter==11) std::cout << dest << ' ' << graph->randlist[dest] << ' ' << vid << ' ' << local_rand << std::endl;
  }
  //if(found_larger) return;
  //std::cout << vid << " zhaodaoyige! " << iter <<"\n";
  //if(iter == 2 && vid == 2932)
  //  std::cout << "iter =====2 " << vid << std::endl;
  if(!found_larger)
  {
    if(graph->ga[vid] == 0)  graph->ga[vid] = iter;
  }
  else
  {
    //if(iter == 2 && vid == 2932)
    //{
    //  FILE *fp = fopen("caonima.txt","w");
    //  fprintf(fp, "%d\n", vid);
    //  fclose(fp);
    //}
  }
}

void Graphcolor::Kernel(GraphGC *graph, int iter)
{ 
  if(iter%2 != 0)
  {
    for(int i=0; i<graph->v_num; i++) {
      //if(graph->color[i].color == 0)
      huafen(graph, i, iter);
    }
  }
  else
  {
    for(int i=graph->v_num-1; i>=0; i--) {
      //if(graph->color[i].color == 0)
      huafen(graph, i, iter);
    }
  }
}

void Graphcolor::Docolor(GraphGC *graph, int iter, int *tt)
{
  //int tot = 0;
  //for(int t=0; t<10; t++)
  for(int i=0; i<graph->v_num; i++) {
    if(graph->ga[i] && graph->color[i].color == 0)
    {
      //if(i == 3999 || i == 466) std::cout <<  i << ":" << iter << ":" << graph->ga[i] << std::endl;
      graph->color[i].color = graph->ga[i];
      (*tt)--;
    }
  }
  std::cout << "Docolor wan le!\n";
}

void GraphcolorFluid::Kernel(GraphGC *graph, int iter, int *call_num)
{
  if(iter%2 != 100)
  {
    for(int i=0; i<graph->v_num; i++) {
      //if(graph->color[i].color == 0)
      huafen(graph, i, iter),
      (*call_num)++;
      //print_to_logs(graph,"call_num: ");
      //std::cout << "call_num: " << (*call_num) << std::endl;
    }
  }
  else
  {
    for(int i=graph->v_num-1; i>=0; i--) {
      //if(graph->color[i].color == 0)
      huafen(graph, i, iter),
      (*call_num)++;
      //print_to_logs(graph,"call_num: ");
      //std::cout << "call_num: " << (*call_num) << std::endl;
    }
  }
  //std::cout << "Kernel wan le!\n";
}


/////////////////////////////////////////////////////////////////////////////////////////////////

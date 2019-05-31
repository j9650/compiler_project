//#include "../stdafx.h"
#include "../utils/profiler.h"
#include "experiments.h"
#include <thread>
#include <iostream>

Profiler GlobalProfiler;
CPUProfiler GlobalCPUProfiler;

std::string SobelExperiments::inputpath = "/home/mdl/hzj5142/compiler_project/FluidProject/input/";

std::string GraphcolorExperiments::inputpath = "/home/mdl/hzj5142/compiler_project/FluidProject/input/";

std::string Bellman_fordExperiments::inputpath = "/home/mdl/hzj5142/compiler_project/FluidProject/input/";

std::string KMeansExperiments::inputpath = "/home/mdl/hzj5142/compiler_project/FluidProject/input/";
std::string KMeansExperiments::outputpath = "/home/mdl/hzj5142/compiler_project/FluidProject/output/10.out";

std::string TestExperiments::inputpath = "/home/mdl/hzj5142/compiler_project/FluidProject/input/";
std::string TestExperiments::outputpath = "/home/mdl/hzj5142/compiler_project/FluidProject/output/10.out";

std::string SSSPExperiment::inputpath("C:/Users/huz123/source/repos/FluidProject/FluidProject/input/delaunay_n10.graph");
void SSSPExperiment::experiment() {//singleton???
	std::string inputGraphFile = std::string(inputpath);
	std::string  graphFormat = "metis";
	bool veryVerbose = false;
	bool displayGraph = true;
	int startVertexId = 0;
	//Basic Types and Settings


	dragon_li::util::GraphCsr< Types > graph;
	if (graph.buildFromFile(inputGraphFile, graphFormat))
		return;
	if (displayGraph) {
		if (graph.displayCsr(veryVerbose))
			return;
	}
	bool fluid__ = true;
	//dragon_li::sssp::SsspCpu<Settings>::ssspCpuDJ(graph, startVertexId);
	for (int i = 0; i < 1; i++) {
		WallTimer wtdb; wtdb.start();
		CPUTimer  ctdb; ctdb.start();

/*		if (fluid__ == true) {
			auto sssp = new dragon_li::sssp::SSSPBFFluidSpec<Settings>;
			sssp->ssspcpu(graph, startVertexId);
			GlobalProfiler.printTimeStamp();
			GlobalCPUProfiler.printTimeStamp();
			SyncLogger::print("Fluid CPUTimer: ", ctdb.stop(), " WallTimer: ", wtdb.stop());
			auto sssp2 = new dragon_li::sssp::SSSPBF<Settings>;
			sssp2->ssspcpu(graph, startVertexId);
			if (std::equal(sssp->cpuSearchDistance.begin(), sssp->cpuSearchDistance.end(),
				sssp2->cpuSearchDistance.begin(), sssp2->cpuSearchDistance.end()))
				SyncLogger::print("success");
			//auto sssp = new dragon_li::sssp::SSSPBFFluid<Settings>;
			//sssp->ssspcpu(graph, startVertexId);
			//GlobalProfiler.printTimeStamp();
			//GlobalCPUProfiler.printTimeStamp();
			//SyncLogger::print("Fluid CPUTimer: ", ctdb.stop(), " WallTimer: ", wtdb.stop());
			//auto sssp2 = new dragon_li::sssp::SSSPBF<Settings>;
			//sssp2->ssspcpu(graph, startVertexId);
			//if (std::equal(sssp->cpuSearchDistance.begin(), sssp->cpuSearchDistance.end(),
			//	sssp2->cpuSearchDistance.begin(), sssp2->cpuSearchDistance.end()))
			//	SyncLogger::print("success");
		}
		else {
		
			auto sssp = new dragon_li::sssp::SSSPBF<Settings>;
			sssp->ssspcpu(graph, startVertexId);
			GlobalProfiler.printTimeStamp();
			GlobalCPUProfiler.printTimeStamp();
			SyncLogger::print("Fluid CPUTimer: ", ctdb.stop(), " WallTimer: ", wtdb.stop());

		}*/

	}


	return;
}

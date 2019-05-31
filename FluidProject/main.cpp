// FluidProject.cpp : Defines the entry point for the console application.
//
//#include "../../Header1.h"
//#include "stdafx.h"


#include <iostream>       // std::cout, std::endl
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <signal.h>
#include <string>     // std::string, std::to_string
#include <random>

//#pragma once
#include <functional>
#include <iostream>
#include <thread>


#include "./apps/kmeans.h"
#include "./apps/kmeans_fluid.h"
#include "./utils/logger.h"
#include "./apps/experiments.h"


#include "./apps/matrix.h"
#include "./apps/matrix_fluid.h"

#include "./utils/timer.h"

//std::string inputfilename = "cell2.rgb";
//std::string inputfilename = "21.rgb";
std::string inputfilename = "10.rgb";
std::string outputfilename = "10.rgb";
//std::string inputfilename = "rmat.gr";
//std::string inputfilename = "delaunay_n10.graph";
// 1532995200 2018/7/30/ 8:00pm GMT-04:00
std::string compiletime = std::to_string((std::chrono::system_clock::now().time_since_epoch().count()/10000000) % 1532995200);

std::mutex StoppableThread::mtx;
std::random_device StoppableThread::rd; //
//std::random_device StoppableThread::rd(__TIMESTAMP__);
std::string Logger::file("/home/mdl/hzj5142/compiler_project/FluidProject/results/" + std::string("SSSPBFFluidSpec-")+ inputfilename+"."+ compiletime + ".out");
std::ofstream Logger::resultfile(Logger::file);
std::string SyncLogger::prefix;
Logger logger;
SyncLogger slogger;


int main(int argc, char** argv) {
    printf("Hello World!\n");
    int * sb = (int*)malloc(1024*sizeof(int));

    if(sb == NULL){
    	printf("sha bi zhen de bu xing!\n");
    }
	//std::cout << Logger::file << std::endl;

	//return 0;
	//for (int i = 0; i < 10; i++)

	//auto sssp = new SSSPExperiment();
	//sssp->experiment();

	//auto me = new MatrixExperiment<256, 256>();
	//me->experiment();

	//auto mm = new MatrixMultiplication(256,256);
	//mm->exec();

	//inputfilename = std::string(argv[1]);
	//std::cout << inputfilename << std::endl;
	//auto me = new KMeansExperiments(inputfilename);
	//me->rate = (atoi(argv[2])/100.0);
	//std::cout << me->rate << std::endl;
	//me->iterations = atoi(argv[3]);
	//me->class_num = atoi(argv[4]);
	//me->experiment();

	//inputfilename = std::string(argv[1]);
	//std::cout << inputfilename << std::endl;
	//auto me = new Bellman_fordExperiments(inputfilename);
	//me->rate = (atoi(argv[2])/100.0);
	//me -> experiment();

	inputfilename = std::string(argv[1]);
	std::cout << inputfilename << std::endl;
	auto me = new GraphcolorExperiments(inputfilename);
	me->rate = (atoi(argv[2])/100.0);
	me -> experiment();

	//inputfilename = std::string(argv[1]);
	//std::cout << inputfilename << std::endl;
	//outputfilename = std::string(argv[2]);
	//std::cout << outputfilename << std::endl;
	//auto me = new SobelExperiments(inputfilename, outputfilename);
	//me->rate = (atoi(argv[3])/100.0);
	//me->sobel_th = (atoi(argv[4])/100.0);
	//me->sigma = (atoi(argv[5])/1.0);
	//me -> experiment();

	//inputfilename = std::string(argv[1]);
	//std::cout << inputfilename << std::endl;
	//auto me = new TestExperiments(inputfilename);
	//me->rate = (atoi(argv[2])/100.0);
	//std::cout << me->rate << std::endl;
	//me->iterations = atoi(argv[3]);
	//me->class_num = atoi(argv[4]);
	//me->experiment();


	return 0;
}



bool StoppableThread::rdlk = false;
//$(ProjectDir)\input\10.rgb $(ProjectDir)\output\10.kmeans.rgb

// KmeansSchemeFluidX 1:07, 10.rgb
// KmeansRandomChangedRecenter 1:47, 10.rgb

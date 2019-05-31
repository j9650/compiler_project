#pragma once

#include "sssp_fluid.h"
#include "sssp.h"

#include "matrix.h"
#include "matrix_fluid.h"

#include "kmeans_fluid.h"

#include "bellman_ford_fluid.h"

#include "graphcolor_fluid.h"

#include "sobel_fluid.h"

#include "test_out.h"

#include "../utils/timer.h"
#include <stdio.h>
#include <unistd.h>

extern Profiler GlobalProfiler;
extern CPUProfiler GlobalCPUProfiler;
#define THREADS_NUM 4


class Experiements {
public:
	std::string inputfilename;
	std::string outputfilename;
	std::string experimentname;
	virtual void experiment() = 0;
};

class KMeansExperiments : public Experiements {
	
public:
	static std::string inputpath;
	static std::string outputpath;
	KMeansExperiments(std::string filename) {
		inputfilename = filename;
	}
	double rate;
	int iterations;
	int class_num;
	virtual void experiment() {//singleton???
		int n = 1;
		std::vector<example::Clusters *> cvv;
		double cc = 0;
		double ss = 0;
		for (int i = 0; i < n; i++) {

			WallTimer wtdb; wtdb.start();
			CPUTimer  ctdb; ctdb.start();

			wtdb.start();
			ctdb.start();
			//auto km = new example::KmeansSchemeFluidPause();
			//auto km = new example::KmeansRandom();
			//auto km = new example::KmeansRandomP(THREADS_NUM);
			auto km = new example::KmeansSchemeFluidX();
			//auto km = new example::KmeansSchemeFluidX2(THREADS_NUM);
			//auto km = new example::KmeansSchemeFluidXS();
			//auto km = new example::KmeansSchemeFluidX2S(THREADS_NUM);
			//auto km = new example::KmeansSchemeFluidXSB();
			//auto km = new example::KmeansSchemeFluidX2SB(THREADS_NUM);
			//auto km = new example::KmeansSchemeFluidXpq();
			//auto km = new example::Kmeans();

			km -> rate = rate;
			km -> iterations = iterations;
			km -> class_num = class_num;
			km->kmeans(inputpath + inputfilename, outputpath);

			double tt=wtdb.stop();
			cc += tt;
			ss += km->sum;
			SyncLogger::print("Fluid CPUTimer: ", ctdb.stop(), " WallTimer: ", tt);
			FILE *f = fopen("output","a");
			fprintf(f,"%f %f\n",km->sum,tt);
			fclose(f);
			
			//f = fopen("TIME_TAG","w");
			//for(i=0;i<km->time_tag.size();i++)
			//	fprintf(f,"%s\n",km->time_tag[i].c_str());
			//fclose(f);
			
			//auto DF = me1->multi(A, B, C, D);
			cvv.push_back(&(km->clusters));
			//Matrix<int>::verify(DB, DF);
			GlobalProfiler.printTimeStamp();
			GlobalCPUProfiler.printTimeStamp();
		}
		cc = cc / double(n);
		ss = ss / double(n);
		//FILE *f = fopen("WallTimer","a");
		//fprintf(f,"%f\n",cc);
		//fclose(f);
		FILE *f = fopen("DIST","a");
		fprintf(f," %10f %10f\n",ss,cc);
		fclose(f);
		std::cout << "Avg WallTimer: " << cc << " Avg DIST: " << ss << std::endl;
		example::Clusters ctotal;
		ctotal.k = cvv[0]->k; 
		ctotal.centroids = (example::Centroid*)malloc(ctotal.k * sizeof(example::Centroid));



		for (int i = 0; i < ctotal.k; i++) {
			(ctotal.centroids)[i].r = 0;
			(ctotal.centroids)[i].g = 0;
			(ctotal.centroids)[i].b = 0;
			(ctotal.centroids)[i].n = 0;
			for (auto &p : cvv) {
				(ctotal.centroids)[i].r += (p->centroids)[i].r;
				(ctotal.centroids)[i].g += (p->centroids)[i].g;
				(ctotal.centroids)[i].b += (p->centroids)[i].b;
				(ctotal.centroids)[i].n += (p->centroids)[i].n;
			}
		}
		for (int i = 0; i < ctotal.k; i++) {
			SyncLogger::print(
				"R:", (ctotal.centroids)[i].r,
				"G:", (ctotal.centroids)[i].g,
				"B:", (ctotal.centroids)[i].b,
				"N:", (ctotal.centroids)[i].n
			);
		}
		return;
	}
};



template<int const HH, int const WW>
class MatrixExperiment : public Experiements {
	static int hh, ww;
public:
	 virtual void experiment() {//singleton???

		auto me0 = new MatrixMultiplicationBaseline(hh, ww);
		auto me1 = new MatrixMultiplicationFluid(hh, ww);
		auto A = new Matrix<int>(hh, ww);
		auto B = new Matrix<int>(hh, ww);
		auto C = new Matrix<int>(hh, ww);
		auto D = new Matrix<int>(hh, ww, 0);

		WallTimer wtdb; wtdb.start();
		CPUTimer  ctdb; ctdb.start();
		//auto DB = me0->multi(A, B, C, D);
		//std::cout << "Baseline CPUTimer: " << ctdb.stop() << " WallTimer: " << wtdb.stop() <<std::endl;

		wtdb.start();
		ctdb.start();
		auto DF = me1->multi(A, B, C, D);
		SyncLogger::print("Fluid CPUTimer: ", ctdb.stop(), " WallTimer: ", wtdb.stop());
		//auto DF = me1->multi(A, B, C, D);

		//Matrix<int>::verify(DB, DF);
		GlobalProfiler.printTimeStamp();
		GlobalCPUProfiler.printTimeStamp();

		return;
	}
};

template<int const HH, int const WW>
int  MatrixExperiment<HH, WW>::hh = HH;

template<int const HH, int const WW>
int MatrixExperiment<HH, WW>::ww = WW;

class SSSPExperiment: public Experiements {
public:
	static std::string inputpath;
	typedef dragon_li::util::Types<
		int 			//SizeType
	> _Types;

	typedef dragon_li::util::Settings<
		_Types,						//types
		256, 						//THREADS
		104,						//CTAS
		5,							//CDP_THREADS_BITS
		32							//CDP_THRESHOLD
	> _Settings;

	typedef dragon_li::sssp::Types<
		_Types, 		//Basic Types
		int,			//VertexIdType
		int				//EdgeWeightType
	> Types;
	typedef dragon_li::sssp::Settings<
		_Settings, 					//Basic Settings
		Types,						//SSSP Types
		INT_MAX						//Max weight
	> Settings;
	virtual void experiment() override;
};

class Bellman_fordExperiments : public Experiements {
	
public:
	static std::string inputpath;
	static std::string outputpath;
	Bellman_fordExperiments(std::string filename) {
		inputfilename = filename;
	}
	double rate;
	virtual void experiment() {//singleton???
		int n = 1;
		double cc = 0;
		double ss = 0;
		for (int i = 0; i < n; i++) {

			WallTimer wtdb; wtdb.start();
			CPUTimer  ctdb; ctdb.start();

			wtdb.start();
			ctdb.start();
			//auto bf = new example::Bellman_ford();
			//auto bf = new example::Bellman_fordFluidX();
			//auto bf = new example::Bellman_fordFluidXS();
			auto bf = new example::Bellman_fordFluidXSB();
			bf->rate = rate;
			bf -> bellman_ford(inputpath + inputfilename);

			double tt=wtdb.stop();
			SyncLogger::print("Fluid CPUTimer: ", ctdb.stop(), " WallTimer: ", tt);
  			cc += tt;
			{
				FILE *fp = fopen("dist_result","w");
				for(int i=0; i<bf->graph->v_num; i++)
					fprintf(fp, "%d\n", bf->graph->dist[i].dist);
				fclose(fp);
			}

		}
		cc = cc/n;
		FILE *f = fopen("DIST","a");
		fprintf(f," %10f\n",cc);
		fclose(f);
		return;
	}
};

class GraphcolorExperiments : public Experiements {
	
public:
	static std::string inputpath;
	static std::string outputpath;
	GraphcolorExperiments(std::string filename) {
		inputfilename = filename;
	}
	double rate;
	virtual void experiment() {//singleton???
		int n = 1;
		double cc = 0;
		double ss = 0;
		for (int i = 0; i < n; i++) {

			WallTimer wtdb; wtdb.start();
			CPUTimer  ctdb; ctdb.start();

			wtdb.start();
			ctdb.start();
			//auto gc = new example::Graphcolor();
			auto gc = new example::GraphcolorFluidX();
			//auto gc = new example::GraphcolorFluidXS();
			gc->rate = rate;
			gc -> graphcolor(inputpath + inputfilename);

			double tt=wtdb.stop();
			SyncLogger::print("Fluid CPUTimer: ", ctdb.stop(), " WallTimer: ", tt);
		}
		return;
	}
};

class SobelExperiments : public Experiements {
	
public:
	static std::string inputpath;
	static std::string outputpath;
	SobelExperiments(std::string in, std::string out) {
		inputfilename = in;
		outputfilename = out;
	}
	double rate;
	double sobel_th;
	double sigma;
	virtual void experiment() {//singleton???
		int n = 10;
		double cc = 0; //time
		double ss = 0; //iou
		for (int i = 0; i < n; i++) {

			WallTimer wtdb; wtdb.start();
			CPUTimer  ctdb; ctdb.start();

			wtdb.start();
			ctdb.start();
			auto sb = new example::Sobel();
			//auto sb = new example::SobelFluid();
			sb->rate = rate;
			sb->sobel_th = sobel_th;
			sb->sigma = sigma;
			sb -> sobel(inputpath + inputfilename, inputpath + outputfilename);

			double tt=wtdb.stop();
			//SyncLogger::print("Fluid CPUTimer: ", ctdb.stop(), " WallTimer: ", tt);
			printf("Fluid CPUTimer: %10f WallTimer: %10f\n", ctdb.stop(), tt);
			//sleep(1);
  			sb->testImage(sb->image, inputpath + outputfilename);
  			ss += sb->iou;
  			cc += tt;
		}
		ss = ss/n;
		cc = cc/n;
		FILE *f = fopen("DIST","a");
		fprintf(f," %10f %10f\n",ss,cc);
		fclose(f);
		return;
	}
};

class TestExperiments : public Experiements {
	
public:
	static std::string inputpath;
	static std::string outputpath;
	TestExperiments(std::string filename) {
		inputfilename = filename;
	}
	double rate;
	int iterations;
	int class_num;
	virtual void experiment() {//singleton???
		int n = 1;
		std::vector<example::Clusters *> cvv;
		double cc = 0;
		double ss = 0;
		for (int i = 0; i < n; i++) {

			WallTimer wtdb; wtdb.start();
			CPUTimer  ctdb; ctdb.start();

			wtdb.start();
			ctdb.start();
			//auto km = new example::KmeansSchemeFluidX();
			auto km = new example::KmeansImage();
			

			km -> rate = rate;
			km -> iterations = iterations;
			km -> class_num = class_num;
			km->kmeans(inputpath + inputfilename, outputpath);

			double tt=wtdb.stop();
			cc += tt;
			ss += km->sum;
			SyncLogger::print("Fluid CPUTimer: ", ctdb.stop(), " WallTimer: ", tt);
		}
		cc = cc / double(n);
		ss = ss / double(n);
		std::cout << "Avg WallTimer: " << cc << " Avg DIST: " << ss << std::endl;

		return;
	}
};

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
#define STABLE_TH 4
#define STABLE_DI 1
namespace example {




	typedef struct {
		float r;
		float g;
		float b;
		int n;
	} Centroid;

	typedef struct {
		int k;
		Centroid* centroids;
	} Clusters;

	typedef std::vector<Centroid> CentroidVec_t;
	
	typedef struct {
		float r;
		float g;
		float b;
		int cluster;
		float distance;
		int stable;
		int mark;
	} RgbPixel;
	typedef std::vector<RgbPixel> RgbPixelVec_t;

	typedef struct {
		int w;
		int h;
		int size;
		RgbPixel** pixels;
		char* meta;
		int iteration;
		int **pq;
		int *l;
		int *r;
		int ql;
	} RgbImage;

	typedef struct {
		int w;
		int h;
		RgbPixelVec_t pixels;
		char* meta;
	} RgbImageVec;


	class Kmeans {
	public:
		Clusters clusters;
		static std::string loopindicator;
		double rate;
		double sum;
		int iterations;
		int class_num;
		RgbImage srcImage;
		std::vector<std::string> time_tag;

		bool stable_check(bool *finished) {
			if (*finished) return true;
			int cc = 0;
			for (int xx = 0; xx<srcImage.h; xx++)
				for (int yy = 0; yy<srcImage.w; yy++)
					if(srcImage.pixels[xx][yy].stable>STABLE_TH) cc++;

			if(cc>rate*srcImage.size || (*finished)) 
				return true;
			else
				return false;
		}

		bool stable_check(bool *finished, int iter) {
			if (*finished) return true;
			int cc = 0;
			for (int xx = 0; xx<srcImage.h; xx++)
				for (int yy = 0; yy<srcImage.w; yy++)
					if(srcImage.pixels[xx][yy].stable>STABLE_TH) cc++;

			if((cc>rate*srcImage.size && iter == srcImage.iteration) || (*finished)) 
				return true;
			else
				return false;
		}

		bool stable_check(std::vector<bool*> finished) {
			bool tt = false;
			for(int i=0; i<finished.size(); i++)
				tt = tt || *(finished[i]);
			if (tt) return true;
			int cc = 0;
			for (int xx = 0; xx<srcImage.h; xx++)
				for (int yy = 0; yy<srcImage.w; yy++)
					if(srcImage.pixels[xx][yy].stable>STABLE_TH) cc++;

			tt = false;
			for(int i=0; i<finished.size(); i++)
				tt = tt || *(finished[i]);
			if(cc>rate*srcImage.size || tt) 
				return true;
			else
				return false;
		}
		
		
		double print_sum_to_DIST1(RgbImage *image, Clusters *clusters);
		void initRgbImage(RgbImage* image);
		int loadRgbImage(const char* fileName, RgbImage* image, float scale);
		int saveRgbImage(RgbImage* image, const char* fileName, float scale);
		void freeRgbImage(RgbImage* image);

		void makeGrayscale(RgbImage* rgbImage);
		float euclideanDistance(RgbPixel* p, Centroid* c1);
		int pickCluster(RgbPixel* p, Centroid* c1);

		int initClusters(Clusters* clusters, int k, float scale);
		//void segmentImageFluidXY(RgbImage* image, Clusters* clusters, int n);
		//void segmentImageFluidXI(RgbImage* image, Clusters* clusters, int n);
		//void segmentImageFluidX(RgbImage* image, Clusters* clusters, int n);
		//void segmentImageFluid100(RgbImage* image, Clusters* clusters, int n);


		void freeClusters(Clusters* clusters);

		int readCell(FILE *fp, char* w);

		int kmeans(std::string in, std::string out);

		virtual void AssignCluster(RgbImage* image, Clusters* clusters);
		virtual void Recenter(RgbImage* image, Clusters* clusters);
		//virtual void Recenter2(RgbImage* image, Clusters* clusters, int num_thread);
		void assignCluster(RgbPixel* p, Clusters* clusters);
		void assignCluster_stable(RgbPixel* p, Clusters* clusters);
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
		virtual void segmentImageVec(RgbImageVec* image, CentroidVec_t* clusters, int n) {
			assert(0);
		}
		//void Recenter2(RgbImage* image, Clusters* clusters, int& counter);

	};

	class KmeansRandom : public Kmeans {
	public:
		//virtual void AssignCluster(RgbImage* image, Clusters* clusters, int& counter);
		//virtual void Recenter(RgbImage* image, Clusters* clusters);
		KmeansRandom() {
					
		}
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
		virtual void AssignCluster(RgbImage* image, Clusters* clusters, int* counter);
		virtual void AssignCluster2(RgbImage* image, Clusters* clusters, int num_thread, int thread_id, std::vector<int>* counter);
		virtual void AssignCluster2P(RgbImage* image, Clusters* clusters, int num_thread, int* counter);
		virtual void AssignCluster_stable(RgbImage* image, Clusters* clusters, int iter, int* counter);
		virtual void AssignCluster_sb(RgbImage* image, Clusters* clusters, int iter, int* stable);
		virtual void AssignCluster2_stable(RgbImage* image, Clusters* clusters, int num_thread, int thread_id, std::vector<int>* counter);
		virtual void AssignCluster2_sb(RgbImage* image, Clusters* clusters, int num_thread, int thread_id, std::vector<int>* counter);
		virtual void AssignCluster_pq(RgbImage* image, Clusters* clusters, int iter, int* counter);
		virtual void AssignCluster2_pq(RgbImage* image, Clusters* clusters, int num_thread, int* counter);
		//virtual void Recenter2(RgbImage* image, Clusters* clusters, int num_thread);
		//virtual void AssignCluster2_threads(RgbImage* image, Clusters* clusters, std::vector<int>* counter, int thread_id);
	};
	class KmeansRandomP : public KmeansRandom {
	public:
		//virtual void AssignCluster(RgbImage* image, Clusters* clusters, int& counter);
		//virtual void Recenter(RgbImage* image, Clusters* clusters);
		KmeansRandomP(int _thread_num) {
			thread_num = _thread_num;
		}
		int thread_num;
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
		//virtual void AssignCluster(RgbImage* image, Clusters* clusters, int* counter);
		//virtual void AssignCluster2(RgbImage* image, Clusters* clusters, std::vector<int>* counter);
		virtual void Recenter2(RgbImage* image, Clusters* clusters, int num_thread);
		//virtual void AssignCluster2_threads(RgbImage* image, Clusters* clusters, std::vector<int>* counter, int thread_id);
	};
	class KmeansRandomChangedRecenter : public KmeansRandom {
	public:
		//virtual void AssignCluster(RgbImage* image, Clusters* clusters, int& counter);
		virtual void Recenter(RgbImage* image, Clusters* clusters);
		virtual void Recenter2(RgbImage* image, Clusters* clusters, int num_thread);
	};


};


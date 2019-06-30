#include <cmath>
#include <map>
#include <cstdio>
#include <cstring>
#include <random>
#include <mutex>
#include <cassert>
#include <stdio.h>
#include "stdlib.h"
#include <cstdlib>
#include <climits>
#include <ctime>
#include <chrono>
#include <sstream>
#include <unordered_set>
#include <set>
#include <vector>
#include "../fluid/guard.h"
#include "../fluid/valve.h"
#include "../fluid/fluid.h"
#include "../fluid/guardscheduler.h"

#define MAX_COUNT 1200000
#define STABLE_TH 4
#define STABLE_DI 1
#define RAND_ST (randg1/image->size) % 7 < 3

namespace example {
typedef struct {
	float r;
	float g;
	float b;
	int n;
} Centroid_new;

typedef struct {
	int k;
	Centroid_new* centroids;
} Clusters_new;



typedef struct {
	float r;
	float g;
	float b;
	int cluster;
	int stable;
	float distance;
	int mark;
} RgbPixel_new;


typedef struct {
	int w;
	int h;
	int size;
	RgbPixel_new** pixels;
	char* meta;
	int iteration;
	int **pq;
	int *l;
	int *r;
	int ql;
} RgbImage_new;










class KmeansImage {
public:
	Clusters_new clusters;
	static std::string loopindicator;
	double rate;
	double sum;
	int iterations;
	int class_num;


	void initRgbImage(RgbImage_new* image);
	int loadRgbImage(const char* fileName, RgbImage_new* image, float scale);
	int saveRgbImage(RgbImage_new* image, const char* fileName, float scale);
	void freeRgbImage(RgbImage_new* image);

	void makeGrayscale(RgbImage_new* rgbImage);
	float euclideanDistance(RgbPixel_new* p, Centroid_new* c1);
	int pickCluster(RgbPixel_new* p, Centroid_new* c1);

	int initClusters(Clusters_new* clusters, int k, float scale);

	void freeClusters(Clusters_new* clusters);

	int readCell(FILE *fp, char* w);

	int kmeans(std::string in, std::string out);

	virtual void AssignCluster(RgbImage_new* image, Clusters_new* clusters, int *counter);
	virtual void AssignCluster_stable(RgbImage_new* image, Clusters_new* clusters);
	virtual void Recenter(RgbImage_new* image, Clusters_new* clusters);
	void assignCluster(RgbPixel_new* p, Clusters_new* clusters);
	virtual void segmentImage(RgbImage_new* image, Clusters_new* clusters, int n);
	ValveGT<int> v1;
};
}

#include "test_out.h" // 
#define _CRT_RAND_S // 
#include <cstdlib> // 
#include <cstdio> // 
#include <climits> // 
#include <ctime> // 
#include <random> // 
#include <chrono> // 
#include <sstream> // 
#include <unordered_set> // 
#include <set> // 
#include <vector> // 
 // 
using RgbImage = example::RgbImage; // 
using Clusters = example::Clusters; // 
using namespace example; // 
 // 
 // 
 // 
void KmeansImage::AssignCluster(RgbImage_new* image, Clusters_new* clusters) { // KmeansImage
	std::stringstream ss; // KmeansImage
	printf("KmeansRandom::AssignCluster\n"); // KmeansImage
	srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count()); // KmeansImage
	for (int y = 0; y < image->h; y++) { // KmeansImage
		for (int x = 0; x < image->w; x++) { // KmeansImage
 // KmeansImage
		     uint32_t randg1 = StoppableThread::random() % (image->h * image->w); // KmeansImage
 // KmeansImage
			uint32_t tt = randg1; // KmeansImage
			uint32_t xx = tt%image->w; // KmeansImage
			uint32_t yy = tt / image->w; // KmeansImage
			assert(xx < image->w && yy < image->h); // KmeansImage
			{ // KmeansImage
				assignCluster(&image->pixels[yy][xx], clusters); // KmeansImage
				(*counter)++; // KmeansImage
			} // KmeansImage
		} // KmeansImage
	} // KmeansImage
} // 
void KmeansImage::AssignCluster_stable(RgbImage_new* image, Clusters_new* clusters) { // KmeansImage
	image->iteration++; // KmeansImage
	std::stringstream ss; // KmeansImage
	srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count()); // KmeansImage
	for (int y = 0; y < image->h; y++) { // KmeansImage
		for (int x = 0; x < image->w; x++) { // KmeansImage
 // KmeansImage
		    uint32_t randg1 = StoppableThread::random(); // KmeansImage
 // KmeansImage
			uint32_t tt = randg1 % (image->h * image->w); // KmeansImage
			uint32_t xx = tt%image->w; // KmeansImage
 // KmeansImage
			uint32_t yy = tt / image->w; // KmeansImage
			assert(xx < image->w && yy < image->h); // KmeansImage
			RgbPixel_new *p = &image->pixels[yy][xx]; // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
		} // KmeansImage
 // KmeansImage
	} // KmeansImage
} // 
void KmeansImage::segmentImage(RgbImage_new* image, Clusters_new* clusters, int n) { // KmeansImage
 // KmeansImage
	Task::init(4); // KmeansImage
 // KmeansImage
	double  rate = 1; // KmeansImage
	rate = this->rate; // KmeansImage
	double pre = 100000000.0; // KmeansImage
	double sum = 0.0; // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
	int *call_num1_counter = new int(0); // KmeansImage
	auto call_num1 = new __fluid__<int>(call_num1_counter); // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
	<<<g1,{},{},call_num1>>>AssignCluster(image,clusters); // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
	<<<g2,{v1(call_num1, image->h*image->w*rate), v1(call_num1, image->h*image->w*rate) },{}>>>Recenter(image,clusters); // KmeansImage
 // KmeansImage
 // KmeansImage
	auto call_num2 = call_num1; // KmeansImage
	for (int i = 1; i < n; ++i) { // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
		int *call_num1_counter = new int(0); // KmeansImage
		auto call_num1 = new __fluid__<int>(call_num1_counter); // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
		<<<g1,{v1(call_num2, image->h*image->w*rate)},{},call_num1>>>AssignCluster(image,clusters); // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
		<<<g2,{v1(call_num1, image->h*image->w*rate)},{}>>>Recenter(image,clusters); // KmeansImage
 // KmeansImage
 // KmeansImage
		call_num2 = call_num1; // KmeansImage
	} // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
	Task::sync(); // KmeansImage
 // KmeansImage
	sum = 0; // KmeansImage
	for (int y = 0; y < image->h; y++) { // KmeansImage
		for (int x = 0; x < image->w; x++) { // KmeansImage
			sum += (pow(image->pixels[y][x].r - clusters->centroids[image->pixels[y][x].cluster].r, 2) + // KmeansImage
				pow(image->pixels[y][x].g - clusters->centroids[image->pixels[y][x].cluster].g, 2) + // KmeansImage
				pow(image->pixels[y][x].b - clusters->centroids[image->pixels[y][x].cluster].b, 2)); // KmeansImage
		} // KmeansImage
	} // KmeansImage
	this->sum = sum; // KmeansImage
 // KmeansImage
	for (int y = 0; y < image->h; y++) { // KmeansImage
		for (int x = 0; x < image->w; x++) { // KmeansImage
			image->pixels[y][x].r = clusters->centroids[image->pixels[y][x].cluster].r; // KmeansImage
			image->pixels[y][x].g = clusters->centroids[image->pixels[y][x].cluster].g; // KmeansImage
			image->pixels[y][x].b = clusters->centroids[image->pixels[y][x].cluster].b; // KmeansImage
		} // KmeansImage
	} // KmeansImage
} // 
void KmeansImage::Recenter(RgbImage_new* image, Clusters_new* clusters) { // KmeansImage
	auto r = new double[clusters->k]; // KmeansImage
	auto g = new double[clusters->k]; // KmeansImage
	auto b = new double[clusters->k]; // KmeansImage
	auto n = new int[clusters->k]; // KmeansImage
	for (int c = 0; c < clusters->k; ++c) { // KmeansImage
		r[c] = 0.; // KmeansImage
		g[c] = 0.; // KmeansImage
		b[c] = 0.; // KmeansImage
		n[c] = 0; // KmeansImage
	} // KmeansImage
 // KmeansImage
	for (int y = 0; y < image->h; y++) { // KmeansImage
		for (int x = 0; x < image->w; x++) { // KmeansImage
			r[image->pixels[y][x].cluster] += image->pixels[y][x].r; // KmeansImage
			g[image->pixels[y][x].cluster] += image->pixels[y][x].g; // KmeansImage
			b[image->pixels[y][x].cluster] += image->pixels[y][x].b; // KmeansImage
			n[image->pixels[y][x].cluster] += 1; // KmeansImage
		} // KmeansImage
	} // KmeansImage
	for (int c = 0; c < clusters->k; ++c) { // KmeansImage
 // KmeansImage
		clusters->centroids[c].n = n[c]; // KmeansImage
	} // KmeansImage
	for (int c = 0; c < clusters->k; ++c) { // KmeansImage
		if (n[c] == 0) { // KmeansImage
			clusters->centroids[c].r = r[c]; // KmeansImage
			clusters->centroids[c].g = g[c]; // KmeansImage
			clusters->centroids[c].b = b[c]; // KmeansImage
 // KmeansImage
		} // KmeansImage
		else if (n[c] != 0) { // KmeansImage
 // KmeansImage
			clusters->centroids[c].r = r[c] / n[c]; // KmeansImage
			clusters->centroids[c].g = g[c] / n[c]; // KmeansImage
			clusters->centroids[c].b = b[c] / n[c]; // KmeansImage
 // KmeansImage
		} // KmeansImage
	} // KmeansImage
	delete[] r; // KmeansImage
	delete[] g; // KmeansImage
	delete[] b; // KmeansImage
	delete[] n; // KmeansImage
} // 
void KmeansImage::assignCluster(RgbPixel_new* p, Clusters_new* clusters) { // KmeansImage
	int i = 0; // KmeansImage
 // KmeansImage
	float d; // KmeansImage
	d = euclideanDistance(p, &clusters->centroids[i]); // KmeansImage
	p->distance = d; // KmeansImage
 // KmeansImage
	for (i = 1; i < clusters->k; ++i) { // KmeansImage
		d = euclideanDistance(p, &clusters->centroids[i]); // KmeansImage
		if (d < p->distance) { // KmeansImage
			p->cluster = i; // KmeansImage
			p->distance = d; // KmeansImage
		} // KmeansImage
	} // KmeansImage
} // 
int KmeansImage::kmeans(std::string in, std::string out) { // KmeansImage
 // KmeansImage
	RgbImage_new srcImage; // KmeansImage
 // KmeansImage
 // KmeansImage
	initRgbImage(&srcImage); // KmeansImage
 // KmeansImage
	std::string inImageName = in; // KmeansImage
	std::string outImageName = out; // KmeansImage
 // KmeansImage
	loadRgbImage(inImageName.c_str(), &srcImage, 256); // KmeansImage
 // KmeansImage
	initClusters(&clusters, class_num, 1); // KmeansImage
	segmentImage(&srcImage, &clusters, iterations); // KmeansImage
	saveRgbImage(&srcImage, outImageName.c_str(), 255); // KmeansImage
 // KmeansImage
 // KmeansImage
	freeRgbImage(&srcImage); // KmeansImage
	return 0; // KmeansImage
} // 
float KmeansImage::euclideanDistance(RgbPixel_new* p, Centroid_new* c1) { // KmeansImage
	float r; // KmeansImage
 // KmeansImage
	r = 0; // KmeansImage
	double r_tmp; // KmeansImage
 // KmeansImage
	double dataIn[6]; // KmeansImage
	dataIn[0] = p->r; // KmeansImage
	dataIn[1] = p->g; // KmeansImage
	dataIn[2] = p->b; // KmeansImage
	dataIn[3] = c1->r; // KmeansImage
	dataIn[4] = c1->g; // KmeansImage
	dataIn[5] = c1->b; // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
	r += (p->r - c1->r) * (p->r - c1->r); // KmeansImage
	r += (p->g - c1->g) * (p->g - c1->g); // KmeansImage
	r += (p->b - c1->b) * (p->b - c1->b); // KmeansImage
 // KmeansImage
	r_tmp = sqrt(r); // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
	return (float)r_tmp; // KmeansImage
} // 
int KmeansImage::pickCluster(RgbPixel_new* p, Centroid_new* c1) { // KmeansImage
	float d1; // KmeansImage
 // KmeansImage
	d1 = euclideanDistance(p, c1); // KmeansImage
 // KmeansImage
	if (p->distance <= d1) // KmeansImage
		return 0; // KmeansImage
 // KmeansImage
	return 1; // KmeansImage
} // 
int KmeansImage::initClusters(Clusters_new* clusters, int k, float scale) { // KmeansImage
	int i; // KmeansImage
	float x; // KmeansImage
	std::random_device rd; // KmeansImage
 // KmeansImage
 // KmeansImage
	clusters->centroids = new Centroid_new[k]; // KmeansImage
 // KmeansImage
	if (clusters == NULL) { // KmeansImage
		printf("Warning: Oops! Cannot allocate memory for the clusters!\n"); // KmeansImage
 // KmeansImage
		return 0; // KmeansImage
	} // KmeansImage
 // KmeansImage
	clusters->k = k; // KmeansImage
	for (i = 0; i < clusters->k; i++) { // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
		x = (((uint32_t)rd()) *1.0 / (uint32_t)(std::numeric_limits<unsigned int>::max())) * scale; // KmeansImage
		clusters->centroids[i].r =0.5; // KmeansImage
 // KmeansImage
		x = (((uint32_t)rd()) *1.0 / (uint32_t)(std::numeric_limits<unsigned int>::max())) * scale; // KmeansImage
		clusters->centroids[i].g =  0.5; // KmeansImage
 // KmeansImage
		x = (((uint32_t)rd()) *1.0 / (uint32_t)(std::numeric_limits<unsigned int>::max())) * scale; // KmeansImage
		clusters->centroids[i].b =  0.5; // KmeansImage
		clusters->centroids[i].n = 0; // KmeansImage
	} // KmeansImage
 // KmeansImage
 // KmeansImage
	return 1; // KmeansImage
} // 
void KmeansImage::freeClusters(Clusters_new* clusters) { // KmeansImage
	if (clusters != NULL) // KmeansImage
		free(clusters->centroids); // KmeansImage
} // 
void KmeansImage::initRgbImage(RgbImage_new* image) { // KmeansImage
	image->w = 0; // KmeansImage
	image->h = 0; // KmeansImage
	image->pixels = NULL; // KmeansImage
	image->meta = NULL; // KmeansImage
	image->iteration = 0; // KmeansImage
} // 
int KmeansImage::readCell(FILE *fp, char* w) { // KmeansImage
	int c; // KmeansImage
	int i; // KmeansImage
 // KmeansImage
	w[0] = 0; // KmeansImage
	for (c = fgetc(fp), i = 0; c != EOF; c = fgetc(fp)) { // KmeansImage
		if (c == ' ' || c == '\t') { // KmeansImage
			if (w[0] != '\"') // KmeansImage
				continue; // KmeansImage
		} // KmeansImage
 // KmeansImage
		if (c == ',' || c == '\n') { // KmeansImage
			if (w[0] != '\"') // KmeansImage
				break; // KmeansImage
			else if (c == '\"') { // KmeansImage
				w[i] = c; // KmeansImage
				i++; // KmeansImage
				break; // KmeansImage
			} // KmeansImage
		} // KmeansImage
 // KmeansImage
		w[i] = c; // KmeansImage
		i++; // KmeansImage
	} // KmeansImage
	w[i] = 0; // KmeansImage
 // KmeansImage
	return c; // KmeansImage
} // 
int KmeansImage::loadRgbImage(const char* fileName, RgbImage_new* image, float scale) { // KmeansImage
	int c; // KmeansImage
	int i; // KmeansImage
	int j; // KmeansImage
	char w[256]; // KmeansImage
	RgbPixel_new** pixels; // KmeansImage
	FILE *fp; // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
	fp = fopen(fileName, "r"); // KmeansImage
	if (!fp) { // KmeansImage
		printf("Warning: Oops! Cannot open %s!\n", fileName); // KmeansImage
		return 0; // KmeansImage
	} // KmeansImage
 // KmeansImage
	c = readCell(fp, w); // KmeansImage
	image->w = atoi(w); // KmeansImage
	c = readCell(fp, w); // KmeansImage
	image->h = atoi(w); // KmeansImage
	image->size = image->w * image->h; // KmeansImage
	image->pq = new int*[STABLE_TH+1]; // KmeansImage
	image->l = new int[STABLE_TH+1]; // KmeansImage
	image->r = new int[STABLE_TH+1]; // KmeansImage
	image->ql = image->size+1; // KmeansImage
	for(int i=0; i<=STABLE_TH; i++) // KmeansImage
	{ // KmeansImage
		image->pq[i] = new int[image->ql]; // KmeansImage
		image->l[i] = 1; // KmeansImage
		image->r[i] = 0; // KmeansImage
		if(i==0) // KmeansImage
		{ // KmeansImage
			for(int j=0; j<image->size;j++) // KmeansImage
				image->pq[0][j] = j; // KmeansImage
			image->r[i] = image->size-1; // KmeansImage
		} // KmeansImage
	} // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
	pixels = (RgbPixel_new**)malloc(image->h * sizeof(RgbPixel_new*)); // KmeansImage
 // KmeansImage
 // KmeansImage
 // KmeansImage
	if (pixels == NULL) { // KmeansImage
		printf("Warning: Oops! Cannot allocate memory for the pixels!2\n"); // KmeansImage
 // KmeansImage
		fclose(fp); // KmeansImage
 // KmeansImage
		return 0; // KmeansImage
	} // KmeansImage
 // KmeansImage
	c = 0; // KmeansImage
	for (i = 0; i < image->h; i++) { // KmeansImage
		pixels[i] = (RgbPixel_new*)malloc(image->w * sizeof(RgbPixel_new)); // KmeansImage
		if (pixels[i] == NULL) { // KmeansImage
			c = 1; // KmeansImage
			break; // KmeansImage
		} // KmeansImage
	} // KmeansImage
 // KmeansImage
	if (c == 1) { // KmeansImage
		printf("Warning: Oops! Cannot allocate memory for the pixels!1\n"); // KmeansImage
 // KmeansImage
		for (i--; i >= 0; i--) // KmeansImage
			free(pixels[i]); // KmeansImage
		free(pixels); // KmeansImage
 // KmeansImage
		fclose(fp); // KmeansImage
 // KmeansImage
		return 0; // KmeansImage
	} // KmeansImage
 // KmeansImage
	for (i = 0; i < image->h; i++) { // KmeansImage
		for (j = 0; j < image->w; j++) { // KmeansImage
			c = readCell(fp, w); // KmeansImage
			pixels[i][j].r = atoi(w) / scale; // KmeansImage
 // KmeansImage
			c = readCell(fp, w); // KmeansImage
			pixels[i][j].g = atoi(w) / scale; // KmeansImage
 // KmeansImage
			c = readCell(fp, w); // KmeansImage
			pixels[i][j].b = atoi(w) / scale; // KmeansImage
 // KmeansImage
			pixels[i][j].cluster = 0; // KmeansImage
			pixels[i][j].distance = 0.; // KmeansImage
			pixels[i][j].stable = 0; // KmeansImage
			pixels[i][j].mark = 0; // KmeansImage
		} // KmeansImage
	} // KmeansImage
	image->pixels = pixels; // KmeansImage
 // KmeansImage
	c = readCell(fp, w); // KmeansImage
	image->meta = (char*)malloc(256); // KmeansImage
	if (image->meta == NULL) { // KmeansImage
		printf("Warning: Oops! Cannot allocate memory for the pixels!3\n"); // KmeansImage
 // KmeansImage
		for (i = 0; i < image->h; i++) // KmeansImage
			free(pixels[i]); // KmeansImage
		free(pixels); // KmeansImage
 // KmeansImage
		fclose(fp); // KmeansImage
 // KmeansImage
		return 0; // KmeansImage
 // KmeansImage
	} // KmeansImage
	strcpy(image->meta, w); // KmeansImage
 // KmeansImage
 // KmeansImage
	return 1; // KmeansImage
} // 
int KmeansImage::saveRgbImage(RgbImage_new* image, const char* fileName, float scale) { // KmeansImage
	int i; // KmeansImage
	int j; // KmeansImage
	FILE *fp; // KmeansImage
 // KmeansImage
 // KmeansImage
	fp = fopen(fileName, "w"); // KmeansImage
	if (!fp) { // KmeansImage
		printf("Warning: Oops! Cannot open %s!\n", fileName); // KmeansImage
		return 0; // KmeansImage
	} // KmeansImage
 // KmeansImage
	fprintf(fp, "%d,%d\n", image->w, image->h); // KmeansImage
 // KmeansImage
	for (i = 0; i < image->h; i++) { // KmeansImage
		for (j = 0; j < (image->w - 1); j++) { // KmeansImage
			fprintf(fp, "%d,%d,%d,", int(image->pixels[i][j].r * scale), int(image->pixels[i][j].g * scale), int(image->pixels[i][j].b * scale)); // KmeansImage
		} // KmeansImage
		fprintf(fp, "%d,%d,%d\n", int(image->pixels[i][j].r * scale), int(image->pixels[i][j].g * scale), int(image->pixels[i][j].b * scale)); // KmeansImage
	} // KmeansImage
 // KmeansImage
	fprintf(fp, "%s", image->meta); // KmeansImage
 // KmeansImage
 // KmeansImage
	fclose(fp); // KmeansImage
 // KmeansImage
	return 1; // KmeansImage
} // 
void KmeansImage::freeRgbImage(RgbImage_new* image) { // KmeansImage
	int i; // KmeansImage
 // KmeansImage
	if (image->meta != NULL) // KmeansImage
		free(image->meta); // KmeansImage
 // KmeansImage
	if (image->pixels == NULL) // KmeansImage
		return; // KmeansImage
 // KmeansImage
	for (i = 0; i < image->h; i++) // KmeansImage
		if (image->pixels != NULL) // KmeansImage
			free(image->pixels[i]); // KmeansImage
 // KmeansImage
	free(image->pixels); // KmeansImage
} // 
void KmeansImage::makeGrayscale(RgbImage_new* image) { // KmeansImage
	int i; // KmeansImage
	int j; // KmeansImage
	float luminance; // KmeansImage
 // KmeansImage
	float rC = (float)0.30; // KmeansImage
	float gC = (float)0.59; // KmeansImage
	float bC = (float)0.11; // KmeansImage
 // KmeansImage
	for (i = 0; i < image->h; i++) { // KmeansImage
		for (j = 0; j < image->w; j++) { // KmeansImage
			luminance = // KmeansImage
				rC * image->pixels[i][j].r + // KmeansImage
				gC * image->pixels[i][j].g + // KmeansImage
				bC * image->pixels[i][j].b; // KmeansImage
 // KmeansImage
			image->pixels[i][j].r = (unsigned char)luminance; // KmeansImage
			image->pixels[i][j].g = (unsigned char)luminance; // KmeansImage
			image->pixels[i][j].b = (unsigned char)luminance; // KmeansImage
		} // KmeansImage
	} // KmeansImage
} // 
#include <cmath> // 
#include <map> // 
#include <cstdio> // 
#include <cstring> // 
#include <random> // 
#include <mutex> // 
#include <cassert> // 
#include <stdio.h> // 
#include "stdlib.h" // 
#include <cstdlib> // 
#include <climits> // 
#include <ctime> // 
#include <chrono> // 
#include <sstream> // 
#include <unordered_set> // 
#include <set> // 
#include <vector> // 
#include "../fluid/guard.h" // 
#include "../fluid/valve.h" // 
#include "../fluid/fluid.h" // 
#include "../fluid/guardscheduler.h" // 
 // 
#define MAX_COUNT 1200000 // 
#define STABLE_TH 4 // 
#define STABLE_DI 1 // 
#define RAND_ST (randg1/image->size) % 7 < 3 // 
 // 
namespace example { // 
typedef struct { // 
	float r; // 
	float g; // 
	float b; // 
	int n; // 
} Centroid_new; // 
 // 
typedef struct { // 
	int k; // 
	Centroid_new* centroids; // 
} Clusters_new; // 
 // 
 // 
__Fluid__ // 
typedef struct { // 
	float r; // 
	float g; // 
	float b; // 
	int cluster; // 
	int stable; // 
	float distance; // 
	int mark; // 
} RgbPixel_new; // 
 // 
 // 
typedef struct { // 
	int w; // 
	int h; // 
	int size; // 
	RgbPixel_new** pixels; // 
	char* meta; // 
	int iteration; // 
	int **pq; // 
	int *l; // 
	int *r; // 
	int ql; // 
} RgbImage_new; // 
 // 
 // 
 // 
 // 
 // 
 // 
 // 
 // 
 // 
__Fluid__ // KmeansImage
class KmeansImage { // KmeansImage
public: // KmeansImage
	Clusters_new clusters; // KmeansImage
	static std::string loopindicator; // KmeansImage
	double rate; // KmeansImage
	double sum; // KmeansImage
	int iterations; // KmeansImage
	int class_num; // KmeansImage
 // KmeansImage
 // KmeansImage
	void initRgbImage(RgbImage_new* image); // KmeansImage
	int loadRgbImage(const char* fileName, RgbImage_new* image, float scale); // KmeansImage
	int saveRgbImage(RgbImage_new* image, const char* fileName, float scale); // KmeansImage
	void freeRgbImage(RgbImage_new* image); // KmeansImage
 // KmeansImage
	void makeGrayscale(RgbImage_new* rgbImage); // KmeansImage
	float euclideanDistance(RgbPixel_new* p, Centroid_new* c1); // KmeansImage
	int pickCluster(RgbPixel_new* p, Centroid_new* c1); // KmeansImage
 // KmeansImage
	int initClusters(Clusters_new* clusters, int k, float scale); // KmeansImage
 // KmeansImage
	void freeClusters(Clusters_new* clusters); // KmeansImage
 // KmeansImage
	int readCell(FILE *fp, char* w); // KmeansImage
 // KmeansImage
	int kmeans(std::string in, std::string out); // KmeansImage
 // KmeansImage
	virtual void AssignCluster(RgbImage_new* image, Clusters_new* clusters); // KmeansImage
	virtual void AssignCluster_stable(RgbImage_new* image, Clusters_new* clusters); // KmeansImage
	virtual void Recenter(RgbImage_new* image, Clusters_new* clusters); // KmeansImage
	void assignCluster(RgbPixel_new* p, Clusters_new* clusters); // KmeansImage
	virtual void segmentImage(RgbImage_new* image, Clusters_new* clusters, int n); // KmeansImage
	ValveGT<int> v1; // KmeansImage
}; // KmeansImage
} // 

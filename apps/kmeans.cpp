//#include "../stdafx.h"
#include "kmeans.h"
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
#include <sys/time.h>

using namespace example;	


double Kmeans::print_sum_to_DIST1(RgbImage *image, Clusters *clusters)
{
	double sum = 0;

	//https://www.tandfonline.com/doi/abs/10.1080/01969727308546046
	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			sum += (pow(image->pixels[y][x].r - clusters->centroids[image->pixels[y][x].cluster].r, 2) +
				pow(image->pixels[y][x].g - clusters->centroids[image->pixels[y][x].cluster].g, 2) +
				pow(image->pixels[y][x].b - clusters->centroids[image->pixels[y][x].cluster].b, 2));
		}
	}

	//https://www.tandfonline.com/doi/pdf/10.1007/s11806-008-0094-8
	//for (int y = 0; y < image->h; y++) {
	//	for (int x = 0; x < image->w; x++) {
	//		for(int i = 0; i<clusters->k; i++) {
	//		//int i = image->pixels[y][x].cluster;
	//			double uij = 0;
	//			for(int c=0; c< clusters->k; c++) {
	//				uij += (pow(image->pixels[y][x].r - clusters->centroids[c].r, 2) +
	//						pow(image->pixels[y][x].g - clusters->centroids[c].g, 2) +
	//						pow(image->pixels[y][x].b - clusters->centroids[c].b, 2));
	//			}
	//			uij = (pow(image->pixels[y][x].r - clusters->centroids[i].r, 2) +
	//						pow(image->pixels[y][x].g - clusters->centroids[i].g, 2) +
	//						pow(image->pixels[y][x].b - clusters->centroids[i].b, 2)) /
	//						uij;
	//			sum+=uij*log(uij);
	//		}
	//	}
	//}
	//sum = sum / image->h / image->w;

	//double *sigma = new double[clusters->k];
	//for (int i=0; i < clusters->k; i++)
	//	sigma[i] = 0.0;
	//for (int y = 0; y < image->h; y++) {
	//	for (int x = 0; x < image->w; x++) {
	//		sigma[image->pixels[y][x].cluster] += sqrt(pow(image->pixels[y][x].r - clusters->centroids[image->pixels[y][x].cluster].r, 2) +
	//			pow(image->pixels[y][x].g - clusters->centroids[image->pixels[y][x].cluster].g, 2) +
	//			pow(image->pixels[y][x].b - clusters->centroids[image->pixels[y][x].cluster].b, 2));
	//	}
	//}
	//for (int i=0; i < clusters->k; i++) {
	//	sigma[i] = sigma[i] / clusters->centroids[i].n;
	//	//std::cout << sigma[i] << std::endl;
	//}
	//for (int i=0; i < clusters->k; i++) {
	//	double max = 0.0;
	//	for (int j=0; j < clusters->k; j++) {
	//		if (i == j) continue;
	//		double tmp= (sigma[i] + sigma[j]) / sqrt(pow(clusters->centroids[i].r - clusters->centroids[j].r, 2) +
	//						pow(clusters->centroids[i].g - clusters->centroids[j].g, 2) +
	//						pow(clusters->centroids[i].b - clusters->centroids[j].b, 2));
	//		if (tmp > max) max = tmp;
	//		//sum += tmp;
	//	}
	//	sum += max;
	//}

	//print
	std::cout << "xin ban acc: " << sum << std::endl;
	FILE *f = fopen("DIST1","a");
	fprintf(f,"%f\n",sum);
	fclose(f);

	return sum;
}

int Kmeans::kmeans(std::string in, std::string out) {

	//RgbImage srcImage;
	

	initRgbImage(&srcImage);

	std::string inImageName = in;
	std::string outImageName = out;

	loadRgbImage(inImageName.c_str(), &srcImage, 256);

	initClusters(&clusters, class_num, 1);
	segmentImage(&srcImage, &clusters, iterations);
	saveRgbImage(&srcImage, outImageName.c_str(), 255);


	freeRgbImage(&srcImage);
	//freeClusters(&clusters);
	return 0;
}


float Kmeans::euclideanDistance(RgbPixel* p, Centroid* c1) {
	float r;

	r = 0;
	double r_tmp;

	double dataIn[6];
	dataIn[0] = p->r;
	dataIn[1] = p->g;
	dataIn[2] = p->b;
	dataIn[3] = c1->r;
	dataIn[4] = c1->g;
	dataIn[5] = c1->b;

	//#pragma parrot(input, "kmeans", [6]dataIn)

	r += (p->r - c1->r) * (p->r - c1->r);
	r += (p->g - c1->g) * (p->g - c1->g);
	r += (p->b - c1->b) * (p->b - c1->b);

	r_tmp = sqrt(r);

	//#pragma parrot(output, "kmeans", <0.0; 1.0>r_tmp)

	return (float)r_tmp;
}

int Kmeans::pickCluster(RgbPixel* p, Centroid* c1) {
	float d1;

	d1 = euclideanDistance(p, c1);

	if (p->distance <= d1)
		return 0;

	return 1;
}

void Kmeans::assignCluster(RgbPixel* p, Clusters* clusters) {
	int i = 0;

	float d;
	d = euclideanDistance(p, &clusters->centroids[i]);
	p->distance = d;

	for (i = 1; i < clusters->k; ++i) {
		d = euclideanDistance(p, &clusters->centroids[i]);
		if (d < p->distance) {
			p->cluster = i;
			p->distance = d;
		}
	}
}
void Kmeans::assignCluster_stable(RgbPixel* p, Clusters* clusters) {
	int i = 0;
	//int j = p->cluster;

	float d;
	d = euclideanDistance(p, &clusters->centroids[i]);
	p->distance = d;

	for (i = 1; i < clusters->k; ++i) {
		d = euclideanDistance(p, &clusters->centroids[i]);
		if (d < p->distance) {
			p->cluster = i;
			p->distance = d;
		}
	}
	//printf("before: %d, after: %d, stable: %d\n",j,p->cluster,p->stable);
	//if(p->cluster == j)
	//	p->stable++;
	//else
	//	p->stable=0;
}

int Kmeans::initClusters(Clusters* clusters, int k, float scale) {
	int i;
	float x;
	std::random_device rd;
	//std::random_device rd(__TIMESTAMP__);
	//clusters->centroids = (Centroid*)malloc(k * sizeof(Centroid));
	clusters->centroids = new Centroid[k];

	if (clusters == NULL) {
		printf("Warning: Oops! Cannot allocate memory for the clusters!\n");

		return 0;
	}

	clusters->k = k;
	for (i = 0; i < clusters->k; i++) {
		//x = (((float)rand()) / RAND_MAX) * scale;
		//clusters->centroids[i].r =x;

		//x = (((float)rand()) / RAND_MAX) * scale;
		//clusters->centroids[i].g = x;

		//x = (((float)rand()) / RAND_MAX) * scale;
		//clusters->centroids[i].b = x;
		x = (((uint32_t)rd()) *1.0 / (uint32_t)(std::numeric_limits<unsigned int>::max())) * scale;
		clusters->centroids[i].r =0.5;

		x = (((uint32_t)rd()) *1.0 / (uint32_t)(std::numeric_limits<unsigned int>::max())) * scale;
		clusters->centroids[i].g =  0.5;

		x = (((uint32_t)rd()) *1.0 / (uint32_t)(std::numeric_limits<unsigned int>::max())) * scale;
		clusters->centroids[i].b =  0.5;
		clusters->centroids[i].n = 0;
	}


	return 1;
}

void Kmeans::freeClusters(Clusters* clusters) {
	if (clusters != NULL)
		free(clusters->centroids);
}

void Kmeans::AssignCluster(RgbImage* image, Clusters* clusters) {
	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			assignCluster(&image->pixels[y][x], clusters);
		}
	}
}

//
void Kmeans::Recenter(RgbImage* image, Clusters* clusters) {
	
    //struct  timeval    tv;
	//struct  timezone   tz;
	//gettimeofday(&tv,&tz);
	//time_tag.push_back("Recenter start: "+std::to_string(tv.tv_sec)+"\t: "+std::to_string(tv.tv_usec));

	/** Recenter */
	for (int c = 0; c < clusters->k; ++c) {
		clusters->centroids[c].r = 0.;
		clusters->centroids[c].g = 0.;
		clusters->centroids[c].b = 0.;
		clusters->centroids[c].n = 0;
	}
	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			clusters->centroids[image->pixels[y][x].cluster].r += image->pixels[y][x].r;
			clusters->centroids[image->pixels[y][x].cluster].g += image->pixels[y][x].g;
			clusters->centroids[image->pixels[y][x].cluster].b += image->pixels[y][x].b;
			clusters->centroids[image->pixels[y][x].cluster].n += 1;
		}
	}
	for (int c = 0; c < clusters->k; ++c) {
		if (clusters->centroids[c].n != 0) {
			clusters->centroids[c].r = clusters->centroids[c].r / clusters->centroids[c].n;
			clusters->centroids[c].g = clusters->centroids[c].g / clusters->centroids[c].n;
			clusters->centroids[c].b = clusters->centroids[c].b / clusters->centroids[c].n;
		}
	}
	for (int c = 0; c < clusters->k; ++c) {
		SyncLogger::print(loopindicator, "clusters: ", c, " ", clusters->centroids[c].n, clusters->centroids[c].r, clusters->centroids[c].g, clusters->centroids[c].b);
		//ASyncLogger::print("clusters-" +std::to_string(c), " ", clusters->centroids[c].n, clusters->centroids[c].r, clusters->centroids[c].g, clusters->centroids[c].b);

	}
	//print_sum_to_DIST1(image,clusters); //print_sum//
	//double sum = print_sum_to_DIST1(image,clusters); //print_sum//
	//SyncLogger::print("DIST: ", sum);
	//gettimeofday(&tv,&tz);
	//time_tag.push_back("Recenter finish: "+std::to_string(tv.tv_sec)+"\t: "+std::to_string(tv.tv_usec));
}




void Kmeans::segmentImage(RgbImage* image, Clusters* clusters, int n) {
	for (int i = 0; i < n; ++i) {
		loopindicator = "loop" + std::to_string(i);
		AssignCluster(image, clusters);
		Recenter(image, clusters);
	}

	double sum = 0;
	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			sum += (pow(image->pixels[y][x].r - clusters->centroids[image->pixels[y][x].cluster].r, 2) +
				pow(image->pixels[y][x].g - clusters->centroids[image->pixels[y][x].cluster].g, 2) +
				pow(image->pixels[y][x].b - clusters->centroids[image->pixels[y][x].cluster].b, 2 ));
		}
	}
	SyncLogger::print("DIST: ", sum);

	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			image->pixels[y][x].r = clusters->centroids[image->pixels[y][x].cluster].r;
			image->pixels[y][x].g = clusters->centroids[image->pixels[y][x].cluster].g;
			image->pixels[y][x].b = clusters->centroids[image->pixels[y][x].cluster].b;
		}
	}
}


void Kmeans::initRgbImage(RgbImage* image) {
	image->w = 0;
	image->h = 0;
	image->pixels = NULL;
	image->meta = NULL;
	image->iteration = 0;
}

int Kmeans::readCell(FILE *fp, char* w) {
	int c;
	int i;

	w[0] = 0;
	for (c = fgetc(fp), i = 0; c != EOF; c = fgetc(fp)) {
		if (c == ' ' || c == '\t') {
			if (w[0] != '\"')
				continue;
		}

		if (c == ',' || c == '\n') {
			if (w[0] != '\"')
				break;
			else if (c == '\"') {
				w[i] = c;
				i++;
				break;
			}
		}

		w[i] = c;
		i++;
	}
	w[i] = 0;

	return c;
}

int Kmeans::loadRgbImage(const char* fileName, RgbImage* image, float scale) {
	int c;
	int i;
	int j;
	char w[256];
	RgbPixel** pixels;
	FILE *fp;

	//printf("Loading %s ...\n", fileName);
	//fopen(&fp, fileName, "r");
	fp = fopen(fileName, "r");
	if (!fp) {
		printf("Warning: Oops! Cannot open %s!\n", fileName);
		return 0;
	}

	c = readCell(fp, w);
	image->w = atoi(w);
	c = readCell(fp, w);
	image->h = atoi(w);
	image->size = image->w * image->h;
	image->pq = new int*[STABLE_TH+1];
	image->l = new int[STABLE_TH+1];
	image->r = new int[STABLE_TH+1];
	image->ql = image->size+1;
	for(int i=0; i<=STABLE_TH; i++)
	{
		image->pq[i] = new int[image->ql];
		image->l[i] = 1;
		image->r[i] = 0;
		if(i==0)
		{
			for(int j=0; j<image->size;j++)
				image->pq[0][j] = j;
			image->r[i] = image->size-1;
		}
	}

	//printf("%d x %d\n", image->w, image->h);

	pixels = (RgbPixel**)malloc(image->h * sizeof(RgbPixel*));
	//printf("h:%d\n",image->h);
	//pixels = new RgbPixel*[image->h];

	if (pixels == NULL) {
		printf("Warning: Oops! Cannot allocate memory for the pixels!2\n");

		fclose(fp);

		return 0;
	}

	c = 0;
	for (i = 0; i < image->h; i++) {
		pixels[i] = (RgbPixel*)malloc(image->w * sizeof(RgbPixel));
		if (pixels[i] == NULL) {
			c = 1;
			break;
		}
	}

	if (c == 1) {
		printf("Warning: Oops! Cannot allocate memory for the pixels!1\n");

		for (i--; i >= 0; i--)
			free(pixels[i]);
		free(pixels);

		fclose(fp);

		return 0;
	}

	for (i = 0; i < image->h; i++) {
		for (j = 0; j < image->w; j++) {
			c = readCell(fp, w);
			pixels[i][j].r = atoi(w) / scale;

			c = readCell(fp, w);
			pixels[i][j].g = atoi(w) / scale;

			c = readCell(fp, w);
			pixels[i][j].b = atoi(w) / scale;

			pixels[i][j].cluster = 0;
			pixels[i][j].distance = 0.;
			pixels[i][j].stable = 0;
			pixels[i][j].mark = 0;
		}
	}
	image->pixels = pixels;

	c = readCell(fp, w);
	image->meta = (char*)malloc(256);
	if (image->meta == NULL) {
		printf("Warning: Oops! Cannot allocate memory for the pixels!3\n");

		for (i = 0; i < image->h; i++)
			free(pixels[i]);
		free(pixels);

		fclose(fp);

		return 0;

	}
	strcpy(image->meta, w);

	//printf("%s\n", image->meta);

	//printf("w=%d x h=%d\n", image->w, image->h);

	return 1;
}

int Kmeans::saveRgbImage(RgbImage* image, const char* fileName, float scale) {
	int i;
	int j;
	FILE *fp;

	//printf("Saving %s ...\n", fileName);

	fp = fopen(fileName, "w");
	if (!fp) {
		printf("Warning: Oops! Cannot open %s!\n", fileName);
		return 0;
	}

	fprintf(fp, "%d,%d\n", image->w, image->h);
	//printf("%d,%d\n", image->w, image->h);

	for (i = 0; i < image->h; i++) {
		for (j = 0; j < (image->w - 1); j++) {
			fprintf(fp, "%d,%d,%d,", int(image->pixels[i][j].r * scale), int(image->pixels[i][j].g * scale), int(image->pixels[i][j].b * scale));
		}
		fprintf(fp, "%d,%d,%d\n", int(image->pixels[i][j].r * scale), int(image->pixels[i][j].g * scale), int(image->pixels[i][j].b * scale));
	}

	fprintf(fp, "%s", image->meta);
	//printf("%s\n", image->meta);

	fclose(fp);

	return 1;
}

void Kmeans::freeRgbImage(RgbImage* image) {
	int i;

	if (image->meta != NULL)
		free(image->meta);

	if (image->pixels == NULL)
		return;

	for (i = 0; i < image->h; i++)
		if (image->pixels != NULL)
			free(image->pixels[i]);

	free(image->pixels);
}

void Kmeans::makeGrayscale(RgbImage* image) {
	int i;
	int j;
	float luminance;

	float rC = (float)0.30;
	float gC = (float)0.59;
	float bC = (float)0.11;

	for (i = 0; i < image->h; i++) {
		for (j = 0; j < image->w; j++) {
			luminance =
				rC * image->pixels[i][j].r +
				gC * image->pixels[i][j].g +
				bC * image->pixels[i][j].b;

			image->pixels[i][j].r = (unsigned char)luminance;
			image->pixels[i][j].g = (unsigned char)luminance;
			image->pixels[i][j].b = (unsigned char)luminance;
		}
	}
}

#define _CRT_RAND_S

static int RangedRand(int range_min, int range_max)
{
	// Generate random numbers in the half-closed interval  
	// [range_min, range_max). In other words,  
	// range_min <= random number < range_max  

		return (double)rand() / (RAND_MAX + 1) * (range_max - range_min)
			+ range_min;
	
}

#include <numeric>
#include <cassert>
#include <algorithm>
#include <random>
#include <thread>
//for single stread non-STABLE
void KmeansRandom::AssignCluster(RgbImage* image, Clusters* clusters, int* counter) {
	//std::random_device rd0();
    //struct  timeval    tv;
	//struct  timezone   tz;
	//gettimeofday(&tv,&tz);
	//time_tag.push_back("AssignCluster start: "+std::to_string(tv.tv_sec)+"\t: "+std::to_string(tv.tv_usec));

	std::stringstream ss;
	printf("KmeansRandom::AssignCluster\n");
	//std::vector<uint32_t> pixelset(image->h * image->w);
	//std::iota(std::begin(pixelset), std::end(pixelset), 0);
	//for (int i = 0; i < image->h * image->w; ++i)
	//	pixelset.insert(pixelset.end(), i);
	//std::random_shuffle(pixelset.begin(), pixelset.end());
	//ss << std::chrono::system_clock::now();
	//srand((unsigned)time(NULL));
	srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			
			//unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
			//std::mt19937 g1(seed1);  // mt19937 is a standard mersenne_twister_engine
		     uint32_t randg1 = StoppableThread::random() % (image->h * image->w);
			

			//assert(pixelid != pixelset.end());
			uint32_t tt = randg1;
			uint32_t xx = tt%image->w;
			
			uint32_t yy = tt / image->w;
			//pixelset.erase(pixelset.begin() + randg1);
			assert(xx < image->w && yy < image->h);
			assignCluster(&image->pixels[yy][xx], clusters);
			(*counter)++;
		}
		//SyncLogger::print("AssignCluster", y);
	}
	//gettimeofday(&tv,&tz);
	//time_tag.push_back("AssignCluster finish: "+std::to_string(tv.tv_sec)+"\t: "+std::to_string(tv.tv_usec));
}

//void KmeansRandom::AssignCluster2_threads(RgbImage* image, Clusters* clusters, std::vector<int>* counter, int thread_id) {
void AssignCluster2_threads(KmeansRandom* kr, RgbImage* image, Clusters* clusters, int num_thread, int thread_id) {
	srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	//int num_thread = counter->size();
	std::random_device rdrd;
	for (int x = 0; x < image->w*image->h/num_thread; x++) {
	//for (int x = thread_id; x < image->w*image->h; x+=num_thread) {
		
		//unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
		//std::mt19937 g1(seed1);  // mt19937 is a standard mersenne_twister_engine
	     //uint32_t randg1 = StoppableThread::random() % (image->h * image->w);
	     uint32_t randg1 = rdrd() % (image->h * image->w);
	     randg1 = (randg1/num_thread)*num_thread + thread_id;
	     if(randg1>image->h * image->w) randg1-=num_thread;
		
		//uint32_t randg1 = x;
		

		//assert(pixelid != pixelset.end());
		uint32_t tt = randg1;
		uint32_t xx = tt%image->w;
		
		uint32_t yy = tt / image->w;
		//pixelset.erase(pixelset.begin() + randg1);
		//printf("xx: %d w: %d, yy: %d, h: %d, randg1: %d, thread_id: %d\n", xx, image->w, yy, image->h, randg1, thread_id);
		assert(xx < image->w && yy < image->h);
		kr->assignCluster(&image->pixels[yy][xx], clusters);
		//(*counter)[thread_id]++;
	}
	//SyncLogger::print("AssignCluster", y);
}

//for multi stread non-STABLE
void KmeansRandom::AssignCluster2P(RgbImage* image, Clusters* clusters, int num_thread, int* counter) {

	printf("KmeansRandom::AssignCluster2\n");
	std::thread* threads = new std::thread[num_thread-1];
	for(int i=1; i<num_thread; i++) {
		threads[i-1] = std::thread(AssignCluster2_threads, this, image, clusters, num_thread, i);
	}
	srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	//int num_thread = counter->size();
	std::random_device rdrd;
	int thread_id=0;
	//for (int x = 0; x < image->w*image->h; x+=num_thread) {
	for (int x = 0; x < image->w*image->h/num_thread; x++) {
		
		//unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
		//std::mt19937 g1(seed1);  // mt19937 is a standard mersenne_twister_engine
	     //uint32_t randg1 = StoppableThread::random() % (image->h * image->w);
	     
	     uint32_t randg1 = rdrd() % (image->h * image->w);
	     randg1 = (randg1/num_thread)*num_thread + thread_id;
	     if(randg1>image->h * image->w) randg1-=num_thread;
		//uint32_t randg1 = x;

		

		//assert(pixelid != pixelset.end());
		uint32_t tt = randg1;
		uint32_t xx = tt%image->w;
		
		uint32_t yy = tt / image->w;
		//pixelset.erase(pixelset.begin() + randg1);
		//printf("xx: %d w: %d, yy: %d, h: %d, randg1: %d, thread_id: %d\n", xx, image->w, yy, image->h, randg1, thread_id);
		assert(xx < image->w && yy < image->h);
		this->assignCluster(&image->pixels[yy][xx], clusters);
		(*counter)++;
	}
    for(int i=1; i<num_thread; i++) {
        threads[i-1].join();
    }
}

//for multi stread non-STABLE
void KmeansRandom::AssignCluster2(RgbImage* image, Clusters* clusters, int num_thread, int thread_id, std::vector<int>* counter) {

	//printf("KmeansRandom::AssignCluster2\n");
	///std::thread* threads = new std::thread[num_thread-1];
	///for(int i=1; i<num_thread; i++) {
	///	threads[i-1] = std::thread(AssignCluster2_threads, this, image, clusters, num_thread, i);
	///}
	//srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	//int num_thread = counter->size();
	//std::random_device rdrd;
	//int thread_id=0;
	//for (int x = 0; x < image->w*image->h; x+=num_thread) {
	for (int x = 0; x < image->w*image->h/num_thread; x++) {
		
		//unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
		//std::mt19937 g1(seed1);  // mt19937 is a standard mersenne_twister_engine
	     //uint32_t randg1 = StoppableThread::random() % (image->h * image->w);
	     
	     //uint32_t randg1 = rdrd() % (image->h * image->w);
	     uint32_t randg1 = StoppableThread::random() % (image->h * image->w);
	     randg1 = (randg1/num_thread)*num_thread + thread_id;
	     if(randg1>image->h * image->w) randg1-=num_thread;
		//uint32_t randg1 = x;

		

		//assert(pixelid != pixelset.end());
		uint32_t tt = randg1;
		uint32_t xx = tt%image->w;
		
		uint32_t yy = tt / image->w;
		//pixelset.erase(pixelset.begin() + randg1);
		//printf("xx: %d w: %d, yy: %d, h: %d, randg1: %d, thread_id: %d\n", xx, image->w, yy, image->h, randg1, thread_id);
		assert(xx < image->w && yy < image->h);
		RgbPixel *p = &image->pixels[yy][xx];
		this->assignCluster(&image->pixels[yy][xx], clusters);
		(*counter)[thread_id]++;
	}
	//std::cout << "KmeansRandom::AssignCluster2 counter:" << counter << std::endl;
    ///for(int i=1; i<num_thread; i++) {
    ///    threads[i-1].join();
    ///}
}

//for single stread STABLE
void KmeansRandom::AssignCluster_stable(RgbImage* image, Clusters* clusters, int iter, int* counter) {
	//std::random_device rd0();
	image->iteration++;
	std::stringstream ss;
	//printf("KmeansRandom::AssignCluster_stable\n");
	//std::vector<uint32_t> pixelset(image->h * image->w);
	//std::iota(std::begin(pixelset), std::end(pixelset), 0);
	//for (int i = 0; i < image->h * image->w; ++i)
	//	pixelset.insert(pixelset.end(), i);
	//std::random_shuffle(pixelset.begin(), pixelset.end());
	//ss << std::chrono::system_clock::now();
	//srand((unsigned)time(NULL));
	//auto rrr= rand();
	srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			
			//unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
			//std::mt19937 g1(seed1);  // mt19937 is a standard mersenne_twister_engine
		     uint32_t randg1 = StoppableThread::random();
			

			//assert(pixelid != pixelset.end());
			uint32_t tt = randg1 % (image->h * image->w);
			uint32_t xx = tt%image->w;
			
			uint32_t yy = tt / image->w;
			//pixelset.erase(pixelset.begin() + randg1);
			assert(xx < image->w && yy < image->h);
			RgbPixel *p = &image->pixels[yy][xx];
			//printf("p->stable:%d\n", p->stable);
			//if((p->stable >= STABLE_DI)||((p->stable & (STABLE_TH - 1)) == 0))
			//if(p->mark < image->iteration) {
			//if((p->stable <= STABLE_TH)||(((tt/image->h/image->w) & (STABLE_TH - 1)) == 0))
			//if((p->stable <= STABLE_TH)||((tt/image->size) % (p->stable + 1) == 0)) {
			if((p->stable <= STABLE_TH)||((randg1/image->size) % 7 < 3)) {
				auto j=p->cluster;
				assignCluster_stable(p, clusters); //printf("p->stable:%d\n", p->stable);}
				if(p->cluster == j)
					p->stable++;
				else
					p->stable=0;
			}
			else
				p->stable ++;
			//	p->mark = image->iteration;
			//}
			(*counter)++;
		}
		//SyncLogger::print("AssignCluster", y);
	}
}


//for single stread STABLE
void KmeansRandom::AssignCluster_sb(RgbImage* image, Clusters* clusters, int iter, int* stable) {
	//std::random_device rd0();
    //struct  timeval    tv;
	//struct  timezone   tz;
	//gettimeofday(&tv,&tz);
	//time_tag.push_back("AssignCluster start: "+std::to_string(tv.tv_sec)+"\t: "+std::to_string(tv.tv_usec));
	image->iteration++;
	std::stringstream ss;
	//printf("KmeansRandom::AssignCluster_stable\n");
	//std::vector<uint32_t> pixelset(image->h * image->w);
	//std::iota(std::begin(pixelset), std::end(pixelset), 0);
	//for (int i = 0; i < image->h * image->w; ++i)
	//	pixelset.insert(pixelset.end(), i);
	//std::random_shuffle(pixelset.begin(), pixelset.end());
	//ss << std::chrono::system_clock::now();
	//srand((unsigned)time(NULL));
	//auto rrr= rand();
	srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			
			//unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
			//std::mt19937 g1(seed1);  // mt19937 is a standard mersenne_twister_engine
		     uint32_t randg1 = StoppableThread::random();
			

			//assert(pixelid != pixelset.end());
			uint32_t tt = randg1 % (image->h * image->w);
			uint32_t xx = tt%image->w;
			
			uint32_t yy = tt / image->w;
			//pixelset.erase(pixelset.begin() + randg1);
			assert(xx < image->w && yy < image->h);
			RgbPixel *p = &image->pixels[yy][xx];
			//printf("p->stable:%d\n", p->stable);
			//if((p->stable >= STABLE_DI)||((p->stable & (STABLE_TH - 1)) == 0))
			//if(p->mark < image->iteration) {
			//if((p->stable <= STABLE_TH)||(((tt/image->h/image->w) & (STABLE_TH - 1)) == 0))
			//if((p->stable <= STABLE_TH)||((tt/image->size) % (p->stable + 1) == 0)) {
			//if((p->stable <= STABLE_TH)||((randg1/image->size) % 7 < 3)) {
			auto j=p->cluster;
			assignCluster_stable(p, clusters); //printf("p->stable:%d\n", p->stable);}
			if(p->cluster == j)
				p->stable++;
			else
				p->stable=0;
			//if(p->stable > STABLE_TH) (*stable)++;
			//	p->mark = image->iteration;
			//}
			//(*counter)++;
		}
		//SyncLogger::print("AssignCluster", y);
	}
	//(*stable) = image->size;
	//gettimeofday(&tv,&tz);
	//time_tag.push_back("AssignCluster finish: "+std::to_string(tv.tv_sec)+"\t: "+std::to_string(tv.tv_usec));
}

//void KmeansRandom::AssignCluster2_threads(RgbImage* image, Clusters* clusters, std::vector<int>* counter, int thread_id) {
void AssignCluster3_threads(KmeansRandom* kr, RgbImage* image, Clusters* clusters, int num_thread, int thread_id) {
	srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	std::random_device rdrd;
	for (int x = 0; x < image->w*image->h/num_thread; x++) {
	//for (int x = thread_id; x < image->w*image->h; x+=num_thread) {
		
		//unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
		//std::mt19937 g1(seed1);  // mt19937 is a standard mersenne_twister_engine
	     //uint32_t randg1 = StoppableThread::random() % (image->h * image->w);
	     uint32_t randg1 = rdrd() % (image->h * image->w);
	     randg1 = (randg1/num_thread)*num_thread + thread_id;
	     if(randg1>image->h * image->w) randg1-=num_thread;
		
		//uint32_t randg1 = x;
		

		//assert(pixelid != pixelset.end());
		uint32_t tt = randg1;
		uint32_t xx = tt%image->w;
		
		uint32_t yy = tt / image->w;
		//pixelset.erase(pixelset.begin() + randg1);
		//printf("xx: %d w: %d, yy: %d, h: %d, randg1: %d, thread_id: %d\n", xx, image->w, yy, image->h, randg1, thread_id);
		assert(xx < image->w && yy < image->h);
		RgbPixel *p = &image->pixels[yy][xx];
		//if((p->stable <= STABLE_TH)||(((tt/image->h/image->w) & (STABLE_TH - 1)) == 0))
		//	kr->assignCluster_stable(p, clusters);
		//else
		//	p->stable++;
		if((p->stable <= STABLE_TH)||((randg1/image->size) % 2 != 0)) {
			auto j=p->cluster;
			kr->assignCluster_stable(p, clusters); //printf("p->stable:%d\n", p->stable);}
			if(p->cluster == j)
				p->stable++;
			else
				p->stable=0;
		}
		else
			p->stable ++;
		//(*counter)[thread_id]++;
	}
	//SyncLogger::print("AssignCluster", y);
}

//for multi stread STABLE
//void KmeansRandom::AssignCluster2_stable(RgbImage* image, Clusters* clusters, int num_thread, int* counter) {
void KmeansRandom::AssignCluster2_stable(RgbImage* image, Clusters* clusters, int num_thread, int thread_id, std::vector<int>* counter) {

	//printf("KmeansRandom::AssignCluster2_stable\n");
	//image->iteration++;
	//std::thread* threads = new std::thread[num_thread-1];
	//for(int i=1; i<num_thread; i++) {
	//	threads[i-1] = std::thread(AssignCluster3_threads, this, image, clusters, num_thread, i);
	//}
	srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	//std::random_device rdrd;
	//int thread_id=0;
	//for (int x = 0; x < image->w*image->h; x+=num_thread) {
	for (int x = 0; x < image->w*image->h/num_thread; x++) {
		
		//unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
		//std::mt19937 g1(seed1);  // mt19937 is a standard mersenne_twister_engine
	     //uint32_t randg1 = StoppableThread::random() % (image->h * image->w);
	     
	     //uint32_t randg1 = rdrd();
	     uint32_t randg1 = StoppableThread::random();
	     uint32_t tt = randg1 % (image->h * image->w);
	     tt = (tt/num_thread)*num_thread + thread_id;
	     if(tt>image->h * image->w) tt-=num_thread;
		//uint32_t randg1 = x;

		

		//assert(pixelid != pixelset.end());
		//uint32_t tt = randg1;
		uint32_t xx = tt%image->w;
		
		uint32_t yy = tt / image->w;
		//pixelset.erase(pixelset.begin() + randg1);
		//printf("xx: %d w: %d, yy: %d, h: %d, randg1: %d, thread_id: %d\n", xx, image->w, yy, image->h, randg1, thread_id);
		assert(xx < image->w && yy < image->h);
		RgbPixel *p = &image->pixels[yy][xx];
		//if((p->stable <= STABLE_TH)||(((tt/image->h/image->w) & (STABLE_TH - 1)) == 0))
		//	this->assignCluster_stable(p, clusters);
		//else
		//	p->stable++;
		//(*counter)++;
		if((p->stable <= STABLE_TH)||((randg1/image->size) % 7 < 3)) {
			auto j=p->cluster;
			assignCluster_stable(p, clusters); //printf("p->stable:%d\n", p->stable);}
			if(p->cluster == j)
				p->stable++;
			else
				p->stable=0;
		}
		else
			p->stable ++;
		(*counter)[thread_id]++;
	}
    //for(int i=1; i<num_thread; i++) {
    //    threads[i-1].join();
    //}
}

//for multi stread STABLE
//void KmeansRandom::AssignCluster2_stable(RgbImage* image, Clusters* clusters, int num_thread, int* counter) {
void KmeansRandom::AssignCluster2_sb(RgbImage* image, Clusters* clusters, int num_thread, int thread_id, std::vector<int>* counter) {

	//printf("KmeansRandom::AssignCluster2_stable\n");
	image->iteration++;
	//std::thread* threads = new std::thread[num_thread-1];
	//for(int i=1; i<num_thread; i++) {
	//	threads[i-1] = std::thread(AssignCluster3_threads, this, image, clusters, num_thread, i);
	//}
	srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	//std::random_device rdrd;
	//int thread_id=0;
	//for (int x = 0; x < image->w*image->h; x+=num_thread) {
	for (int x = 0; x < image->w*image->h/num_thread; x++) {
		
		//unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
		//std::mt19937 g1(seed1);  // mt19937 is a standard mersenne_twister_engine
	     //uint32_t randg1 = StoppableThread::random() % (image->h * image->w);
	     
	     //uint32_t randg1 = rdrd();
	     uint32_t randg1 = StoppableThread::random();
	     uint32_t tt = randg1 % (image->h * image->w);
	     tt = (tt/num_thread)*num_thread + thread_id;
	     if(tt>image->h * image->w) tt-=num_thread;
		//uint32_t randg1 = x;

		

		//assert(pixelid != pixelset.end());
		//uint32_t tt = randg1;
		uint32_t xx = tt%image->w;
		
		uint32_t yy = tt / image->w;
		//pixelset.erase(pixelset.begin() + randg1);
		//printf("xx: %d w: %d, yy: %d, h: %d, randg1: %d, thread_id: %d\n", xx, image->w, yy, image->h, randg1, thread_id);
		assert(xx < image->w && yy < image->h);
		RgbPixel *p = &image->pixels[yy][xx];
		//if((p->stable <= STABLE_TH)||(((tt/image->h/image->w) & (STABLE_TH - 1)) == 0))
		//	this->assignCluster_stable(p, clusters);
		//else
		//	p->stable++;
		//(*counter)++;
		/*
		if((p->stable <= STABLE_TH)||((randg1/image->size) % 7 < 3)) {
			auto j=p->cluster;
			assignCluster_stable(p, clusters); //printf("p->stable:%d\n", p->stable);}
			if(p->cluster == j)
				p->stable++;
			else
				p->stable=0;
		}
		else
			p->stable ++;
		(*counter)[thread_id]++;*/
		auto j=p->cluster;
		assignCluster_stable(p, clusters); //printf("p->stable:%d\n", p->stable);}
		if(p->cluster == j)
			p->stable++;
		else
			p->stable=0;
	}
    //for(int i=1; i<num_thread; i++) {
    //    threads[i-1].join();
    //}
}

//for single stread priority queue
void KmeansRandom::AssignCluster_pq(RgbImage* image, Clusters* clusters, int iter, int* counter) {
	//std::random_device rd0();
	image->iteration++;
	std::stringstream ss;
	//printf("KmeansRandom::AssignCluster_stable\n");
	//std::vector<uint32_t> pixelset(image->h * image->w);
	//std::iota(std::begin(pixelset), std::end(pixelset), 0);
	//for (int i = 0; i < image->h * image->w; ++i)
	//	pixelset.insert(pixelset.end(), i);
	//std::random_shuffle(pixelset.begin(), pixelset.end());
	//ss << std::chrono::system_clock::now();
	//srand((unsigned)time(NULL));
	int cc=0;
	srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			cc=0;
			for(int i=0;i<=STABLE_TH;i++)
				cc+=(STABLE_TH+1-i)*((image->ql+image->r[i]-image->l[i])%image->ql);
			//unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
			//std::mt19937 g1(seed1);  // mt19937 is a standard mersenne_twister_engine
		    //uint32_t randg1 = StoppableThread::random() % (image->h * image->w);
		    uint32_t randg1 = StoppableThread::random() % cc;
		    uint32_t randg2 = randg1;
			
		    int i=0;
		    while(randg1>0)
		    {
		    	if(randg1 < (STABLE_TH+1-i)*((image->ql+image->r[i]-image->l[i])%image->ql)) break;
		    	randg1-=(STABLE_TH+1-i)*((image->ql+image->r[i]-image->l[i])%image->ql);
		    	i++;
		    }
		    i = i % STABLE_TH;
		    //printf("%d\n", i);
		    //while((image->r[i] + 1)%image->ql == image->l[i]) i=(i+STABLE_TH-1)%STABLE_TH;
			//assert(pixelid != pixelset.end());
		    	/*
			if(!(i<=STABLE_TH && i>=0)) 
			{
				printf("i=%d, x=%d, y=%d, rand==%d, remain=%d\n", i,x,y,randg2,randg1);
				for(int t=0;t<=STABLE_TH;t++)
					//printf("%d possibility: %d\n",t,((image->size+image->r[t]-image->l[t])%image->size));
					printf("%d possibility: %d\n",t,(STABLE_TH+1-t)*((image->size+image->r[t]-image->l[t])%image->size));
			cc=0;
			for(int i=0;i<=STABLE_TH;i++)
			{
				cc+=(STABLE_TH+1-i)*((image->size+image->r[i]-image->l[i])%image->size);
				printf("%d possibility: %d\n",i,(STABLE_TH+1-i)*((image->size+image->r[i]-image->l[i])%image->size));
				//cc+=((image->size+image->r[i]-image->l[i])%image->size);
			}
			printf("cc=%d\n",cc);
			}
			*/
			assert(i<=STABLE_TH && i>=0);
			uint32_t tt = image->pq[i][image->l[i]];
			image->l[i] = (image->l[i]+1)%image->ql;
			uint32_t xx = tt%image->w;
			
			uint32_t yy = tt / image->w;
			//pixelset.erase(pixelset.begin() + randg1);
			assert(xx < image->w && yy < image->h);
			RgbPixel *p = &image->pixels[yy][xx];
			//printf("p->stable:%d\n", p->stable);
			//if((p->stable >= STABLE_DI)||((p->stable & (STABLE_TH - 1)) == 0))
			//if((p->stable < STABLE_TH)||((p->stable & (STABLE_TH - 1)) == 0))
			//{
				assignCluster_stable(p, clusters); //printf("p->stable:%d\n", p->stable);}
				int ps=p->stable;
				if(ps < STABLE_TH)
				{
					//assert(ps < STABLE_TH);
					image->r[ps] = (image->r[ps]+1)%image->ql;
					//if(!(image->r[ps] < image->ql)) printf("%d,%d,%d,%d,%d\n",image->r[ps],image->ql,image->size,ps,p->stable);
					//assert(image->r[ps] < image->ql);
					image->pq[ps][image->r[ps]] = tt;
				}
				else
				{
					image->r[STABLE_TH] = (image->r[STABLE_TH]+1)%image->ql;
					//assert(image->r[STABLE_TH] < image->ql);
					image->pq[STABLE_TH][image->r[STABLE_TH]] = tt;
				}
			//}
			/*
			else
			{
				p->stable ++;
				image->r[STABLE_TH] = (image->r[STABLE_TH]+1)%image->ql;
				image->pq[STABLE_TH][image->r[STABLE_TH]] = tt;
			}*/
			(*counter)++;
		}
		//SyncLogger::print("AssignCluster", y);
	}
}

//void KmeansRandom::AssignCluster2_threads(RgbImage* image, Clusters* clusters, std::vector<int>* counter, int thread_id) {
void AssignCluster4_threads(KmeansRandom* kr, RgbImage* image, Clusters* clusters, std::vector<int>* counter, int thread_id) {
	srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	int num_thread = counter->size();
	std::random_device rdrd;
	for (int x = 0; x < image->w*image->h/num_thread; x++) {
	//for (int x = thread_id; x < image->w*image->h; x+=num_thread) {
		
		//unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
		//std::mt19937 g1(seed1);  // mt19937 is a standard mersenne_twister_engine
	     //uint32_t randg1 = StoppableThread::random() % (image->h * image->w);
	     uint32_t randg1 = rdrd() % (image->h * image->w);
	     randg1 = (randg1/num_thread)*num_thread + thread_id;
	     if(randg1>image->h * image->w) randg1-=num_thread;
		
		//uint32_t randg1 = x;
		

		//assert(pixelid != pixelset.end());
		uint32_t tt = randg1;
		uint32_t xx = tt%image->w;
		
		uint32_t yy = tt / image->w;
		//pixelset.erase(pixelset.begin() + randg1);
		//printf("xx: %d w: %d, yy: %d, h: %d, randg1: %d, thread_id: %d\n", xx, image->w, yy, image->h, randg1, thread_id);
		assert(xx < image->w && yy < image->h);
		RgbPixel *p = &image->pixels[yy][xx];
		if((p->stable <= STABLE_TH)||((p->stable & (STABLE_TH - 1)) == 0))
			kr->assignCluster_stable(p, clusters);
		else
			p->stable++;
		(*counter)[thread_id]++;
	}
	//SyncLogger::print("AssignCluster", y);
}

//for multi stread priority queue
void KmeansRandom::AssignCluster2_pq(RgbImage* image, Clusters* clusters, int num_thread, int *counter) {

	//printf("KmeansRandom::AssignCluster2_stable\n");
	image->iteration++;
	std::thread* threads = new std::thread[num_thread-1];
	for(int i=1; i<num_thread; i++) {
		threads[i-1] = std::thread(AssignCluster3_threads, this, image, clusters, num_thread, i);
	}
	srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	//int num_thread = counter->size();
	std::random_device rdrd;
	int thread_id=0;
	//for (int x = 0; x < image->w*image->h; x+=num_thread) {
	for (int x = 0; x < image->w*image->h/num_thread; x++) {
		
		//unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
		//std::mt19937 g1(seed1);  // mt19937 is a standard mersenne_twister_engine
	     //uint32_t randg1 = StoppableThread::random() % (image->h * image->w);
	     
	     uint32_t randg1 = rdrd() % (image->h * image->w);
	     randg1 = (randg1/num_thread)*num_thread + thread_id;
	     if(randg1>image->h * image->w) randg1-=num_thread;
		//uint32_t randg1 = x;

		

		//assert(pixelid != pixelset.end());
		uint32_t tt = randg1;
		uint32_t xx = tt%image->w;
		
		uint32_t yy = tt / image->w;
		//pixelset.erase(pixelset.begin() + randg1);
		//printf("xx: %d w: %d, yy: %d, h: %d, randg1: %d, thread_id: %d\n", xx, image->w, yy, image->h, randg1, thread_id);
		assert(xx < image->w && yy < image->h);
		RgbPixel *p = &image->pixels[yy][xx];
		if((p->stable <= STABLE_TH)||((p->stable & (STABLE_TH - 1)) == 0))
			this->assignCluster_stable(p, clusters);
		else
			p->stable++;
		(*counter)++;
	}
    for(int i=1; i<num_thread; i++) {
        threads[i-1].join();
    }
}
void KmeansRandom::segmentImage(RgbImage* image, Clusters* clusters, int n) {
	double pre = 100000000.0;
	double sum = 0.0;
	for (int i = 0; i < n; ++i) {
		loopindicator = "loop" + std::to_string(i);
		int* ttt = new int(0);
		AssignCluster(image, clusters, ttt);
		Recenter(image, clusters);
		//sum = print_sum_to_DIST1(image, clusters);
		//if(sum > pre - 0.01)
		//{
		//	std::cout << "terminate at iter: " << i << std::endl;
		//	break;
		//}
		//pre = sum;
	}
	sum = 0;
	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			sum += (pow(image->pixels[y][x].r - clusters->centroids[image->pixels[y][x].cluster].r, 2) +
				pow(image->pixels[y][x].g - clusters->centroids[image->pixels[y][x].cluster].g, 2) +
				pow(image->pixels[y][x].b - clusters->centroids[image->pixels[y][x].cluster].b, 2));
		}
	}
	SyncLogger::print("DIST: ", sum);
	this->sum = sum;
	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			image->pixels[y][x].r = clusters->centroids[image->pixels[y][x].cluster].r;
			image->pixels[y][x].g = clusters->centroids[image->pixels[y][x].cluster].g;
			image->pixels[y][x].b = clusters->centroids[image->pixels[y][x].cluster].b;
		}
	}
}
void KmeansRandomP::segmentImage(RgbImage* image, Clusters* clusters, int n) {
	//std::vector<int>* ttt = new std::vector<int>;
	//ttt->push_back(0);
	//ttt->insert(ttt->begin(), this->thread_num-1, 0);
	int *ttt = new int(0);
	for (int i = 0; i < n; ++i) {
		loopindicator = "loop" + std::to_string(i);
		//AssignCluster2(image, clusters, ttt);
		AssignCluster2P(image, clusters, this->thread_num, ttt);
		//Recenter2(image, clusters, this->thread_num);
		Recenter(image, clusters);
	}
	double sum = 0;
	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			sum += (pow(image->pixels[y][x].r - clusters->centroids[image->pixels[y][x].cluster].r, 2) +
				pow(image->pixels[y][x].g - clusters->centroids[image->pixels[y][x].cluster].g, 2) +
				pow(image->pixels[y][x].b - clusters->centroids[image->pixels[y][x].cluster].b, 2));
		}
	}
	SyncLogger::print("DIST: ", sum);
	this->sum = sum;
	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			image->pixels[y][x].r = clusters->centroids[image->pixels[y][x].cluster].r;
			image->pixels[y][x].g = clusters->centroids[image->pixels[y][x].cluster].g;
			image->pixels[y][x].b = clusters->centroids[image->pixels[y][x].cluster].b;
		}
	}
}

void KmeansRandomChangedRecenter::Recenter(RgbImage* image, Clusters* clusters) {
	printf("KmeansRandomChangedRecenter::Recenter\n");

    //struct  timeval    tv;
	//struct  timezone   tz;
	//gettimeofday(&tv,&tz);
	//time_tag.push_back("Recenter start: "+std::to_string(tv.tv_sec)+"\t: "+std::to_string(tv.tv_usec));

	auto r = new double[clusters->k];
	auto g = new double[clusters->k];
	auto b = new double[clusters->k];
	auto n = new int[clusters->k];
	//printf("shi zhege me??????\n");
	for (int c = 0; c < clusters->k; ++c) {
		r[c] = 0.;
		g[c] = 0.;
		b[c] = 0.;
		n[c] = 0;
	}

	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			r[image->pixels[y][x].cluster] += image->pixels[y][x].r;
			g[image->pixels[y][x].cluster] += image->pixels[y][x].g;
			b[image->pixels[y][x].cluster] += image->pixels[y][x].b;
			n[image->pixels[y][x].cluster] += 1;
		}
	}
	for (int c = 0; c < clusters->k; ++c) {
		SyncLogger::print(loopindicator, "clusters-NC" + std::to_string(c), " ", n[c]);

		clusters->centroids[c].n = n[c];
	}
	for (int c = 0; c < clusters->k; ++c) {
		if (n[c] == 0) {
			clusters->centroids[c].r = r[c];
			clusters->centroids[c].g = g[c];
			clusters->centroids[c].b = b[c];

		}
		else if (n[c] != 0) {

			clusters->centroids[c].r = r[c] / n[c];
			clusters->centroids[c].g = g[c] / n[c];
			clusters->centroids[c].b = b[c] / n[c];

		}
	}
	for (int c = 0; c < clusters->k; ++c) {
		//SyncLogger::print("clusters: ", c, " ", clusters->centroids[c].n, clusters->centroids[c].r, clusters->centroids[c].g, clusters->centroids[c].b);
		SyncLogger::print(loopindicator, "clusters-FN" + std::to_string(c), " ", n[c], clusters->centroids[c].n, clusters->centroids[c].r, clusters->centroids[c].g, clusters->centroids[c].b);

	}
	delete[] r;
	delete[] g;
	delete[] b;
	delete[] n;

	//gettimeofday(&tv,&tz);
	//time_tag.push_back("Recenter finish: "+std::to_string(tv.tv_sec)+"\t: "+std::to_string(tv.tv_usec));
	//double sum = print_sum_to_DIST1(image,clusters); //print_sum//
	//SyncLogger::print("DIST: ", sum);
}

void Recenter2_threads(RgbImage* image, Clusters* clusters, int thread_id, int num_thread, double* r, double* g, double* b, int* n) {
	r = &r[thread_id*clusters->k];
	g = &g[thread_id*clusters->k];
	b = &b[thread_id*clusters->k];
	n = &n[thread_id*clusters->k];
	for (int c = 0; c < clusters->k; ++c) {
		r[c] = 0.;
		g[c] = 0.;
		b[c] = 0.;
		n[c] = 0;
	}

	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x+=num_thread) {
			r[image->pixels[y][x].cluster] += image->pixels[y][x].r;
			g[image->pixels[y][x].cluster] += image->pixels[y][x].g;
			b[image->pixels[y][x].cluster] += image->pixels[y][x].b;
			n[image->pixels[y][x].cluster] += 1;
		}
	}
}
void KmeansRandomChangedRecenter::Recenter2(RgbImage* image, Clusters* clusters, int num_thread) {
	auto r = new double[num_thread*clusters->k];
	auto g = new double[num_thread*clusters->k];
	auto b = new double[num_thread*clusters->k];
	auto n = new int[num_thread*clusters->k];

	//auto r = new double[clusters->k];
	//auto g = new double[clusters->k];
	//auto b = new double[clusters->k];
	//auto n = new int[clusters->k];
	std::thread* threads = new std::thread[num_thread-1];
	for(int i=1; i<num_thread; i++) {
		threads[i-1] = std::thread(Recenter2_threads, image, clusters, i, num_thread, r, g, b, n);
	}

	for (int c = 0; c < clusters->k; ++c) {
		r[c] = 0.;
		g[c] = 0.;
		b[c] = 0.;
		n[c] = 0;
	}

	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x+=num_thread) {
			r[image->pixels[y][x].cluster] += image->pixels[y][x].r;
			g[image->pixels[y][x].cluster] += image->pixels[y][x].g;
			b[image->pixels[y][x].cluster] += image->pixels[y][x].b;
			n[image->pixels[y][x].cluster] += 1;
		}
	}
	for (int c = 0; c < clusters->k; ++c) {
		//SyncLogger::print(loopindicator, "clusters-NC" + std::to_string(c), " ", n[c]);
		clusters->centroids[c].n = n[c];
		clusters->centroids[c].r = r[c];
		clusters->centroids[c].g = g[c];
		clusters->centroids[c].b = b[c];
	}

    	for(int i=1; i<num_thread; i++) {
            threads[i-1].join();
    	}
	for(int i = 1; i < num_thread; i++) {
	    for (int c = 0; c < clusters->k; ++c) {
		//SyncLogger::print(loopindicator, "clusters-NC" + std::to_string(c), " ", n[c]);
		clusters->centroids[c].n += n[i*num_thread+c];
		clusters->centroids[c].r += r[i*num_thread+c];
		clusters->centroids[c].g += g[i*num_thread+c];
		clusters->centroids[c].b += b[i*num_thread+c];
	    }
        }
	for (int c = 0; c < clusters->k; ++c) {
		if (n[c] == 0) {
			clusters->centroids[c].r = clusters->centroids[c].r;
			clusters->centroids[c].g = clusters->centroids[c].g;
			clusters->centroids[c].b = clusters->centroids[c].b;

		}
		else if (n[c] != 0) {

			clusters->centroids[c].r = clusters->centroids[c].r / clusters->centroids[c].n;
			clusters->centroids[c].g = clusters->centroids[c].g / clusters->centroids[c].n;
			clusters->centroids[c].b = clusters->centroids[c].b / clusters->centroids[c].n;

		}
	}
	for (int c = 0; c < clusters->k; ++c) {
		//SyncLogger::print("clusters: ", c, " ", clusters->centroids[c].n, clusters->centroids[c].r, clusters->centroids[c].g, clusters->centroids[c].b);
		SyncLogger::print(loopindicator, "clusters-FN" + std::to_string(c), " ", n[c], clusters->centroids[c].n, clusters->centroids[c].r, clusters->centroids[c].g, clusters->centroids[c].b);

	}
	delete[] r;
	delete[] g;
	delete[] b;
	delete[] n;
}
void KmeansRandomP::Recenter2(RgbImage* image, Clusters* clusters, int num_thread) {
	auto r = new double[num_thread*clusters->k];
	auto g = new double[num_thread*clusters->k];
	auto b = new double[num_thread*clusters->k];
	auto n = new int[num_thread*clusters->k];

	//auto r = new double[clusters->k];
	//auto g = new double[clusters->k];
	//auto b = new double[clusters->k];
	//auto n = new int[clusters->k];
	std::thread* threads = new std::thread[num_thread-1];
	for(int i=1; i<num_thread; i++) {
		threads[i-1] = std::thread(Recenter2_threads, image, clusters, i, num_thread, r, g, b, n);
	}

	for (int c = 0; c < clusters->k; ++c) {
		r[c] = 0.;
		g[c] = 0.;
		b[c] = 0.;
		n[c] = 0;
	}

	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x+=num_thread) {
			r[image->pixels[y][x].cluster] += image->pixels[y][x].r;
			g[image->pixels[y][x].cluster] += image->pixels[y][x].g;
			b[image->pixels[y][x].cluster] += image->pixels[y][x].b;
			n[image->pixels[y][x].cluster] += 1;
		}
	}
	for (int c = 0; c < clusters->k; ++c) {
		//SyncLogger::print(loopindicator, "clusters-NC" + std::to_string(c), " ", n[c]);
		clusters->centroids[c].n = n[c];
		clusters->centroids[c].r = r[c];
		clusters->centroids[c].g = g[c];
		clusters->centroids[c].b = b[c];
	}

    	for(int i=1; i<num_thread; i++) {
            threads[i-1].join();
    	}
	for(int i = 1; i < num_thread; i++) {
	    for (int c = 0; c < clusters->k; ++c) {
		//SyncLogger::print(loopindicator, "clusters-NC" + std::to_string(c), " ", n[c]);
		clusters->centroids[c].n += n[i*num_thread+c];
		clusters->centroids[c].r += r[i*num_thread+c];
		clusters->centroids[c].g += g[i*num_thread+c];
		clusters->centroids[c].b += b[i*num_thread+c];
	    }
        }
	for (int c = 0; c < clusters->k; ++c) {
		if (n[c] == 0) {
			clusters->centroids[c].r = clusters->centroids[c].r;
			clusters->centroids[c].g = clusters->centroids[c].g;
			clusters->centroids[c].b = clusters->centroids[c].b;

		}
		else if (n[c] != 0) {

			clusters->centroids[c].r = clusters->centroids[c].r / clusters->centroids[c].n;
			clusters->centroids[c].g = clusters->centroids[c].g / clusters->centroids[c].n;
			clusters->centroids[c].b = clusters->centroids[c].b / clusters->centroids[c].n;

		}
	}
	for (int c = 0; c < clusters->k; ++c) {
		//SyncLogger::print("clusters: ", c, " ", clusters->centroids[c].n, clusters->centroids[c].r, clusters->centroids[c].g, clusters->centroids[c].b);
		SyncLogger::print(loopindicator, "clusters-FN" + std::to_string(c), " ", n[c], clusters->centroids[c].n, clusters->centroids[c].r, clusters->centroids[c].g, clusters->centroids[c].b);

	}
	delete[] r;
	delete[] g;
	delete[] b;
	delete[] n;
}

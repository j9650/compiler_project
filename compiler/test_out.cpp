
#include "test_out.h"
#define _CRT_RAND_S
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

using RgbImage = example::RgbImage;
using Clusters = example::Clusters;
using namespace example;	

void KmeansImage::AssignCluster(RgbImage_new* image, Clusters_new* clusters, int *counter) {
	std::stringstream ss;
	printf("KmeansRandom::AssignCluster\n");
	srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {

		     uint32_t randg1 = StoppableThread::random() % (image->h * image->w);

			uint32_t tt = randg1;
			uint32_t xx = tt%image->w;
			uint32_t yy = tt / image->w;
			assert(xx < image->w && yy < image->h);
			{
				assignCluster(&image->pixels[yy][xx], clusters);
				(*counter)++;
			}
		}
	}
}
void KmeansImage::AssignCluster_stable(RgbImage_new* image, Clusters_new* clusters, int *counter) {
	image->iteration++;
	std::stringstream ss;
	srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {

		    uint32_t randg1 = StoppableThread::random();

			uint32_t tt = randg1 % (image->h * image->w);
			uint32_t xx = tt%image->w;

			uint32_t yy = tt / image->w;
			assert(xx < image->w && yy < image->h);
			RgbPixel_new *p = &image->pixels[yy][xx];
			{
				if((p->stable <= STABLE_TH)||(RAND_ST)) {
					auto tmp = p->cluster;
					assignCluster(p, clusters);
					if(p->cluster == tmp)
						p->stable++;
					else
						p->stable = 0;
				}
				else
					p->stable++;

				(*counter)++;
			}














		}

	}
}
void KmeansImage::segmentImage(RgbImage_new* image, Clusters_new* clusters, int n) {

	auto tf = new TaskFactory();
	auto gs = new AggressiveGS(4);


	double  rate = 1;
	rate = this->rate;
	double pre = 100000000.0;
	double sum = 0.0;



	int *call_num1_counter = new int(0);
	auto call_num1 = new __fluid__<int>(call_num1_counter);




	auto tpb__g1 = std::bind(&KmeansImage::AssignCluster, this, image,clusters, std::placeholders::_1);
	auto tp__g1 = tf->newTask<decltype(tpb__g1), int*>("Task", tpb__g1, call_num1->p);
	Guard*       g1 = gs->newGuard("Guard", {}, tp__g1, {});







	auto v0_ = v1.init(call_num1, image->h*image->w*rate);
	auto v1_ = v1.init(call_num1, image->h*image->w*rate);
	auto tpb__g2 = std::bind(&KmeansImage::Recenter, this, image,clusters);
	auto tp__g2 = tf->newTask<decltype(tpb__g2)>("Task", tpb__g2);
	Guard*       g2 = gs->newGuard("Guard", {v0_, v1_}, tp__g2, {});



	auto call_num2 = call_num1;
	for (int i = 1; i < n; ++i) {



		int *call_num1_counter = new int(0);
		auto call_num1 = new __fluid__<int>(call_num1_counter);





		auto v2_ = v1.init(call_num2, image->h*image->w*rate);
		auto tpb__g1 = std::bind(&KmeansImage::AssignCluster, this, image,clusters, std::placeholders::_1);
		auto tp__g1 = tf->newTask<decltype(tpb__g1), int*>("Task", tpb__g1, call_num1->p);
		Guard*       g1 = gs->newGuard("Guard", {v2_}, tp__g1, {});







		auto v3_ = v1.init(call_num1, image->h*image->w*rate);
		auto tpb__g2 = std::bind(&KmeansImage::Recenter, this, image,clusters);
		auto tp__g2 = tf->newTask<decltype(tpb__g2)>("Task", tpb__g2);
		Guard*       g2 = gs->newGuard("Guard", {v3_}, tp__g2, {});



		call_num2 = call_num1;
	}


	int *counter_counter = new int(0);
	auto counter = new __fluid__<int>(counter_counter);



	auto tpb__g3 = std::bind(&KmeansImage::AssignCluster_stable, this, image, clusters, std::placeholders::_1);
	auto tp__g3 = tf->newTask<decltype(tpb__g3), int*>("Task", tpb__g3, counter->p);
	Guard*       g3 = gs->newGuard("Guard", {}, tp__g3, {});





	auto tpend = tf->newTask<void(*)(void *), void *>("End", &nullfun, NULL);
	gs->sync(tpend);


	sum = 0;
	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			sum += (pow(image->pixels[y][x].r - clusters->centroids[image->pixels[y][x].cluster].r, 2) +
				pow(image->pixels[y][x].g - clusters->centroids[image->pixels[y][x].cluster].g, 2) +
				pow(image->pixels[y][x].b - clusters->centroids[image->pixels[y][x].cluster].b, 2));
		}
	}
	this->sum = sum;

	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			image->pixels[y][x].r = clusters->centroids[image->pixels[y][x].cluster].r;
			image->pixels[y][x].g = clusters->centroids[image->pixels[y][x].cluster].g;
			image->pixels[y][x].b = clusters->centroids[image->pixels[y][x].cluster].b;
		}
	}
}
void KmeansImage::Recenter(RgbImage_new* image, Clusters_new* clusters) {
	auto r = new double[clusters->k];
	auto g = new double[clusters->k];
	auto b = new double[clusters->k];
	auto n = new int[clusters->k];
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
	delete[] r;
	delete[] g;
	delete[] b;
	delete[] n;
}
void KmeansImage::assignCluster(RgbPixel_new* p, Clusters_new* clusters) {
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
int KmeansImage::kmeans(std::string in, std::string out) {

	RgbImage_new srcImage;


	initRgbImage(&srcImage);

	std::string inImageName = in;
	std::string outImageName = out;

	loadRgbImage(inImageName.c_str(), &srcImage, 256);

	initClusters(&clusters, class_num, 1);
	segmentImage(&srcImage, &clusters, iterations);
	saveRgbImage(&srcImage, outImageName.c_str(), 255);


	freeRgbImage(&srcImage);
	return 0;
}
float KmeansImage::euclideanDistance(RgbPixel_new* p, Centroid_new* c1) {
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



	r += (p->r - c1->r) * (p->r - c1->r);
	r += (p->g - c1->g) * (p->g - c1->g);
	r += (p->b - c1->b) * (p->b - c1->b);

	r_tmp = sqrt(r);



	return (float)r_tmp;
}
int KmeansImage::pickCluster(RgbPixel_new* p, Centroid_new* c1) {
	float d1;

	d1 = euclideanDistance(p, c1);

	if (p->distance <= d1)
		return 0;

	return 1;
}
int KmeansImage::initClusters(Clusters_new* clusters, int k, float scale) {
	int i;
	float x;
	std::random_device rd;


	clusters->centroids = new Centroid_new[k];

	if (clusters == NULL) {
		printf("Warning: Oops! Cannot allocate memory for the clusters!\n");

		return 0;
	}

	clusters->k = k;
	for (i = 0; i < clusters->k; i++) {








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
void KmeansImage::freeClusters(Clusters_new* clusters) {
	if (clusters != NULL)
		free(clusters->centroids);
}
void KmeansImage::initRgbImage(RgbImage_new* image) {
	image->w = 0;
	image->h = 0;
	image->pixels = NULL;
	image->meta = NULL;
	image->iteration = 0;
}
int KmeansImage::readCell(FILE *fp, char* w) {
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
int KmeansImage::loadRgbImage(const char* fileName, RgbImage_new* image, float scale) {
	int c;
	int i;
	int j;
	char w[256];
	RgbPixel_new** pixels;
	FILE *fp;



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



	pixels = (RgbPixel_new**)malloc(image->h * sizeof(RgbPixel_new*));



	if (pixels == NULL) {
		printf("Warning: Oops! Cannot allocate memory for the pixels!2\n");

		fclose(fp);

		return 0;
	}

	c = 0;
	for (i = 0; i < image->h; i++) {
		pixels[i] = (RgbPixel_new*)malloc(image->w * sizeof(RgbPixel_new));
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


	return 1;
}
int KmeansImage::saveRgbImage(RgbImage_new* image, const char* fileName, float scale) {
	int i;
	int j;
	FILE *fp;


	fp = fopen(fileName, "w");
	if (!fp) {
		printf("Warning: Oops! Cannot open %s!\n", fileName);
		return 0;
	}

	fprintf(fp, "%d,%d\n", image->w, image->h);

	for (i = 0; i < image->h; i++) {
		for (j = 0; j < (image->w - 1); j++) {
			fprintf(fp, "%d,%d,%d,", int(image->pixels[i][j].r * scale), int(image->pixels[i][j].g * scale), int(image->pixels[i][j].b * scale));
		}
		fprintf(fp, "%d,%d,%d\n", int(image->pixels[i][j].r * scale), int(image->pixels[i][j].g * scale), int(image->pixels[i][j].b * scale));
	}

	fprintf(fp, "%s", image->meta);


	fclose(fp);

	return 1;
}
void KmeansImage::freeRgbImage(RgbImage_new* image) {
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
void KmeansImage::makeGrayscale(RgbImage_new* image) {
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


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

//http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.302.9543&rep=rep1&type=pdf

#define MAX_COUNT 1200000
#define STABLE_TH 6
#define STABLE_DI 1
namespace example {

	class tifImage {
	public:
		int w;
		int h;
		int size;
		double **pixels;
		double **gaussian;
		int **sobel;
		int **link;
		int **segment;
		int **label;
		char *meta;
		int iteration;
	};


	class Sobel {
	public:
		static std::string loopindicator;
		double rate;
		double sum;
		double sobel_th;
		double sigma;
		tifImage *image;
		double iou;

		int sobel(std::string in, std::string out);
		void InitImage(tifImage *image, std::string in);
		void testImage(tifImage *image, std::string in);
  		//void Randomcolor(tifImage *graph);
  		//void Checkcolor(tifImage *graph);
		
		virtual void segmentImage(tifImage *image);

		virtual void Gaussian_filter(tifImage *image);
		virtual void Sobel_filter(tifImage *image);
		virtual void Link_edge(tifImage *image);
		virtual void Segment(tifImage *image);
		double conv(double **feature, double *kernel, int k_n, int x, int y);
		void conv_gaussian(double **feature, double *kernel, int x, int y, double **output);
		void conv_sobel(double **feature, double *kernel_x, double *kernel_y, int x, int y, int **output);

	};
}
#pragma once
/*
* convolution.hpp
*
* Created on: Sep 9, 2013
* 			Author: Amir Yazdanbakhsh <a.yazdanbakhsh@gatech.edu>
*/

#ifndef __CONV_H__
#define __CONV_H__

#include "rgb_image.h"
#include <iostream>

int total = 0;
static float kx[][3] =
{
	{ -1, -2, -1 },
	{ 0,  0,  0 },
	{ 1,  2,  1 }
};

static float ky[][3] =
{
	{ -1, 0, 1 },
	{ -2, 0, 2 },
	{ -1, 0, 1 }
};

float convolve(float w[][3], float k[][3])
{
	float r;
	r = 0.0;
	for (int j = 0; j < 3; j++)
		for (int i = 0; i < 3; i++)
		{
			r += w[i][j] * k[j][i];
		}
	return r;
}

float sobel(float w[][3]) {
	float sx;
	float sy;
	float s;

	double dataIn[9];

	dataIn[0] = w[0][0];
	dataIn[1] = w[0][1];
	dataIn[2] = w[0][2];
	dataIn[3] = w[1][0];
	dataIn[4] = w[1][1];
	dataIn[5] = w[1][2];
	dataIn[6] = w[2][0];
	dataIn[7] = w[2][1];
	dataIn[8] = w[2][2];

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (DEBUG) std::cout << "w_ " << i << j << ": " << w[i][j] << std::endl;
	sx = convolve(w, ky);
	if (DEBUG) std::cout << "sx: " << sx << std::endl;
	sy = convolve(w, kx);
	if (DEBUG) std::cout << "sy: " << sy << std::endl;

	s = sqrt(sx * sx + sy * sy);
	if (s >= (256 / sqrt(256 * 256 + 256 * 256)))
		s = (float) 255 / (float)sqrt(256 * 256 + 256 * 256);

	return s;
};

#define WINDOW(imagePtr, x, y, window) {					\
 	window[0][0] = imagePtr->pixels[ y - 1 ][ x - 1 ]->r ;	\
 	window[0][1] = imagePtr->pixels[ y - 1 ][ x ]->r ;	    \
 	window[0][2] = imagePtr->pixels[ y - 1 ][ x + 1 ]->r ;	\
\
	window[1][0] = imagePtr->pixels[ y ][ x - 1 ]->r ;		\
 	window[1][1] = imagePtr->pixels[ y ][ x ]->r ;	    	\
 	window[1][2] = imagePtr->pixels[ y ][ x + 1 ]->r ;		\
\
	window[2][0] = imagePtr->pixels[ y + 1 ][ x - 1 ]->r ;	\
 	window[2][1] = imagePtr->pixels[ y + 1 ][ x ]->r ;	    \
 	window[2][2] = imagePtr->pixels[ y + 1 ][ x + 1 ]->r ;	\
}

#define HALF_WINDOW(imagePtr, x, y, window) {																			\
	window[0][0] = (x == 0 || y == 0										) ? 0 : imagePtr->pixels[y - 1][x - 1]->r;	\
	window[0][1] = (y == 0													) ? 0 : imagePtr->pixels[y - 1][x]->r;		\
	window[0][2] = (x == imagePtr->width -1 || y == 0						) ? 0 : imagePtr->pixels[y - 1][x + 1]->r;	\
\
	window[1][0] = (x == 0 													) ? 0 : imagePtr->pixels[y][x - 1]->r;		\
	window[1][1] = 																	imagePtr->pixels[y][x]->r;			\
	window[1][2] = (x == imagePtr->width -1									) ? 0 : imagePtr->pixels[y][x + 1]->r;		\
\
	window[2][0] = (x == 0 || y == imagePtr->height - 1						) ? 0 : imagePtr->pixels[y + 1][x - 1]->r;	\
	window[2][1] = (y == imagePtr->height - 1								) ? 0 : imagePtr->pixels[y + 1][x]->r;		\
	window[2][2] = (x == imagePtr->width -1 || y == imagePtr->height - 1	) ? 0 : imagePtr->pixels[y + 1][x + 1]->r;	\
}

#endif
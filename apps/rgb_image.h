#pragma once
/*
* rgb_image.hpp
*
* Created on: Sep 9, 2013
* 			Author: Amir Yazdanbakhsh <a.yazdanbakhsh@gatech.edu>
* Edited by: Haibo Zhang <huz123@cse.psu.edu>
*/

#ifndef __RGB_IMAGE_HPP__
#define __RGB_IMAGE_HPP__

#include <memory>
#include <iostream>
#include <regex>
#include <string>

#include <vector>
#include <fstream>
#include <cstdlib>

#define DEBUG 0

class Pixel {
public:
	Pixel(float r, float g, float b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}
	float r;
	float g;
	float b;
};

void  resplit(std::vector<std::string>& elems, const std::string & s, std::regex& rgx) {

	std::sregex_token_iterator iter(s.begin(), s.end(), rgx, -1);
	std::sregex_token_iterator end;

	while (iter != end) {
		//std::cout << "S43:" << *iter << std::endl;
		elems.push_back(*iter);
		++iter;
	}
}

class Image {
public:
	int 			width;
	int 			height;
	std::vector 	<std::vector<std::shared_ptr<Pixel> > > pixels;
	std::string 	meta;

	// Constructor
	Image()
	{
		this->width = 0;
		this->height = 0;
	}

	int loadRgbImage(std::string filename) {
		std::ifstream imageFile;

		if (DEBUG)
			std::cout << "# Loading " << filename << " ..." << std::endl;

		imageFile.open(filename.c_str());
		if (!imageFile.is_open())
		{
			std::cerr << "# Error openning the file!" << std::endl;
			return 0;
		}

		// Read first line and split based on the , and any spaces before or after
		std::string line;
		std::getline(imageFile, line);
		std::vector<std::string> imageInfo;
		resplit(imageInfo, line, std::regex("\t*,\t*"));
		this->width = atoi(imageInfo[0].c_str());
		this->height = atoi(imageInfo[1].c_str());

		if (DEBUG)
		{
			std::cout << "# Width:  " << this->width;
			std::cout << "# Height: " << this->height << std::endl;
		}

		// We assume there is a newline after each row
		for (int h = 0; h < this->height; h++)
		{
			std::getline(imageFile, line);
			std::vector<std::string> currRowString;
			resplit(currRowString, line, std::regex("\t*,\t*"));

			std::vector<std::shared_ptr<Pixel> > currRow;

			for (int w = 0; w < this->width; w++)
			{
				int index = w * 3;
				float r = (float)atoi(currRowString[index++].c_str());
				float g = (float)atoi(currRowString[index++].c_str());
				float b = (float)atoi(currRowString[index++].c_str());
				// Add pixel to the current row
				std::shared_ptr<Pixel> currPixel(new Pixel(r, g, b));
				currRow.push_back(currPixel);
			}
			this->pixels.push_back(currRow);
		}

		std::getline(imageFile, line);
		this->meta = line;
		return 1;
	}
	int saveRgbImage(std::string outFilename, double scale) 
	{
		if (DEBUG)
		{
			std::cout << "# Savint into " << outFilename << " ..." << std::endl;
		}

		std::ofstream outFile;
		outFile.open(outFilename.c_str());

		outFile << this->width << "," << this->height << std::endl;

		for (int h = 0; h < this->height; h++)
		{
			for (int w = 0; w < (this->width - 1); w++)
			{
				// Write Red
				int red = (int)(this->pixels[h][w]->r * scale);
				int green = (int)(this->pixels[h][w]->g * scale);
				int blue = (int)(this->pixels[h][w]->b * scale);

				//if ( red > 255 )
				//		red = 255 ;
				//	if ( green > 255 )
				//		green = 255 ;
				///	if ( blue > 255 )
				//	blue = 255 ;
				outFile << red << ",";
				// Write Green
				outFile << green << ",";
				// Write Blue
				outFile << blue << ",";

			}

			int red = (int)(this->pixels[h][this->width - 1]->r * scale);
			int green = (int)(this->pixels[h][this->width - 1]->g * scale);
			int blue = (int)(this->pixels[h][this->width - 1]->b * scale);


			// Write Red
			outFile << red << ",";
			// Write Green
			outFile << green << ",";
			// Write Blue
			outFile << blue << std::endl;
		}

		// Print the meta information
		outFile << this->meta;
		outFile.close();
		return 1;
	}

	void makeGrayscale() {
		float luminance;

		float rC = (float)(0.30 / 256.0);
		float gC = (float)(0.59 / 256.0);
		float bC = (float)(0.11 / 256.0);

		for (int h = 0; h < this->height; h++)
		{
			for (int w = 0; w < this->width; w++)
			{
				luminance = (rC * this->pixels[h][w]->r) +
					(gC * this->pixels[h][w]->g) +
					(bC * this->pixels[h][w]->b);

				this->pixels[h][w]->r = luminance;
				this->pixels[h][w]->g = luminance;
				this->pixels[h][w]->b = luminance;
			}
		}
	}
	void printPixel(int x, int y) {
		std::cout << "# Red: 	" << this->pixels[x][y]->r << std::endl;
		std::cout << "# Green: 	" << this->pixels[x][y]->g << std::endl;
		std::cout << "# Blue: 	" << this->pixels[x][y]->b << std::endl;
	}
};

#endif
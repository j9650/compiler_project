#pragma once
#include "../fluid/guard.h"
#include "../fluid/valve.h"
#include "../fluid/fluid.h"
#include "../fluid/guardscheduler.h"
#include "sobel.h"
namespace example {

	//class SobelFluid : public SobelFluid {
	//public:
	//	//virtual void Coloring(GraphGC *graph);
	//};


	////__Fluid__
	////class SobelFluid : public Sobel {
	class SobelFluid : public Sobel {
	public:
		////#pragma valve{ValveGT<int> v1}
		ValveGT<int> v1;
		SobelFluid() {
					
		}
		virtual void segmentImage(tifImage *image);

		////virtual void Gaussian_filter(tifImage *image);
		virtual void Gaussian_filter(tifImage *image, int *call_num);
		////virtual void Sobel_filter(tifImage *image);
		virtual void Sobel_filter(tifImage *image);
		virtual void Link_edge(tifImage *image);
		virtual void Segment(tifImage *image);
	};
	
};
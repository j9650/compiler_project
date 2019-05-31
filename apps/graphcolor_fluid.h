#pragma once
#include "../fluid/guard.h"
#include "../fluid/valve.h"
#include "../fluid/fluid.h"
#include "../fluid/guardscheduler.h"
#include "graphcolor.h"
namespace example {

	class GraphcolorFluidX : public GraphcolorFluid {
	public:
		ValveGT<int> v1;
		virtual void Coloring(GraphGC *graph);
	};
	
};
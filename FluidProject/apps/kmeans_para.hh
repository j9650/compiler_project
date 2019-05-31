//
// Created by huz123 on 9/15/18.
//

#ifndef FLUID_KMEANS_PARA_HH
#define FLUID_KMEANS_PARA_HH

#include "kmeans.h"

namespace example {
    class KmeansParallel : public KmeansRandom{
    public:
        //virtual void AssignCluster(RgbImage* image, Clusters* clusters, int& counter);
        //virtual void Recenter(RgbImage* image, Clusters* clusters);
        KmeansParallel() {

        }
        virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
        virtual void AssignCluster(RgbImage* image, Clusters* clusters, int* counter);
        void subAssignCluster(RgbImage* image, Clusters* clusters, int subhh, int subww, int hh, int ww);
    };

};
#endif //FLUID_KMEANS_PARA_HH

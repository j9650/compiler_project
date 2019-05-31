//
// Created by huz123 on 9/15/18.
//

#include "kmeans_para.hh"
#include "../fluid/thread.h"
#include "../fluid/tasklet.h"

#include <numeric>
#include <cassert>
#include <algorithm>
#include <random>
#include <sstream>
using RgbImage = example::RgbImage;
using Clusters = example::Clusters;
using namespace example;
void KmeansParallel::subAssignCluster(RgbImage* image, Clusters* clusters, int subhh, int subww, int hh, int ww){
    srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
    for (int y = 0; y < hh; y++) {
        for (int x = 0; x < ww; x++) {

            uint32_t randg1 = StoppableThread::random() % (image->h * image->w);


            uint32_t tt = randg1;
            uint32_t xx = tt%hh;

            uint32_t yy = tt/ww;
            assert(xx < image->w && yy < image->h);
            this->assignCluster(&image->pixels[yy][xx], clusters);
        }
    }

}
void KmeansParallel::AssignCluster(RgbImage* image, Clusters* clusters, int* counter) {

    std::stringstream ss;
    auto tf = new TaskFactory();
    auto gs = new AggressiveGS(4);
    srand((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
    int hh = 1, ww = 1;
    for (int y = 0; y <= image->h -hh; y+= hh) {
        for (int x = 0; x <= image->w - ww; x+= ww) {

            uint32_t randg1 = StoppableThread::random() % (image->h * image->w);

            uint32_t tt = randg1;
            uint32_t xx = tt%image->w;

            uint32_t yy = tt / image->w;
            assert(xx < image->w && yy < image->h);
            tf.newTask("assignCluster", this->assignCluster, &image->pixels[yy][xx], clusters);
            (*counter)++;
        }
        //SyncLogger::print("AssignCluster", y);
    }
    tf.sync();
}


void KmeansSchemeFluidX::segmentImage(RgbImage* image, Clusters* clusters, int n) {

    auto tf = new TaskFactory();
    auto gs = new AggressiveGS(8);
    auto tp0 = tf->newTask<void(*)(void *), void *>("Begin", &nullfun, NULL);
    auto tp0status = new __fluid__<bool>(tp0->isfinished());
    auto v0 = new ValveEQ<bool>(tp0status, true);

    tp0->run();

    double  rate = 1;
    SyncLogger::print("=====KmeansSchemeFluidX=====");

    for (int i = 0; i < n; ++i) {
        loopindicator = "loop" + std::to_string(i);
        int* counter = new int(0);
        auto tpb1 = std::bind(&KmeansRandom::AssignCluster, this, image, clusters, std::placeholders::_1);
        auto tp1 = tf->newTask<decltype(tpb1), int*>("AssignCluster" + std::to_string(i), tpb1, counter);
        Guard* g1 = gs->newGuard("GuardAssignCluster" + std::to_string(i), { /*v0*/ }, tp1, {});


        auto tp1status = new __fluid__<int>(counter);

        //if (i < 10) rate = 1;
        if (rate <= 0.05) rate = 0.05;

        auto v1 = new ValveGT<int>(tp1status, image->h*image->w*rate);

        auto tpb2 = std::bind(&KmeansRandom::Recenter, this, image, clusters);
        auto tp2 = tf->newTask<decltype(tpb2)>("Recenter" + std::to_string(i), tpb2);
        Guard*       g2 = gs->newGuard("GuardRecenter" + std::to_string(i), { v1 }, tp2, { g1 });
        gs->iSync();
        //tp0status = new __fluid__<bool>(tp2->isfinished());
        //v0 = new ValveEQ<bool>(tp0status, true);
        rate = rate - i  *1.0 / n;
    }

    auto tp3 = tf->newTask<void(*)(void *), void *>("End", &nullfun, NULL);
    auto tp3status = new __fluid__<bool>(tp3->parent->isfinished());
    auto v3 = new ValveEQ<bool>(tp3status, true);
    //while (*(tp3->parent->isfinished()) == false)
    //	; // HARD HACK!
    gs->sync(tp3);

    double sum = 0;
    for (int y = 0; y < image->h; y++) {
        for (int x = 0; x < image->w; x++) {
            sum += (pow(image->pixels[y][x].r - clusters->centroids[image->pixels[y][x].cluster].r, 2) +
                    pow(image->pixels[y][x].g - clusters->centroids[image->pixels[y][x].cluster].g, 2) +
                    pow(image->pixels[y][x].b - clusters->centroids[image->pixels[y][x].cluster].b, 2));
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
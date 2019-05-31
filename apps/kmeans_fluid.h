#pragma once
#include "../fluid/guard.h"
#include "../fluid/valve.h"
#include "../fluid/fluid.h"
#include "../fluid/guardscheduler.h"
#include "kmeans.h"
namespace example {

	class KmeansSchemePPL : public KmeansRandomChangedRecenter {
	public:
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
	};
	
	//class KmeansSchemeRecenterRandomized : public KmeansRandomChangedRecenter {
	//public:
	//	virtual void Recenter(RgbImage* image, Clusters* clusters);
	//	virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
	//};
	class KmeansSchemeNewFluid : public Kmeans {
	public:
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
	};
	//single thread
	class KmeansSchemeFluidX : public KmeansRandomChangedRecenter {
	public:
		ValveGT<int> v1;
		TaskFactory *ttf;
		AggressiveGS *ggs;
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
	};
	//STABLE single thread
	class KmeansSchemeFluidXS : public KmeansRandomChangedRecenter {
	public:
		ValveGT<int> v1;
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
	};
	//STABLE single thread
	class KmeansSchemeFluidXSB : public KmeansRandomChangedRecenter {
	public:
		ValveST<int> v1;
		ValveSB<Kmeans*, bool*, int> v21;
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
	};
	//priority queue single thread
	class KmeansSchemeFluidXpq : public KmeansRandomChangedRecenter {
	public:
		ValveGT<int> v1;
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
	};
	//multi thread
	class KmeansSchemeFluidX2 : public KmeansRandomChangedRecenter {
	public:
		KmeansSchemeFluidX2(int _thread_num) {
			thread_num = _thread_num;
		}
		int thread_num;
		ValveGT<int> v1;
		ValveGT_vec<int> v2;
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
	};
	//STABLE multi thread
	class KmeansSchemeFluidX2S : public KmeansRandomChangedRecenter {
	public:
		KmeansSchemeFluidX2S(int _thread_num) {
			thread_num = _thread_num;
		}
		int thread_num;
		ValveGT<int> v1;
		ValveGT_vec<int> v2;
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
	};
	//STABLE multi thread
	class KmeansSchemeFluidX2SB : public KmeansRandomChangedRecenter {
	public:
		KmeansSchemeFluidX2SB(int _thread_num) {
			thread_num = _thread_num;
		}
		int thread_num;
		ValveGT<int> v1;
		ValveGT_vec<int> v2;
		ValveSB<Kmeans*, std::vector<bool*>> v20;
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
	};
	/*
	//priority queue multi thread
	class KmeansSchemeFluidX2pq : public KmeansRandomChangedRecenter {
	public:
		KmeansSchemeFluidX2pq(int _thread_num) {
			thread_num = _thread_num;
		}
		int thread_num;
		ValveGT<int> v1;
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
	};*/
	class KmeansSchemeFluidPause : public KmeansRandomChangedRecenter {
	public:
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
	};

	class KmeansSchemeFluidXI : public KmeansRandomChangedRecenter {
	public:
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
	};

	class KmeansSchemeFluidExtreme : public KmeansRandomChangedRecenter {
	public:
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
	};

	class ValveClustersPercent : public ValvePerc<Clusters*> {
	private:
		__fluid__<Clusters*>* f;
		int* curr;
		int* past;
		double raito;
	public:


		ValveClustersPercent(__fluid__<Clusters*>* _f, double percent) {
			f = nullptr;
			if (_f == nullptr) return;
			perc = percent; f = _f;
			raito = 0;
			auto clusterp = *(f->value());
			curr = new int[clusterp->k]; past = new int[clusterp->k];
			for (int i = 0; i < clusterp->k; i++) {
				curr[i] = past[i] = 0;
			}
		}

		bool check() {
			if (f == nullptr) return true;
			raito = 0;
			auto clusterp = *(f->value());
			for (int i = 0; i < clusterp->k; i++) {
				curr[i] = clusterp->centroids[i].n;
				double rate = past[i] ? (curr[i] - past[i]) / (double)past[i] : 1;
				raito += rate * rate;
			}
			auto temp = curr; curr = past; past = temp;
			if (raito / clusterp->k <= perc) {
				return true;
			}
			else
				return false;
		}
	};
};

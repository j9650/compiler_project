
__Fluid__
class KmeansRandom : public Kmeans {
	public:
		KmeansRandom() {
					
		}
		void segmentImage(RgbImage* image, Clusters* clusters, int n);
		__function__ void AssignCluster(RgbImage* image, Clusters* clusters);
		__function__ void Recenter(RgbImage* image, Clusters* clusters);

	__Valve__ <templete T>
	bool ValveGT(T a, T b) {
			current = *(static_cast<T*>(f->value()));
			if (current >= cond) {
				return true;
			}
			else
				return false;
	}
	
};

void KmeansSchemeFluidX::segmentImage(RgbImage* image, Clusters* clusters, int n) {
	...

		__call_num__ counter = new call_num(KmeansRandom::AssignCluster,assignCluster);
		__task__ tp1 = std::bind(&KmeansRandom::AssignCluster, this, image, clusters, __call_num__ counter);
		__guard__ g1 = newGuard("GuardAssignCluster" + std::to_string(i), {}, tp1, {});

		__valve__ v1 = ValveGT<int>(__call_num__ counter, image->size*rate);
		__task__ tp2 = std::bind(&KmeansRandom::Recenter, this, image, clusters);
		__guard__ g2 = newGuard("GuardRecenter" + std::to_string(i), {v1}, tp2, {g1});

	...
}

__function__
void KmeansRandom::AssignCluster(RgbImage* image, Clusters* clusters, int* counter) {
	...

		__call_num__ assignCluster(&image->pixels[yy][xx], clusters);

	...
}
Or:

__function__
void KmeansRandom::AssignCluster(RgbImage* image, Clusters* clusters, int* counter) {
	...

		__stable__ (p->cluster) assignCluster(p, clusters);

	...
}

================================================================================
class KmeansRandom : public Kmeans {
	public:
		KmeansRandom() {
					
		}
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
		virtual void AssignCluster(RgbImage* image, Clusters* clusters, int* call_num);
		virtual void Recenter(RgbImage* image, Clusters* clusters);

	class ValveGT : public Valve {
	private:
		int current;
		int cond;
		__fluid__<int>* f;
	public:
		ValveGT(__fluid__<int>* _f, int para) {
			f = _f;
			cond = para;
		}
	
		bool check() {
			current = *(static_cast<int*>(f->value()));
			if (current >= cond) {
				return true;
			}
			else
				return false;
		}
	
		int* get() {
			return &current;
		}
	};
};

void KmeansSchemeFluidX::segmentImage(RgbImage* image, Clusters* clusters, int n) {
	...

		int* counter = new int(0);
		auto tpb1 = std::bind(&KmeansRandom::AssignCluster, this, image, clusters, std::placeholders::_1);
		auto tp1 = tf->newTask<decltype(tpb1), int*>("AssignCluster" + std::to_string(i), tpb1, counter);
		Guard* g1 = gs->newGuard("GuardAssignCluster" + std::to_string(i), { /*v0*/ }, tp1, {});

		auto tp1status = new __fluid__<int>(counter);

		auto v1 = new ValveGT<int>(tp1status, image->h*image->w*rate);

		auto tpb2 = std::bind(&KmeansRandom::Recenter, this, image, clusters);
		auto tp2 = tf->newTask<decltype(tpb2)>("Recenter" + std::to_string(i), tpb2);
		Guard*       g2 = gs->newGuard("GuardRecenter" + std::to_string(i), { v1 }, tp2, { g1 });
		gs->iSync();

	...
}

void KmeansRandom::AssignCluster(RgbImage* image, Clusters* clusters, int* call_num) {
	...


		assignCluster(&image->pixels[yy][xx], clusters);
		(*call_num)++;

	...
}
Or:

void KmeansRandom::AssignCluster(RgbImage* image, Clusters* clusters, int* counter) {
	...
		if((p->stable <= STABLE_TH)||((rand() % STABLE_TH) == 0))
		{
			auto before=p->cluster;
			assignCluster(p, clusters);
			if(p->cluster != before) p->stable++;
			else p->stable = 0;
		}
		else
		{
			p->stable++;
		}

	...
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
__Fluid__
typedef struct {
	float r;
	float g;
	float b;
	int cluster;
	float distance;
	int mark;
} RgbPixel;

typedef struct {
	int w;
	int h;
	int size;
	RgbPixel** pixels;
	char* meta;
	int iteration;
	int **pq;
	int *l;
	int *r;
	int ql;
} RgbImage;

__Fluid__
class KmeansRandom : public Kmeans {
	public:
		RgbImage *image;

		KmeansRandom() {
					
		}
		void segmentImage(RgbImage* image, Clusters* clusters, int n);
		__function__ virtual void AssignCluster(RgbImage* image, Clusters* clusters);
		__function__ virtual void Recenter(RgbImage* image, Clusters* clusters);

	__Valve__ ValveGT(int a, int b) {
			if (a >= b) {
				return true;
			}
			else
				return false;
	}
	
};

void KmeansSchemeFluidX::segmentImage(RgbImage* image, Clusters* clusters, int n) { //not terminal
	...

		__call_num__ new counter;

		<<<g1,{},{},__call_num__ counter>>>KmeansRandom::AssignCluster(image,clusters);

		<<<g2,{KmeansRandom.ValveGT(counter, image->size*rate)},{}>>>KmeansRandom::Recenter(image,clusters);

	...
}

void KmeansSchemeFluidX::segmentImage(RgbImage* image, Clusters* clusters, int n) { //early terminal
	...

		__call_num__ new counter;

		<<<g1,{},{},__call_num__ counter>>>KmeansRandom::AssignCluster(image,clusters);

		<<<g2,{KmeansRandom.ValveGT(counter, image->size*rate)},{g1}>>>KmeansRandom::Recenter(image,clusters);

	...
}
void KmeansRandom::AssignCluster(RgbImage* image, Clusters* clusters, int* call_num) { // call_num
	...
		__call_num__ assignCluster(&image->pixels[yy][xx], clusters);
	...
}
void KmeansRandom::AssignCluster(RgbImage* image, Clusters* clusters, int* call_num) { // stable
	...
		__call_num__ __stable__<image->pixels[yy][xx].cluster, image->pixels[yy][xx]> assignCluster(&image->pixels[yy][xx], clusters);
	...
}


=========================================================================================================
typedef struct {
	float r;
	float g;
	float b;
	int cluster;
	float distance;
	int mark;
	int stable;
} RgbPixel;

typedef struct {
	int w;
	int h;
	int size;
	RgbPixel** pixels;
	char* meta;
	int iteration;
	int **pq;
	int *l;
	int *r;
	int ql;
} RgbImage;

class KmeansRandom : public Kmeans {
	public:
		RgbImage *image;

		KmeansRandom() {
					
		}
		virtual void segmentImage(RgbImage* image, Clusters* clusters, int n);
		virtual void AssignCluster(RgbImage* image, Clusters* clusters, int* call_num);
		virtual void Recenter(RgbImage* image, Clusters* clusters);
		int stable;
};

class KmeansRandom__ValveGT : public Valve {
private:
	int a;
	int b;
	__fluid__<int>* f;
public:
	ValveGT(__fluid__<int>* a_, int b_) {
		f = a_;
		b = b_;
	}

	bool check() {
		a = *(static_cast<int*>(f->value()));
		if (a >= b) {
			return true;
		}
		else
			return false;
	}

	int* get() {
		return &a;
	}
};

void KmeansSchemeFluidX::segmentImage(RgbImage* image, Clusters* clusters, int n) { //not terminal
	...

		int* counter = new int(0);
		auto tp1status__counter = new __fluid__<int>(counter);

		auto tpb1 = std::bind(&KmeansRandom::AssignCluster, this, image, clusters, std::placeholders::_1);
		auto tp1 = tf->newTask<decltype(tpb1), int*>("AssignCluster" + std::to_string(i), tpb1, counter);
		Guard* g1 = gs->newGuard("GuardAssignCluster" + std::to_string(i), { /*v0*/ }, tp1, {});

		auto v1 = new KmeansRandom__ValveGT(tp1status__counter, image->h*image->w*rate);
		auto tpb2 = std::bind(&KmeansRandom::Recenter, this, image, clusters);
		auto tp2 = tf->newTask<decltype(tpb2)>("Recenter" + std::to_string(i), tpb2);
		Guard*       g2 = gs->newGuard("GuardRecenter" + std::to_string(i), { v1 }, tp2, { });

	...
}
void KmeansSchemeFluidX::segmentImage(RgbImage* image, Clusters* clusters, int n) { //early terminal
	...

		int* counter = new int(0);
		auto tp1status__counter = new __fluid__<int>(counter);

		auto tpb1 = std::bind(&KmeansRandom::AssignCluster, this, image, clusters, std::placeholders::_1);
		auto tp1 = tf->newTask<decltype(tpb1), int*>("AssignCluster" + std::to_string(i), tpb1, counter);
		Guard* g1 = gs->newGuard("GuardAssignCluster" + std::to_string(i), { /*v0*/ }, tp1, {});

		auto v1 = new KmeansRandom__ValveGT(tp1status__counter, image->h*image->w*rate);
		auto tpb2 = std::bind(&KmeansRandom::Recenter, this, image, clusters);
		auto tp2 = tf->newTask<decltype(tpb2)>("Recenter" + std::to_string(i), tpb2);
		Guard*       g2 = gs->newGuard("GuardRecenter" + std::to_string(i), { v1 }, tp2, { g1 });

	...
}

void KmeansRandom::AssignCluster(RgbImage* image, Clusters* clusters, int* call_num) { //call_num
	...
		assignCluster(&image->pixels[yy][xx], clusters);
		(*call_num)++;
	...
}
void KmeansRandom::AssignCluster(RgbImage* image, Clusters* clusters, int* counter) { //stable
	...
		if((image->pixels[yy][xx].stable <= STABLE_TH)||((rand() % STABLE_TH) == 0))
		{
			auto before=image->pixels[yy][xx].cluster;
			assignCluster(&image->pixels[yy][xx], clusters);
			if(image->pixels[yy][xx].cluster == before) image->pixels[yy][xx].stable++;
			else image->pixels[yy][xx].stable = 0;
		}
		else
		{
			image->pixels[yy][xx].stable++;
		}

	...
}
typedef struct {
	float r;
	float g;
	float b;
	int n;
} Centroid;

typedef struct {
	int k;
	Centroid* centroids;
} Clusters;

typedef std::vector<Centroid> CentroidVec_t;

__Fluid__
typedef struct { ////
	float r;
	float g;
	float b;
	#pragma stable {int cluster;}
	float distance;
	int mark;
} RgbPixel;
typedef std::vector<RgbPixel> RgbPixelVec_t;

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

typedef struct {
	int w;
	int h;
	RgbPixelVec_t pixels;
	char* meta;
} RgbImageVec;

__Fluid__
class KmeansImage { ////
public:
	Clusters clusters;
	static std::string loopindicator;
	double rate;
	double sum;
	int iterations;
	int class_num;
	
	
	void initRgbImage(RgbImage* image); //
	int loadRgbImage(const char* fileName, RgbImage* image, float scale); //
	int saveRgbImage(RgbImage* image, const char* fileName, float scale); //
	void freeRgbImage(RgbImage* image); //

	void makeGrayscale(RgbImage* rgbImage); //
	float euclideanDistance(RgbPixel* p, Centroid* c1); //
	int pickCluster(RgbPixel* p, Centroid* c1); //

	int initClusters(Clusters* clusters, int k, float scale); //

	void freeClusters(Clusters* clusters); //

	int readCell(FILE *fp, char* w); //

	int kmeans(std::string in, std::string out); //

	virtual void AssignCluster(RgbImage* image, Clusters* clusters); //
	virtual void Recenter(RgbImage* image, Clusters* clusters); //
	void assignCluster(RgbPixel* p, Clusters* clusters); //
	virtual void segmentImage(RgbImage* image, Clusters* clusters, int n); //
	#pragma valve {ValveGT v1;}
};

int KmeansImage::kmeans(std::string in, std::string out) {

	RgbImage srcImage;
	

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
float KmeansImage::euclideanDistance(RgbPixel* p, Centroid* c1) {
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

	//#pragma parrot(input, "kmeans", [6]dataIn)

	r += (p->r - c1->r) * (p->r - c1->r);
	r += (p->g - c1->g) * (p->g - c1->g);
	r += (p->b - c1->b) * (p->b - c1->b);

	r_tmp = sqrt(r);

	//#pragma parrot(output, "kmeans", <0.0; 1.0>r_tmp)

	return (float)r_tmp;
}
int KmeansImage::pickCluster(RgbPixel* p, Centroid* c1) {
	float d1;

	d1 = euclideanDistance(p, c1);

	if (p->distance <= d1)
		return 0;

	return 1;
}
int KmeansImage::initClusters(Clusters* clusters, int k, float scale) {
	int i;
	float x;
	std::random_device rd;
	//std::random_device rd(__TIMESTAMP__);
	//clusters->centroids = (Centroid*)malloc(k * sizeof(Centroid));
	clusters->centroids = new Centroid[k];

	if (clusters == NULL) {
		printf("Warning: Oops! Cannot allocate memory for the clusters!\n");

		return 0;
	}

	clusters->k = k;
	for (i = 0; i < clusters->k; i++) {
		//x = (((float)rand()) / RAND_MAX) * scale;
		//clusters->centroids[i].r =x;

		//x = (((float)rand()) / RAND_MAX) * scale;
		//clusters->centroids[i].g = x;

		//x = (((float)rand()) / RAND_MAX) * scale;
		//clusters->centroids[i].b = x;
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
void KmeansImage::freeClusters(Clusters* clusters) {
	if (clusters != NULL)
		free(clusters->centroids);
}
void KmeansImage::initRgbImage(RgbImage* image) {
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
int KmeansImage::loadRgbImage(const char* fileName, RgbImage* image, float scale) {
	int c;
	int i;
	int j;
	char w[256];
	RgbPixel** pixels;
	FILE *fp;

	//printf("Loading %s ...\n", fileName);
	//fopen(&fp, fileName, "r");
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

	//printf("%d x %d\n", image->w, image->h);

	pixels = (RgbPixel**)malloc(image->h * sizeof(RgbPixel*));
	//printf("h:%d\n",image->h);
	//pixels = new RgbPixel*[image->h];

	if (pixels == NULL) {
		printf("Warning: Oops! Cannot allocate memory for the pixels!2\n");

		fclose(fp);

		return 0;
	}

	c = 0;
	for (i = 0; i < image->h; i++) {
		pixels[i] = (RgbPixel*)malloc(image->w * sizeof(RgbPixel));
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
int KmeansImage::saveRgbImage(RgbImage* image, const char* fileName, float scale) {
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
	//printf("%s\n", image->meta);

	fclose(fp);

	return 1;
}
void KmeansImage::freeRgbImage(RgbImage* image) {
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
void KmeansImage::makeGrayscale(RgbImage* image) {
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

void KmeansImage::assignCluster(RgbPixel* p, Clusters* clusters) {
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
//for single stread non-STABLE
void KmeansImage::AssignCluster(RgbImage* image, Clusters* clusters) {
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
			#pragma call_num {assignCluster(&image->pixels[yy][xx], clusters);}
		}
	}
}

void KmeansImage::Recenter(RgbImage* image, Clusters* clusters) {
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
void KmeansImage::segmentImage(RgbImage* image, Clusters* clusters, int n) {

	Task::init(4);

	double  rate = 1;
	rate = this->rate;
	double pre = 100000000.0;
	double sum = 0.0;

	//int* call_num1_counter = new int(0);
	//auto call_num1 = new __fluid__<int>(call_num1_counter);
	#pragma call_num {__call_count__ call_num1;}

	//auto tpb1 = std::bind(&KmeansImage::AssignCluster, this, image, clusters, std::placeholders::_1);
	//auto tp1 = tf->newTask<decltype(tpb1), int*>("AssignCluster" + std::to_string(i), tpb1, call_num1->p);
	//Guard* g1 = gs->newGuard("GuardAssignCluster" + std::to_string(i), { }, tp1, {});
	<<<g1,{},{},call_num1>>>AssignCluster(image,cluster);


	//auto v0_ = v1.init(call_num1, image->h*image->w*rate);
	//auto tpb2 = std::bind(&KmeansImage::Recenter, this, image, clusters);
	//auto tp2 = tf->newTask<decltype(tpb2)>("Recenter" + std::to_string(i), tpb2);
	//Guard*       g2 = gs->newGuard("GuardRecenter" + std::to_string(i), { v1_ }, tp2, {  });
	<<<g2,{v1(call_num1, image->h*image->w*rate)},{}>>>Recenter(image,cluster);

	//auto call_num2 = call_num1;
	#pragma call_num {__call_count__ call_num2 = call_num1;}
	for (int i = 1; i < n; ++i) {

		//int* call_num1_counter = new int(0);
		//auto call_num1 = new __fluid__<int>(call_num1_counter);
		#pragma call_num {__call_count__ call_num1;}

		//v1_ = v1.init(call_num2, image->h*image->w*rate);
		//auto tpb1 = std::bind(&KmeansImage::AssignCluster, this, image, clusters, std::placeholders::_1);
		//auto tp1 = tf->newTask<decltype(tpb1), int*>("AssignCluster" + std::to_string(i), tpb1, call_num1->p);
		//Guard* g1 = gs->newGuard("GuardAssignCluster" + std::to_string(i), { v1_ }, tp1, {});
		<<<g1,{v1(call_num2, image->h*image->w*rate)},{},call_num1>>>AssignCluster(image,cluster);


		//auto v2_ = v1.init(call_num1, image->h*image->w*rate);
		//auto tpb2 = std::bind(&KmeansImage::Recenter, this, image, clusters);
		//auto tp2 = tf->newTask<decltype(tpb2)>("Recenter" + std::to_string(i), tpb2);
		//Guard*       g2 = gs->newGuard("GuardRecenter" + std::to_string(i), { v2_ }, tp2, {  });
		<<<g2,{v1(call_num1, image->h*image->w*rate)},{}>>>Recenter(image,cluster);

		//call_num2 = call_num1;
		#pragma call_num {call_num2 = call_num1;}
	}

	//auto tp3 = tf->newTask<void(*)(void *), void *>("End", &nullfun, NULL);
	//auto tp3status = new __fluid__<bool>(tp3->parent->isfinished());
	//auto v3 = new ValveEQ<bool>(tp3status, true);
	//gs->sync(tp3);
	Task::sync();

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



void KmeansImage::segmentImage_stable(RgbImage* image, Clusters* clusters, int n) {

	auto tf = new TaskFactory();
	auto gs = new AggressiveGS(4);
	auto tp0 = tf->newTask<void(*)(void *), void *>("Begin", &nullfun, NULL);
	auto tp0status = new __fluid__<bool>(tp0->isfinished());
	auto v0 = new ValveEQ<bool>(tp0status, true);

	tp0->run();

	double  rate = this->rate;
	std::cout << rate << std::endl;

	for (int i = 0; i < n; ++i) {
		loopindicator = "loop" + std::to_string(i);
		int* counter = new int(0);
		auto tpb1 = std::bind(&KmeansSchemeFluidXS::AssignCluster_stable, this, image, clusters, n, std::placeholders::_1);
		auto tp1 = tf->newTask<decltype(tpb1), int*>("AssignCluster" + std::to_string(i), tpb1, counter);
		Guard* g1 = gs->newGuard("GuardAssignCluster" + std::to_string(i), { v0 }, tp1, {});

		auto tp1status = new __fluid__<int>(counter);

		auto v1 = new ValveGT<int>(tp1status, image->h*image->w*rate);

		auto tpb2 = std::bind(&KmeansSchemeFluidXS::Recenter, this, image, clusters);
		auto tp2 = tf->newTask<decltype(tpb2)>("Recenter" + std::to_string(i), tpb2);
		Guard*       g2 = gs->newGuard("GuardRecenter" + std::to_string(i), { v1 }, tp2, {});
		
		tp0status = new __fluid__<bool>(tp2->isfinished());
		v0 = new ValveEQ<bool>(tp0status, true);
	}

	auto tp3 = tf->newTask<void(*)(void *), void *>("End", &nullfun, NULL);
	auto tp3status = new __fluid__<bool>(tp3->parent->isfinished());
	auto v3 = new ValveEQ<bool>(tp3status, true);
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
	this->sum = sum;

	for (int y = 0; y < image->h; y++) {
		for (int x = 0; x < image->w; x++) {
			image->pixels[y][x].r = clusters->centroids[image->pixels[y][x].cluster].r;
			image->pixels[y][x].g = clusters->centroids[image->pixels[y][x].cluster].g;
			image->pixels[y][x].b = clusters->centroids[image->pixels[y][x].cluster].b;
		}
	}
}
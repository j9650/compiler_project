CXX=/home/mdl/hzj5142/soft/install4/gcc-7.0.1/bin/g++

BT_PATH=../src
ROAR_PATH=/home/mdl/hzj5142/soft/install5/CRoaring/include/roaring
SDSL_PATH=/home/mdl/hzj5142/soft/install5/sdsl-lite/lib
FISH_PATH=/home/mdl/hzj5142/soft/install5/Jellyfish/lib
TCLAP_PATH=./include

CXXFLAGS=-g -std=c++11 -pthread -Wall -O3
#CXXFLAGS+=-I$(HOME)/include -I$(BT_PATH) -I$(HOME)/include/jellyfish-2.2.6/ -I$(TCLAP_PATH)
#NVCCFLAGS+=-I$(HOME)/include -I$(BT_PATH) -I/home/mdl/hzj5142/soft/install5/CRoaring/include -I/home/mdl/hzj5142/soft/install5/Jellyfish/include/jellyfish-2.2.5 -I$(TCLAP_PATH)
NVCCFLAGS+=-I/home/mdl/hzj5142/soft/install5/CRoaring/include
CXXFLAGS+=-I$(HOME)/include -I$(BT_PATH) -I/home/mdl/hzj5142/soft/install5/CRoaring/include -I/home/mdl/hzj5142/soft/install5/Jellyfish/include/jellyfish-2.2.5 -I/home/mdl/hzj5142/soft/install5/sdsl-lite/include -I$(TCLAP_PATH)

#LD_LIB=-L $(ROAR_PATH) -L $(SDSL_PATH) -L $(FISH_PATH)
LD_LIB=-L /home/mdl/hzj5142/soft/install5/CRoaring/lib -L $(SDSL_PATH) -L $(FISH_PATH)
LD_FLAG=-lroaring -lsdsl -ldivsufsort -ldivsufsort64 -ljellyfish-2.0 -lz -lpthread

#TBB_FLAG=-I$(TBB_INCLUDE)
#TBB_LIB=-L $(TBB_LIBRARY_RELEASE)
#TBB_LD=-ltbb

VPATH=$(BT_PATH)

all: sbuild squery

sbuild: BF.o bfcluster.o cluster.o utilities.o bfmain.o util.o Kmers.o BloomTree.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LD_LIB) $(LD_FLAG)

squery: BF.o bfquery.o util.o Kmers.o BloomTree.o Query.o Count.o abc.o cbd.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LD_LIB) -L /home/mdl/hzj5142/cuda-6.5/lib64 $(LD_FLAG) -lcuda -lcudart
#	$(CXX) $(CXXFLAGS) -o $@ $^ $(LD_LIB) -L/home/mdl/hzj5142/cuda-6.5/lib64 $(LD_FLAG) -lcuda -lcudart
#	$(CXX) $(CXXFLAGS) -o $@ $^ $(LD_LIB) $(LD_FLAG) 

pcompress: BF.o parallel_compress.o util.o BloomTree.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LD_LIB) $(TBB_LIB) $(LD_FLAG) $(TBB_LD)

parallel_compress.o: parallel_compress.cc 
	$(CXX) $(CXXFLAGS) $(TBB_FLAG) -c -o $@ $^

abc.o: abc.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^

cbd.o: cbd.cu
	$(NVCC) -ccbin /home/mdl/hzj5142/soft/install4/gcc-4.8.5/bin/g++ -c -arch=sm_35 $(NVCCFLAGS) cbd.cu
#	$(NVCC) $(NVCCFLAGS) -c -arch=sm_35 abc.cu

Query.o: Query.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^

Build.o: Build.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^

BloomTree.o: BloomTree.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^

Count.o: Count.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^

Kmers.o: Kmers.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^

util.o: util.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^

BF.o: BF.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^

bfmain.o: main.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^

bfcluster.o: bfcluster.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^

cluster.o: cluster.c
	$(CXX) $(CXXFLAGS) -c -o $@ $^

utilities.o: utilities.c
	$(CXX) $(CXXFLAGS) -c -o $@ $^


clean:
	rm -f *.o
	rm -f sbuild
	rm -f pcompress
	rm -f squery


#install:
#	mv bfcluster ${HOME}/${MACHTYPE}/bin


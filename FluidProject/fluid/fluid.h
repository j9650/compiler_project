#pragma once
#include <unordered_map>
#include <vector>
/** Fluid Variable, realy carries the data...
 *  read, write.
 *
 *  Note: the fluid variable only monitors its own variable, not the array.
 *
 * */

template<typename T>
class Fluid{
private:
public:
	//  Three Versions: 2 stable versions and 1 current changing version.

	T* prev; // 1. prev -- previous stable version
	T* curr; // 2. curr -- the current stable version
	T* next; // 3. next -- all writes are written in this version.

	T* mem[3];

	// make stable version.
	// should it be locked?? performance will totally waste.
	void sync(){*prev = *curr; *curr = *next;}

	//reset the fluid counter...
	void refluid(){
		writecount = 0;
	}

	T  Prev(){return *prev;} //only used in valves

	T  Curr(){return *curr;} //only used in valves

	T& read(){return *next;} //read the currently changing one?

	T* write(T& t){
		*(next) = t;
	};

	T& write(){
		std::cout <<"Current write count: " << writecount++ << std::endl;
		return *next;
	}

	int writecount;

	Fluid(){
		writecount = 0;
		prev = mem[0] = new T;
		curr = mem[1] = new T;
		next = mem[2] = new T;
	}

	Fluid(T* t){
		writecount = 0;
		prev = mem[0] = new T;
		curr = mem[1] = new T;
		next = mem[2] = new T;

		*(mem[2]) = *(mem[1]) = *(mem[0]) = *t;
	}

	~Fluid(){
		delete [] mem;
	}
};


// code deprecated...
class fluid{

public:
	virtual void* get() = 0;
	virtual void* value() = 0;

	virtual void * operator->() = 0;

};

template<typename T>
class __fluid__ : public fluid {
private:
	uint32_t fid;
public:
	T* p;
	__fluid__(T* _p) {
		p = _p;
	}
	T* get() {
		return p;
	}
	T* value() {
		return p;
	}

	T* operator->(){
		return p;
	}
};

class FluidFactory{
	std::unordered_map<int, fluid*> fluidMap;
public:
	template<typename T>
	__fluid__<T> * newFluid(T* p){

	}
};


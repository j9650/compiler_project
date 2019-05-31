#pragma once

#include "thread.h"
#include "fluid.h"
class ValveThread : public StoppableThread {
public:
	template<typename FunctionType, typename ... Args>
	ValveThread(FunctionType fun, Args ... args){
		thread = std::thread(fun, args...);
		//123//pthread_create(&thread, NULL, fun, args...);
		threadhandle = thread.native_handle();
		std::cout << "Thread Running" << std::endl;
	}
};

class Valve{
public:
	virtual bool check() = 0;
};


template<typename T>
class ValveLT: public Valve{
private:
	T current;
	T cond;
	__fluid__<T>* f;
public:


	ValveLT() {

	}
	ValveLT(__fluid__<T>* _f, T para) {
		f = _f;
		cond = para;
	}
	ValveLT* init(__fluid__<T>* _f, T para) {
		return new ValveLT<T>(_f, para);
	}

	bool check() {
		current = f->value();
		if ( current <= cond) {
			return true;
		}
		else
			return false;
	}

	T* get() {
		return &current;
	}
};

template<typename T>
class ValveGT : public Valve {
private:
	T current;
	T cond;
	__fluid__<T>* f;
public:


	ValveGT() {

	}
	ValveGT(__fluid__<T>* _f, T para) {
		f = _f;
		cond = para;
	}
	ValveGT* init(__fluid__<T>* _f, T para) {
		return new ValveGT<T>(_f, para);
	}

	bool check() {
		current = *(static_cast<T*>(f->value()));
		if (current >= cond) {
			return true;
		}
		else {
			//std::cout << static_cast<T*>(f->value()) << " look at the valve " << cond << std::endl;
			return false;
		}
	}

	T* get() {
		return &current;
	}
};

template<typename T>
class ValveGT_vec : public Valve {
private:
	std::vector<T> current;
	T cond;
	__fluid__<std::vector<T>>* f;
public:


	ValveGT_vec() {

	}
	ValveGT_vec(__fluid__<std::vector<T>>* _f, T para) {
		f = _f;
		cond = para;
	}
	ValveGT_vec* init(__fluid__<std::vector<T>>* _f, T para) {
		return new ValveGT_vec<T>(_f, para);
	}

	bool check() {
		current = *(static_cast<std::vector<T>*>(f->value()));
		auto ccc=current[0];
		for(int i=1; i<current.size(); i++)
		{
			ccc += current[i];
		}
		if (ccc >= cond) {
		//std::cout << "ccc: " << ccc << std::endl;
			return true;
		}
		else
			return false;
	}

	std::vector<T>* get() {
		return &current;
	}
};

template<typename T>
class ValveEQ : public Valve {
private:
	T current;
	T cond;
	__fluid__<T>* f;
public:


	ValveEQ() {

	}
	ValveEQ(__fluid__<T>* _f, T para) {
		f = _f;
		cond = para;
	}
	ValveEQ* init(__fluid__<T>* _f, T para) {
		return new ValveEQ<T>(_f, para);
	}

	bool check() {
		current = *(f->value());
		if (current == cond) {
			return true;
		}
		else
			return false;
	}

	T* get() {
		current = *(f->value());
		return &current;
	}
};

template<typename T>
class ValveST : public Valve {
private:
	T current;
	T cond;
	__fluid__<T>* f;
public:


	ValveST() {

	}
	ValveST(__fluid__<T>* _f, T para) {
		f = _f;
		cond = para;
	}
	ValveST* init(__fluid__<T>* _f, T para) {
		return new ValveST<T>(_f, para);
	}

	bool check() {
		current = *(f->value());
		if (current >= cond) {
			return true;
		}
		else
			return false;
	}

	T* get() {
		current = *(f->value());
		return &current;
	}
};

template<typename T, typename... Ts>
class ValveSB : public Valve {
private:
	T current;
	__fluid__<T>* f;
	std::tuple<Ts...> args1;
	bool tt;
public:


	ValveSB() {

	}
	ValveSB(__fluid__<T>* _f, Ts... args) {
		f = _f;
		//cond = para;
		args1 = (std::make_tuple(args...));
	}
	ValveSB* init(__fluid__<T>* _f, Ts... args) {
		return new ValveSB<T, Ts...>(_f, args...);
	}

 	template<std::size_t ...I>
 	void call_func(std::index_sequence<I...>)
 	{ tt=current->stable_check(std::get<I>(args1)...); }

 	//void delayed_dispatch()
 	//{ call_func(std::index_sequence_for<Args...>{}); }

 	//template<std::size_t ...I>
	bool check() {
		current = *(f->value());
		call_func(std::index_sequence_for<Ts...>{});
		return tt;
		//return current->stable_check(get<I>(args1)...);
		//return current->stable_check(cond);
		//return current->stable_check(args1);
		//return std::apply(&(current->stable_check),args1);
		//return std::apply(std::bind(&stable_check,current),args1);
		//if (current >= cond) {
		//	return true;
		//}
		//else
		//	return false;
	}

	T* get() {
		current = *(f->value());
		return &current;
	}
};
/*
template<typename T, typename Ts>
class ValveSB : public Valve {
private:
	T current;
	__fluid__<T>* f;
	Ts args1;
public:


	ValveSB() {

	}
	ValveSB(__fluid__<T>* _f, Ts args) {
		f = _f;
		//cond = para;
		args1 = args;
	}
	ValveSB* init(__fluid__<T>* _f, Ts args) {
		return new ValveSB<T, Ts>(_f, args);
	}


 	//void delayed_dispatch()
 	//{ call_func(std::index_sequence_for<Args...>{}); }

	bool check() {
		current = *(f->value());
		return current->stable_check(args1);
		//return current->stable_check(args1);
		//return std::apply(&(current->stable_check),args1);
		//return std::apply(std::bind(&stable_check,current),args1);
		//if (current >= cond) {
		//	return true;
		//}
		//else
		//	return false;
	}

	T* get() {
		current = *(f->value());
		return &current;
	}
};*/

template<typename T>
class ValvePerc : public Valve {
protected:
	T current;
	T cond;
	double perc;
	__fluid__<T>* f;
public:
	ValvePerc() {

		perc = 1;
	}

	ValvePerc(__fluid__<T>* _f, T para, double perc_){
		f = _f;
		cond = para;
		perc = perc_;
	}

	virtual bool check() = 0;

	T* get() {
		return &current;
	}
};
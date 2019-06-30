#pragma once
#include <iostream>
#include <cassert>
#include <list>
#include <set>
#include "guard.h"
#include "data.h"
//https://stackoverflow.com/questions/16868129/how-to-store-variadic-template-arguments

namespace helper
{
	template <int... Is>
	struct index {};

	template <int N, int... Is>
	struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};

	template <int... Is>
	struct gen_seq<0, Is...> : index<Is...> {};
}

template <typename F, typename... Ts>
class Action
{
private:
	F f;
	std::tuple<Ts...> args;
	F f1;
	std::tuple<Ts...> args1;
public:
	//template <typename F, typename... Args>
	//template <typename F, typename... Ts>
	Action(F func, Ts... args)
			: f(std::forward<F>(func)),
			  args(std::forward<Ts>(args)...),
			  f1(func),
			  args1(std::make_tuple(args...))
	{}

	Action(F&& func, Ts&&... args)
		: f(std::forward<F>(func)),
		args(std::forward<Ts>(args)...)
	{}

	template <typename... Args, int... Is>
	void func(std::tuple<Args...>& tup, helper::index<Is...>)
	{
		f(std::get<Is>(tup)...);
	}

	template <typename... Args>
	void func(std::tuple<Args...>& tup)
	{
		func(tup, helper::gen_seq<sizeof...(Args)>{});
	}

	void act()
	{
		//func(args);
		std::apply(f1,args1);
	}
};

class Task {
public:
	virtual void run() = 0;
	//virtual void callback() = 0;
	//virtual void setflag(bool* flag) = 0;
};



extern class Guard guard;
class GuardTask :public Task{
public:
	// if we detached a thread,
	// it seems that we won't be able to know if a task is finished,
	// here is the hook for us.
	Guard* guard;
	bool finish;
	GuardTask * parent;
	std::set<Data*> input;
	std::set<Data*> output;
	virtual void run() = 0;
	virtual void setGuard(Guard* g ) = 0;
	virtual bool* isfinished() = 0;
	//virtual void setflag(bool* flag) = 0;
};


static void nullfun(void * p) {
	return;
}
template <typename F, typename... Ts>
class Tasklet : public GuardTask {
private:
	Action<F, Ts...>* act;

	std::string name;


	

	int agn;
public:

	Tasklet(std::string taskname, int _agn, F f, Ts... ts) : name(taskname) {
		parent = NULL;
		act = new Action<F, Ts...>(f, ts...);
		guard = NULL;
		finish = false;
		agn = _agn;
	}

	Tasklet(std::string taskname, F f, Ts... ts) : name(taskname) {
		parent = NULL;
		act = new Action<F, Ts...>(f, ts...);
		guard = NULL;
		finish = false;
		agn = 0;
	}

	Tasklet(std::string taskname, std::set<Data*> input_, std::set<Data*> output_, F f, Ts... ts) : name(taskname) {
		parent = NULL;
		input = input_;
		output = output_;
		act = new Action<F, Ts...>(f, ts...);
		guard = NULL;
		finish = false;
		agn = 0;
	}

	void run(){

		SyncLogger::print("before act");
		//std::cout << "before act" << std::endl;
		act->act();
		//std::cout << "after act" << std::endl;
		//SyncLogger::print("after act");
		//SyncLogger::print("TLET>>>>>>>>>>", guard->name, "Wall: ", (guard->TaskTimerWT).stop(), "CPU:", (guard->TaskTimerCT).stop());

		// when finish, needs to update the signal queue.


		finish = true;
		//std::cout << "after act" << std::endl;

	}


	void setGuard(Guard *g) {
		guard = g;
		for (Data *output_data : output) {
			output_data->set_producer(g);
		}
		for (Data *input_data : input) {
			input_data->set_consumer(g);
		}
	}
	
	bool* isfinished() {
		return &finish;
	}
	~Tasklet() {
		delete act;
	}
};


class TaskFactory {
protected:
	//std::list<GuardTask*> tasklist;
	GuardTask * parentTask;
	int numConcurrentTask;
public:
	TaskFactory() {
		parentTask = NULL;
	}
    TaskFactory(int _numConcurrentTask) {
        parentTask = NULL;
        numConcurrentTask = _numConcurrentTask;
    }
	template <typename F, typename... Ts>
	GuardTask* newTask(std::string taskname, F f, Ts... ts) {
		auto task = new Tasklet<F, Ts...>(taskname, f, ts...);
		//tasklist.insert(task);
		task->parent = parentTask;
		parentTask = task;
		return task;
	}
	template <typename F, typename... Ts>
	GuardTask* newTask(std::string taskname, std::set<Data*> input_, std::set<Data*> output_, F f, Ts... ts) {
		auto task = new Tasklet<F, Ts...>(taskname, input_, output_, f, ts...);
		//tasklist.insert(task);
		task->parent = parentTask;
		parentTask = task;
		return task;
	}
};


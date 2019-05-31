#pragma once
#include <queue>
#include <set>
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class queue_t {
public:
	virtual T pop()		 = 0;
	virtual bool empty() = 0;
	virtual void pop(T&) = 0;
	virtual void push(const T& item) = 0;
	//virtual void push(T&& item) = 0;
};

//https://juanchopanzacpp.wordpress.com/2013/02/26/concurrent-queue-c11/
template <typename T>
class SyncQueue : public queue_t<T>
{
public:
	bool empty() {
		bool empty_ = false;
		std::unique_lock<std::mutex> mlock(mutex_);
		empty_ = queue_.empty();
		mlock.unlock();
		return empty_;
	}
	T pop()
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.empty())
		{
			cond_.wait(mlock);
		}
		auto item = queue_.front();
		queue_.pop();
		return item;
	}

	void pop(T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.empty())
		{
			cond_.wait(mlock);
		}
		item = queue_.front();
		queue_.pop();
	}

	void push(const T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		queue_.push(item);
		mlock.unlock();
		cond_.notify_one();
	}

	//void push(T&& item)
	//{
	//	
	//	std::unique_lock<std::mutex> mlock(mutex_);
	//	queue_.push(std::move(item));
	//	mlock.unlock();
	//	cond_.notify_one();
	//}

	std::mutex * getMutex() {
		return &mutex_;
	}
private:
	std::queue<T> queue_;
	std::mutex mutex_;
	std::condition_variable cond_;
};

template <typename T>
class Queue : public queue_t<T>
{
public:
	bool empty() {
		return queue_.empty();
	}

	T pop()
	{
		assert(!empty());
		auto item = queue_.front();
		queue_.pop();
		return item;
	}

	void pop(T& item)
	{
		assert(!empty());
		item = queue_.front();
		queue_.pop();
	}

	void push(const T& item)
	{
		queue_.push(item);
	}

	//void push(T&& item)
	//{
	//	queue_.push(std::move(item));
	//}
private:
	std::queue<T> queue_;
};



template <typename T>
class set_t {
public:
	virtual bool empty() = 0;
	//virtual void insert(const T& item) = 0;
	virtual void insert(T item) = 0;
	virtual std::set<T>& getSet() = 0;
protected:
	std::set<T> set_;

};

template <typename T>
class SyncSet : public set_t<T>
{
	std::set<T> set_copy;
	std::mutex mutex_;
public:
	bool empty() {
		bool empty_ = false;
		std::unique_lock<std::mutex> mlock(mutex_);
		empty_ = this->set_.empty();
		mlock.unlock();
		return empty_;
	}

	//void insert(const T& item)
	//{
	//	std::unique_lock<std::mutex> mlock(mutex_);
	//	set_.insert(item);
	//	mlock.unlock();
	//}

	void insert(T item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
        this->set_.insert(item);

		mlock.unlock();
	}
	
	std::set<T>& getSet() 
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		set_copy.clear(); for (auto & s : this->set_) set_copy.insert(s);
		mlock.unlock();
		std::cout << set_copy.size() << std::endl;
		return set_copy;
	}


};


template <typename T>
class Set : public set_t<T>
{
public:
	bool empty() {
		return  this->set_.empty();
	}

	//void insert(const T& item)
	//{
	//	set_.insert(item);
	//}

	void insert(T item)
	{
        this->set_.insert(item);
	}
	std::set<T>& getSet() {
		return this->set_;
	}
};

//https://stackoverflow.com/questions/4792449/c0x-has-no-semaphores-how-to-synchronize-threads
class Semaphore {
public:
	Semaphore(int count_ = 0)
		: count(count_) {}

	inline void notify()
	{
		std::unique_lock<std::mutex> lock(mtx);
		count++;
		cv.notify_one();
	}

	inline void wait()
	{
		std::unique_lock<std::mutex> lock(mtx);

		while (count == 0) {
			cv.wait(lock);
		}
		count--;
	}

private:
	std::mutex mtx;
	std::condition_variable cv;
	int count;
};

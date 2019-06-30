#pragma once
#include "guard.h"

class Guard;

// template<typename T>
class Data{
public:
	Data() { version_ = 0; tag_ = 0; };
	~Data();

	// T* data() { return data_; }
	// void set_data(T* value) { data_ = value; }
	void* data() { return data_; }
	void set_data(void* value) { data_ = value; }

	int size() { return size_; }
	void set_size(int value) { size_ = value; }

	Guard* producer() { return producer_; }
	void set_producer(Guard* value) { producer_ = value; }

	Guard* consumer() { return consumer_; }
	void set_consumer(Guard* value) { consumer_ = value; }

	int tag() { return tag_; }
	void set_tag(int value) { tag_ = value; }

	int version() { return version_; }
	void set_version(int value) { version_ = value; }

private:
	// T* data_;
	void* data_;
	int size_;
	Guard* producer_;
	Guard* consumer_;
	int tag_;
	int version_;
};
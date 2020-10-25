#ifndef MUDUO_BASE_SINGLETON_H
#define MUDUO_BASE_SINGLETON_H

#include <pthread.h>
#include <assert.h>
#include <stdlib.h>

template<typename T>
class Singleton {
public:
	Singleton() = delete;
	~Singleton() = delete;

	static T& instance() {
		pthread_once(&ponce_, &Singleton::init);
		assert(value_ != NULL);
		return *value_;
	}
	
private:
	static void init() {
		value_ = new T();
		if (1) {
			atexit(destroy);
		}
	}

	static void destroy() {
		typedef char T_must_be_complete_type[sizeof(T) == 0? -1 : 1];
		T_must_be_complete_type t;
		delete value_;
		value_ = NULL;

	}



	static pthread_once_t ponce_;
	static T* value_;
};


template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;

#endif

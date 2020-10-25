#ifndef MUDUO_BASE_THREADLOCAL_H
#define MUDUO_BASE_THREADLOCAL_H

#include <pthread.h>
#include "Mutex.h"

template<typename T>
class ThreadLocal {
public:
	ThreadLocal() {
		pthread_key_create(&pkey_, &ThreadLocal::destructor);
	}
	
	~ThreadLocal() {
		pthread_key_delete(pkey_);
	}

	T& value() {
		T *perThreadValue = static_cast<T*> (pthread_getspecific(pkey_));
		if (!perThreadValue) {
			T* newObj = new T();
			pthread_setspecific(pkey_, newObj);
			perThreadValue = newObj;
		}
		return *perThreadValue;
	}
private:
	static void destructor(void *x) {
		T* obj = static_cast<T*> (x);
		typedef char T_must_complete_type[sizeof(T) == 0 ? -1 : 1];
		T_must_complete_type dummy;
		delete obj;
	}

	pthread_key_t pkey_;
};






#endif

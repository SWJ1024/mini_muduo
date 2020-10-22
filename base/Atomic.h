#ifndef MUDUO_BASE_ATOMIC_H
#define MUDUO_BASE_ATOMIC_H


#include <stdint.h>
#include <atomic>

template<typename T>
class AtomicIntegerT {
public:
    AtomicIntegerT() : val(0) {}
    T get() {
        return std::atomic_load(&val);
    }

    T getAndAdd(T x) {
        return std::atomic_fetch_add(&val, x);
    }


    T addAndGet(T x) {
        return getAndAdd(x) + x;
    }

    T incrementAndGet() {
        return addAndGet(1);
    }

    T decrementAndGet() {
        return addAndGet(-1);
    }

    void add(T x) {
        getAndAdd(x);
    }

    void increment() {
        incrementAndGet();
    }

    void decremnt() {
        decrementAndGet();
    }

    T getAndSet(T x) {
        return std::atomic_exchange(&val, x); 
    }
private:
    volatile std::atomic<T> val;
};

typedef AtomicIntegerT<int32_t> AtomicInt32;
typedef AtomicIntegerT<int64_t> AtomicInt64;

#endif  //MUDUO_BASE_ATOMIC_H

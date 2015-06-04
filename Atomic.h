#ifndef ATOMIC_H
#define ATOMIC_H

#include <boost/noncopyable.hpp>
#include <stdint.h>

namespace muduo
{
    namespace detail
    {
        //以原子方式操作数据value_
        template<typename T>
        class AtomicInterT: boost::noncopyable
        {
            public:
                AtomicInterT()
                    : value_(0)
                {

                }
                AtomicInterT(T v)
                    : value_(v)
                {

                }

                //返回value_
                T get() const
                {
                    return __sync_val_compare_and_swap(const_cast<volatile T*>(&value_), 0, 0);    
                }

                //对value_加x并返回原值
                T getAndAdd(T x)
                {
                    return __sync_fetch_and_add(&value_, x);
                }

                T addAndGet(T x)
                {
                    return getAndAdd(x) + x;
                }

                T incrementAndGet()
                {
                    return addAndGet(1);
                }

                void add(T x)
                {
                    getAndAdd(x);
                }

                void increment()
                {
                    incrementAndGet();
                }

                void decrement()
                {
                    getAndAdd(-1);
                }

                T getAndSet(T newValue)
                {
                    return __sync_lock_test_and_set(&value_, newValue);
                }
            private:
                volatile T value_;
        };
    }

    typedef detail::AtomicInterT<int32_t> AtomicInt32;
    typedef detail::AtomicInterT<int64_t> AtomicInt64;
}

#endif

#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "Timestamp.h"

namespace muduo
{
    typedef boost::function<void()> TimerCallback;
}

#endif

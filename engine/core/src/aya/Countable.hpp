#pragma once
#include "aya/atomic.hpp"
#include "aya/Declarations.hpp"

namespace Aya
{

namespace Diagnostics
{

template<typename T>
class RBXBaseClass Countable
{
    static Aya::atomic<int> count;

public:
    static long getCount()
    {
        return count;
    }
    ~Countable()
    {
        --count;
    }

protected:
    Countable()
    {
        ++count;
    }
};

template<class T>
Aya::atomic<int> Countable<T>::count;

} // namespace Diagnostics

} // namespace Aya

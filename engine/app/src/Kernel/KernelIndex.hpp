

#pragma once

#include "aya/Debug.hpp"

namespace Aya
{

class KernelIndex
{
protected:
    int kernelIndex;

public:
    bool indexInKernel() const
    {
        return (kernelIndex != -1);
    }

    KernelIndex()
        : kernelIndex(-1)
    {
    }

    ~KernelIndex()
    {
        RBXASSERT(!indexInKernel());
    }
};

} // namespace Aya
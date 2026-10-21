
// d9mz - might be something wrong with my sdk, but this is needed to compile shadercompiler - it sucks
#ifndef PTRDIFF_MAX
#define PTRDIFF_MAX 9223372036854775807
#endif

#include "aya/Debug.hpp"
#include "RbxAssert.hpp"
#include "RbxFormat.hpp"
#include <algorithm>

const int CRASHONASSERT = 255;

void ReleaseAssert(int channel, const char* msg)
{
    if (channel == CRASHONASSERT)
        RBXCRASH(msg);
    else
        FLog::FastLog(channel, msg, 0);
}

namespace Aya
{
// overload this in the debugger to pass by the crash
volatile bool Debugable::doCrashEnabled = true;

void Debugable::doCrash()
{
    if (doCrashEnabled)
    {
        DebugBreak();
    }
}

#pragma optimize("", off)

void Debugable::doCrash(const char* message)
{
#if defined(RBX_PLATFORM_DURANGO)
    OutputDebugString("ASSERTION FAILED: ");
    OutputDebugString(message);
    OutputDebugString("\n");
    if (!IsDebuggerPresent())
        return;
#endif
    if (doCrashEnabled)
    {
        DebugBreak();
    }
}

#pragma optimize("", on)

} // namespace Aya


#include "CheckDbg.hpp"

#include <boost/functional/hash/hash.hpp>
#include <windows.h>

static volatile int g_counter = 0;

// bloxblox - these changes made to account for no more inline-asm may be fucked

static __forceinline DWORD GetFS()
{
    // Use NtCurrentTeb() to access the TIB's linear address
    return reinterpret_cast<DWORD>(NtCurrentTeb());
}

static __forceinline DWORD GetFlag(DWORD f)
{
    DWORD* ptr = reinterpret_cast<DWORD*>(f + 0x30);
    return *ptr;
}

__declspec(noinline) bool isDbg1()
{
    DWORD fs = GetFS();
    g_counter += boost::hash_value(fs);
    g_counter += boost::hash_value(fs + g_counter);
    DWORD flag = GetFlag(fs);
    g_counter += boost::hash_value(fs + flag);
    return (flag & 0x00010000) ? true : false;
}

__declspec(noinline) bool isDbg2()
{
    DWORD fs = GetFS();
    g_counter += boost::hash_value(rand() + fs);
    DWORD flag = GetFlag(fs);
    g_counter += boost::hash_value((fs << 3) + flag);
    bool v = (flag & 0x00010000) ? true : false;
    g_counter += boost::hash_value((fs << 2) + flag + g_counter);
    return v;
}

__declspec(noinline) bool isDbg3()
{
    DWORD fs = GetFS();
    g_counter += boost::hash_value(rand() + g_counter);
    DWORD flag = GetFlag(fs);
    g_counter += boost::hash_value(rand() + g_counter + (rand() << 3));
    return (flag & 0x00010000) ? true : false;
}